//
// file art/pushrelabel.hh
// author Maximilien M. Cura
//

#ifndef __ART_PUSH_RELABEL
#define __ART_PUSH_RELABEL

#include <Ynk/Num/Integers.hh>

#include <Ynk/Result/Result.hh>
#include <Ynk/Option/Option.hh>

#include <Ynk/Fmt/Fmt.hh>

#include <Ynk/Panic/Panic.hh>

#include <list>

namespace Art {
    struct PushRelabelNetwork
    {
        Ynk::usize N;
        Ynk::i64 ** flows;
        Ynk::i64 ** capacities;
        Ynk::usize * labels;
        Ynk::i64 * excesses;
        Ynk::usize * currents;
        Ynk::usize * rolls;
        Ynk::isize * offsets;
        Ynk::isize * reverse_offsets;
        // Note to self: square_side == layer_size.x
        const Ynk::isize square_side;
        const Ynk::isize reverse_offset_width;
        Ynk::i64 ___zero;

        // HL

        std::list<Ynk::usize> * labeled_sets;
        Ynk::isize highest_label;

        // END HL

        Ynk::usize S, T, EOLN;

        PushRelabelNetwork (Ynk::usize);

        void stabilize ();

        void ready ();
        void compute ();

        Ynk::i64 & flow (Ynk::usize, Ynk::usize);
        Ynk::i64 & capacity (Ynk::usize, Ynk::usize);

        Ynk::isize poll_active ();
        bool is_active (Ynk::usize);
        void try_activate (Ynk::usize);
        void push (Ynk::usize, Ynk::usize);
        void relabel (Ynk::usize);
        void discharge (Ynk::usize);
    };
}

#endif /* !@__ART_PUSH_RELABEL */
