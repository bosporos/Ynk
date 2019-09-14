//
// file art/TUIApp/TUIApp.cc
// author Maximilien M. Cura
//

// Grab all the definitions/declarations for Art + various utilities everything uses.
#include <Art/Art.hh>

#include <Art/PushRelabel/PushRelabel.hh>
#include <Art/Model/Model.hh>

#include <Ynk/UX/ColorAdapters.hh>

using namespace Ynk;

void Art::TUIApp ()
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
        UX::RGBA (0xff, 0x4b, 0x3e, 0x10));
    brush.position += Art::d3->create_vec ({ 0, 0, 0 });

    auto layer_size = Art::iq2->create_vec ({ 25, 25 });

    Art::PaperLayer pl (layer_size, Art::PaperConfiguration ());
    Art::WaterLayer wl (layer_size, &brush);
    Art::TintLayer tl (layer_size, &brush);

    Art::Notify ("Constructing WaterLayer PR network...");
    wl._pr_construct (&pl);
    Art::Notify ("Constructing WaterLayer PR network... done");

    Art::Notify ("Constructing TintLayer PR network...");
    // Does nothing atm
    tl._pr_construct (&pl, &wl);
    Art::Notify ("Constructing TintLayer PR network... done");

    Art::Notify ("Initializing WaterLayer PR network...");
    Art::Notify ("Readying...");
    wl._pr_ready ();
    Art::Notify ("Accreting initial layer (filling hydrosaturation fields)...");
    wl._pr_accrete (&pl);
    Art::Notify ("Initializing WaterLayer PR network... done");

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

        tl._pr_ready (&pl, &wl);
        tl._pr_run ();

        wl._pr_accrete (&pl);
        tl._pr_accrete (&pl, &wl);
        // Basically, we print a bunch of debugging screens
        for (i64 y = 0; y < layer_size[1]; y++) {
            for (i64 x = 0; x < layer_size[0]; x++) {
                print ("{}#{}",
                       UX::ANSI (UX::hsva (317, (float)wl.components[y][x]->hydrosaturation / 100.0, 0.86, 1)),
                       UX::ANSIRst ());
            }
            print ("    ");
            for (i64 x = 0; x < layer_size[0]; x++) {
                const char * str = (tl.components[y][x]->tint.quantity > 1000_u64) ? "!" : "#";
                print ("{}{}{}",
                       UX::ANSI (UX::hsva (212, (float)tl.components[y][x]->tint.quantity / 96.0, 0.86, 1)),
                       str,
                       UX::ANSIRst ());
            }
            print ("    ");
            for (i64 x = 0; x < layer_size[0]; x++) {
                UX::RGBA col     = tl.components[y][x]->tint.color.blend (pl.components[y][x]->tint.color);
                const char * str = "#";
                print ("{}{}{}",
                       UX::ANSI (col),
                       str,
                       UX::ANSIRst ());
            }
            print ("    ");
            for (i64 x = 0; x < layer_size[0]; x++) {
                const char * str = (((float)tl.components[y][x]->tint.color.iargb.alpha) > 0
                                        ? "#"
                                        : "!");
                print ("{}{}{}",
                       UX::ANSI (UX::hsva (212, 0, (float)tl.components[y][x]->tint.color.iargb.alpha / 255.0, 1)),
                       str,
                       UX::ANSIRst ());
            }
            print ("    ");
            for (i64 x = 0; x < layer_size[0]; x++) {
                print ("{}#{}",
                       UX::ANSI (tl.components[y][x]->tint.color),
                       UX::ANSIRst ());
            }
            print ("    ");
            for (i64 x = 0; x < layer_size[0]; x++) {
                print ("{}#{}",
                       UX::ANSI (pl.components[y][x]->tint.color),
                       UX::ANSIRst ());
            }
            println ("");
        }
    }
}
