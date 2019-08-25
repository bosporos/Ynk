//
// file ynk/App.cc
// author Maximilien M. Cura
//

#include <Ynk/App.hh>

using namespace Ynk::App;

Runner * Runner::instance ()
{
    if (Runner::_instance == nullptr) {
        Runner::_instance = new Runner ();
    }

    return Runner::_instance;
}

void Runner::fix_runner (StubFactory * runner)
{
    this->stub_runner = runner;
}

int Runner::launch_runner (int argc, char ** argv)
{
    if (this->stub_runner == nullptr) {
        std::fprintf (stderr, "ynk: can't launch runner: no fixed runner");
        std::fprintf (stderr, "ynk: aborting");
        std::abort ();
    }

    Stub * stub = this->stub_runner->create_stub ();
    return stub->run (argc, argv, stub);
}

Runner::Runner ()
    : stub_runner { nullptr }
{}

bool Ynk::App::register_stub (StubFactory * sr)
{
    Runner::instance ()->fix_runner (sr);

    return true;
}
