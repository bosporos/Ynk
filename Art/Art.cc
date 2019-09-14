//
// file art/Art.cc
// author Maximilien M. Cura
//

// Grab all the definitions/declarations for Art + various utilities everything uses.
#include <Art/Art.hh>

// Concretizing the declarations from Art.hh
Ynk::Space<3, double> * Art::d3    = Ynk::Space<3, double>::instance (Ynk::SpaceType::Cartesian);
Ynk::Space<2, Ynk::i64> * Art::iq2 = Ynk::Space<2, Ynk::i64>::instance (Ynk::SpaceType::Cartesian);
Ynk::Vec<2, Ynk::i64> Art::window_size (Art::iq2, { 800_i64, 800_i64 });

/// Main function
//!
//! Normally, C/C++ programs start with a `int main(int argc, char ** argv)` function,
//! but when using Ynk, this is the appropriate way to define the entry point into
//! the program.
//!
//! `argc` and `argv` are actually being passed into this function invisibly by Ynk;
//! they may be used herein, it's just part of the C++ magic I had to use to make
//! this work. Additionally, a variable `application` is also being passed in.
YNK_APP (Test)
{
    bool flag_print_usage = false;
    if (argc == 2) {
        if (!strcmp (argv[1], "tui")) {
            // Calling with the argument "tui" will run the TUI debugging visualizer
            Art::TUIApp ();
        } else if (!strcmp (argv[1], "gl-sim")) {
            // Calling with the argument "gl-sim" will run the full OpenGL simulator
            Art::GLSimulationApp (application);
        } else if (!strcmp (argv[1], "gl-art")) {
            Art::GLArtApp (application);
        } else {
            // Invalid command specifed, so print usage
            flag_print_usage = true;
        }
    } else {
        // Wrong number of arguments, so print usage
        flag_print_usage = true;
    }

    if (flag_print_usage) {
        // Print usage message.
        if (argc <= 1) {
            Art::Error ("No command specified!");
        } else if (argc > 2) {
            Art::Error ("Too many arguments!");
        } else if (strcmp (argv[1], "help")) {
            Art::Error ("No valid command specified!");
        }

        print ("\n");
        println ("Usage: {} (tui|gl-sim|help)", argv[0]);
    }

    return 0;
}

// A bit of boilerplate
YNK_LAUNCH_APP (Test);
