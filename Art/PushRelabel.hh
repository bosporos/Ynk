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

        Ynk::usize S, T, EOLN;

        PushRelabelNetwork (Ynk::usize);

        void ready ();
        void compute ();

        Ynk::isize poll_active ();
        void push (Ynk::usize, Ynk::usize);
        void relabel (Ynk::usize);
        void discharge (Ynk::usize);
    };
}

#endif /* !@__ART_PUSH_RELABEL */
