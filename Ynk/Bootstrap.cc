//
// file ynk/Bootstrap.cc
// author Maximilien M. Cura
//

#include <Ynk/Bootstrap.h>
#include <Ynk/App.h>

Ynk::App::Runner * Ynk::App::Runner::_instance = nullptr;

int _ynk_bootstrap (int argc, char ** argv)
{
    return Ynk::App::Runner::instance ()->launch_runner (argc, argv);
}
