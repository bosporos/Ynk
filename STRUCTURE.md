# Structure of the project

Everything relevant to the algorithm and the generative art itself will be in the
`art` directory; the `Ynk` directory is a mix of C++ framing magic, utilities,
ease-of-use support, and other things.

I will stress that it is not necessary to understand anything in the `Ynk` directories;
*especially* the files `Ynk/*.h` and `Ynk/lang/*.h`.

`Ynk/App.h` uses methods I picked up will trying to make a bare-bones testing framework
based on Googletest; the `Ynk/Bootstrap.*` are a shim to fix a strange bug I encountered,
and exist to get around C++ name mangling rules and bootstrap into `Ynk/App.h`, which
will launch the `YNK_APP()` defined in `Art/Art.cc`
