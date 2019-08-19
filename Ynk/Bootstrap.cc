//
// file ynk/Bootstrap.cc
// author Maximilien M. Cura
//

#include <Ynk/Bootstrap.hh>
#include <Ynk/App.hh>

#include <Ynk/Panic/Panicking.hh>

::Ynk::App::Runner * ::Ynk::App::Runner::_instance = nullptr;

int _ynk_bootstrap (int argc, char ** argv)
{
    try {
        return Ynk::App::Runner::instance ()->launch_runner (argc, argv);
    } catch (::Ynk::Panicking::_Uncatchable::_PanicUnwindingException ex) {
        return -1;
    }
}
