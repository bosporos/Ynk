//
// file ynk/App.h
// author Maximilien M. Cura
//

#ifndef __YNK_APP
#define __YNK_APP

#include <cstdio>
#include <cstdlib>

namespace Ynk::App {
    struct Stub
    {
        virtual int run (int, char **) = 0;
    };

    struct StubRunner
    {
        virtual Stub * create_stub () = 0;
    };

    template <class T>
    struct StubRunnerImpl : public StubRunner
    {
        StubRunnerImpl ()
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

        StubRunner * stub_runner = nullptr;

        void fix_runner (StubRunner * runner)
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
            return stub->run (argc, argv);
        }

    private:
        Runner ()
            : stub_runner { nullptr }
        {}
    };

    bool register_stub (StubRunner * sr)
    {
        Runner::instance ()->fix_runner (sr);

        return true;
    }
}

#define YNK_APP()                                                 \
    namespace Ynk::App {                                          \
        struct StubImpl                                           \
            : public Ynk::App::Stub                               \
        {                                                         \
            static bool registered;                               \
                                                                  \
            int run (int, char **) override;                      \
        };                                                        \
    }                                                             \
                                                                  \
    bool Ynk::App::StubImpl::registered                           \
        = Ynk::App::register_stub (                               \
            new Ynk::App::StubRunnerImpl<Ynk::App::StubImpl> ()); \
                                                                  \
    int Ynk::App::StubImpl::run (int argc, char ** argv)

#endif /* !@__YNK_APP */
