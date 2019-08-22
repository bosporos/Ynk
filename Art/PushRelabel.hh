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

// Inspired by JGraphT

namespace Art {
    struct Node
    {
        Ynk::usize id;
        Ynk::usize label;
        Ynk::usize arc_roll;
        Ynk::usize current_arc;
        Ynk::i64 excess;

        Node (Ynk::usize id);
        Node (Node const &);
    };

    struct Arc
    {
        Node * source;
        Node * target;
        Arc * inverse;
        Ynk::i64 capacity;
        Ynk::i64 flow;

        Arc (Node * src, Node * target);
        Arc (Arc const &);
        Ynk::i64 residual_capacity ();
    };

    struct PushRelabelNetwork
    {
        const Ynk::usize N;
        Node * source;
        Node * target;
        Node ** nodes;
        Arc *** arcs;
        Ynk::isize offsets[8];

        PushRelabelNetwork (Ynk::usize n);

        Node * poll_excess ();
        Node * poll_active ();

        Ynk::i64 push (Node *, Node *);
        Ynk::usize relabel (Node *);
        void discharge (Node *);

        void ready ();
        Ynk::usize compute ();
        void decompose ();

        Arc * edge (Node *, Node *);
    };
}

namespace Ynk::Fmt {
    template <>
    struct Formatter<::Art::Arc>
    {
        static void format (::Art::Arc arc, FormatContext ctx)
        {
            ctx.write_str (
                Fmt::format ("({},{}),c:{},f:{}",
                             *arc.source,
                             *arc.target,
                             arc.capacity,
                             arc.flow));
        }
    };

    template <>
    struct Formatter<::Art::Node>
    {
        static void format (::Art::Node node, FormatContext ctx)
        {
            ctx.write_str (Fmt::format ("({}:{})", node.id, node.label));
            if (node.excess != 0) {
                if (node.excess > 0) {
                    ctx.write_char ('+');
                }
                ctx.write_int (node.excess, 10, false, false);
            }
        }
    };
}

#endif /* !@__ART_PUSH_RELABEL */
