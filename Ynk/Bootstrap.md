# Documentation of the bootstrapping process

As the astute programmer will have noticed, the client application does not declare a `main()` function.
Rather, the client application of Ynk declares an `YNK_APP()` in the following manner:

```c++
YNK_APP(AppName)
{
    // ...

    //! `argc` and `argv` are still very much available
    auto conf = MyYnkAppInternalConfiguration::configure(argc, argv);

    // ...

    //! The return value is still an integer; still 0 = success, non-zero = error
    return 0;
}
```

On the surface, writing `YNK_APP()` is the same as writing `int main(int argc, char ** argv)`.
However, the actual mechanism, is rather more intricate.

The call chain resembles the following:

 - OS calls `main(argc, argv)` (defined in `Ynk/Bootstrap.c`)
 - `main(argc, argv)` calls `_ynk_bootstrap(argc, argv)` (defined in `Ynk/Bootstrap.cc`)
 - `_ynk_bootstrap(argc, argv)` calls `Ynk::App::Runner::instance() -> launch_runner(argc, argv)`, which calls the `YNK_APP()` "function"

However, there's a lot going on behind the scenes here. Firstly, there is the question of how `Ynk::App::Runner`
actually knows how to call YNK_APP, which is really the result of a rather clever bit of C++
I found in Google's Googletest testing framework; it works something like this:

 1. `YNK_APP(AppName)` expands to a `struct` scoped as `Ynk::App::StubImpl_AppName` which inherits from `Ynk::App::Stub`
 2. `Ynk::App::StubImpl_AppName` has two members:
    - A method named `run` with signature `(int, char**) -> int`
    - A static `bool` named registered
 3. After the declaration of the `struct`, there are two (well, one and a half) symbol definitions
    - Firstly, `Ynk::App::StubImpl_AppName::registered` is defined as the result of a call to `Ynk::App::register(Ynk::App:StubFactory *)`
    - Secondly, `Ynk::App::StubImpl_AppName::run()` is *partially* defined; it looks something like the following:

```c++
int Ynk::App::StubImpl_AppName::run(int argc, char ** argv)
```

Because it is the last line of the YNK_APP() macro, expanding the sample usage above would result in something like:

```c++
 namespace Ynk::App {
      struct StubImpl_AppName {
          // All this stuff...
      };
}
bool Ynk::App::StubImpl_AppName::registered = Ynk::App::register(new Ynk::App::StubFactoryImpl<Ynk::App::StubImpl_AppName> ());
int Ynk::App::StubImpl_AppName::run(int argc, char ** argv)
/* MACRO ENDS, CLIENT CODE CONTINUES */
{
    // ...

    //! `argc` and `argv` are still very much available
    auto conf = MyYnkAppInternalConfiguration::configure(argc, argv);

    // ...

    //! The return value is still an integer; still 0 = success, non-zero = error
    return 0;
}
```

`StubImpl_AppName::registered` is required because we need to take advantage of the fact that global variables, which
of course includes static class members, must be initialized before `main()` is called.

You may have noticed that I gave the signature of `register()` to be `(Ynk::App::StubFactory *) -> boolean`, whereas
it is called with what would appear to be a parameter of type `Ynk::App::StubFactoryImpl<Ynk::App::StubImpl_AppName> *`.
`StubFactoryImpl` is actually a subclass of `StubFactory`, and the whole procedure is designed so that `Ynk::App::Runner`'s
singleton instance will have that `StubFactoryImpl<StubImpl_AppName>`, which can create a `StubImpl_AppName` for `Runner`,
even though `Runner` has absolutely no idea what a `StubImpl_AppName` actually is: all it sees is a `Stub`.

Anyway, that's how the bootstrap process works.
