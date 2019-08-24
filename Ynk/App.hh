//
// file ynk/App.hh
// author Maximilien M. Cura
//

#ifndef __YNK_APP
#define __YNK_APP

#include <Ynk/Utility.hh>

#include <cstdio>
#include <cstdlib>

// I recommend reading through Bootstrap.md before looking through this file, if you're
// not used to the trickeries that C++ programmers use to make the language do the things
// it wasn't meant to do

namespace Ynk::App {
    struct Stub
    {
        virtual int run (int, char **, Stub *) = 0;
    };

    struct StubFactory
    {
        virtual Stub * create_stub () = 0;
    };

    template <class T>
    struct StubFactoryImpl : public StubFactory
    {
        StubFactoryImpl ()
        {}

        Stub * create_stub () override
        {
            return new T ();
        }
    };

    struct Runner
    {
        static Runner * _instance;
        static Runner * instance ()
        {
            if (Runner::_instance == nullptr) {
                _instance = new Runner ();
            }

            return _instance;
        }

        StubFactory * stub_runner = nullptr;

        void fix_runner (StubFactory * runner)
        {
            this->stub_runner = runner;
        }

        int launch_runner (int argc, char ** argv)
        {
            if (this->stub_runner == nullptr) {
                std::fprintf (stderr, "ynk: can't launch runner: no fixed runner");
                std::fprintf (stderr, "ynk: aborting");
                std::abort ();
            }

            Stub * stub = this->stub_runner->create_stub ();
            return stub->run (argc, argv, stub);
        }

    private:
        Runner ()
            : stub_runner { nullptr }
        {}
    };

    bool register_stub (StubFactory * sr)
    {
        Runner::instance ()->fix_runner (sr);

        return true;
    }
}

#define YNK_APP_NAMED(_name) StubImpl_##_name
#define YNK_APP_BY_NAME(_name) ::Ynk::App::StubImpl_##_name
#define YNK_APP_NAME(_name) #_name

//! Uses YNK_UNUSED to silence compiler warnings about argc and argv being unused
//! parameters
#define YNK_APP(_name)                                                       \
    namespace Ynk::App {                                                     \
        struct YNK_APP_NAMED (_name)                                         \
            : public Ynk::App::Stub                                          \
        {                                                                    \
            static bool registered;                                          \
            static constexpr const char * const name = YNK_APP_NAME (_name); \
                                                                             \
            int run (int YNK_UNUSED, char ** YNK_UNUSED, Stub *) override;   \
        };                                                                   \
    }                                                                        \
                                                                             \
    int Ynk::App::YNK_APP_NAMED (_name)::run (int YNK_UNUSED argc, char ** YNK_UNUSED argv, YNK_UNUSED Ynk::App::Stub * application)

#define YNK_LAUNCH_APP(_name)                        \
    bool Ynk::App::YNK_APP_NAMED (_name)::registered \
        = Ynk::App::register_stub (                  \
            new Ynk::App::StubFactoryImpl<Ynk::App::YNK_APP_NAMED (_name)> ());

#endif /* !@__YNK_APP */
