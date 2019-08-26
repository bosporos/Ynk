//
// file art/Art.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>

#include <Art/PushRelabel.hh>
#include <Art/Model/Model.hh>

#include <Ynk/UX/ColorAdapters.hh>

using namespace Ynk;

Space<3, double> * Art::d3 = Space<3, double>::instance (SpaceType::Cartesian);
Space<2, i64> * Art::iq2   = Space<2, i64>::instance (SpaceType::Cartesian);
Vec<2, i64> Art::window_size (Art::iq2, { 800_i64, 800_i64 });

void Simulation ();

///! Main function
//!
//! Normally, C/C++ programs start with a `int main(int argc, char ** argv)` function,
//! but when using Ynk, this is the appropriate way to define the entry point into
//! the program.
//!
//! `argc` and `argv` are actually being passed into this function invisibly by Ynk;
//! they may be used herein, it's just part of the C++ magic I had to use to make
//! this work.
YNK_APP (Test)
{
    // Art::Init (argc, argv, application);
    Simulation ();

    return 0;
}

YNK_LAUNCH_APP (Test);

void Simulation ()
{
    Art::Bristle bristles[5] = {
        Art::Bristle (Art::d3->create_vec ({ 0, -1, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ -1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 1, 0, 0 }), 65336),
        Art::Bristle (Art::d3->create_vec ({ 0, 1, 0 }), 65336)
    };
    Art::Brush brush (
        bristles,
        5,
        UX::RGBA (0x9f, 0xa0, 0xff, 0xff));
    brush.position += Art::d3->create_vec ({ 0, 0, 0 });

    auto layer_size = Art::iq2->create_vec ({ 25, 25 });

    Art::PaperLayer pl (layer_size, Art::PaperConfiguration ());
    Art::WaterLayer wl (layer_size, &brush);

    Art::Notify ("Constructing PR network...");
    wl._pr_construct (&pl);
    Art::Notify ("Constructing PR network... done");

    Art::Notify ("Initializing PR network...");
    wl._pr_ready ();
    wl._pr_accrete (&pl);
    Art::Notify ("Initializing PR network... done");

    getchar ();

    for (int i = 0; i < 1000; i++) {
        if (!arc4random_uniform (4)) {
            brush.position += Art::d3->create_vec ({ 2.0 - arc4random_uniform (4),
                                                     2.0 - arc4random_uniform (4),
                                                     0 });
        }
        print ("\x1b[1;1H");
        println ("BRUSH {} {} {} TIME {}", brush.position[0], brush.position[1], brush.position[2], i);
        wl._pr_ready ();
        wl._pr_run ();

        wl._pr_accrete (&pl);
        for (i64 y = 0; y < layer_size[1]; y++) {
            for (i64 x = 0; x < layer_size[0]; x++) {
                print ("{}#{}",
                       UX::ANSI (UX::hsva (354, (float)wl.components[y][x]->hydrosaturation / 100.0, 0.86, 1)),
                       UX::ANSIRst ());
            }
            println ("");
        }
    }
}
