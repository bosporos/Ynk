//
// file art/pushrelabel/pushrelabel.hh
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
        //! Number of nodes in the push-relabel network
        Ynk::usize N;

        //! The flow values in the push-relabel network, stored in an adjacency matrix
        //! sort of thing, but size-optimized for the use case.
        //!
        //! Sized _flows[N][10], but _flows[expr S][N] and _flows[expr T][N] after
        //! [[stabilize]] has been called.
        Ynk::i64 ** _flows;

        //! The capacity values in the push-relabel network, stored in an adjacency matrix
        //! sort of thing, but size-optimized for the use case.
        //!
        //! Sized _capacities[N][10], but _capacities[expr S][N] and _capacities[expr T][N]
        //! after [[stabilize]] has been called.
        Ynk::i64 ** _capacities;

        //! The labels of all the nodes.
        //!
        //! Sized labels[N]
        Ynk::usize * labels;

        //! The excess flow at all the nodes in the network.
        //!
        //! Sized excesses[N]
        Ynk::i64 * excesses;

        //! The current-arc values for all the nodes in the network.
        //!
        //! Sized currents[N]
        Ynk::usize * currents;

        //! The roll values for all the nodes in the network.
        //!
        //! Sized rolls[N]
        Ynk::usize * rolls;

        //! Precomputed set of offsets; essentially, the offsets, in terms of node indices,
        //! of all the nodes that could potentially neighbour a given node.
        //!
        //! Sized offsets[8]
        Ynk::isize * offsets;

        //! Reverse LUT of offsets.
        //!
        //! Sized reverse_offsets[3 + 2 * sqrt(N - 2)]
        Ynk::usize * reverse_offsets;

        //! Used in reverse-offset calculations
        const Ynk::usize square_side;
        const Ynk::usize reverse_offset_width;

        // Used for implementing the highest-label heuristic

        //! Set of doubly-linked lists (for O(1) insert/remove).
        //!
        //! active_sets[b] is the set of all active nodes with label `b`.
        //! Used in compute().
        //!
        //! To add a node to active_sets, please use try_activate() instead of
        //! adding it directly, as it performs checking operations.
        //! Also, do not remove nodes from any of active_sets, except for the bit
        //! that's already implemented in compute().
        std::list<Ynk::usize> * active_sets;

        //! Tracks the highest label of any active node in the network.
        Ynk::isize highest_label;

        //! The source and sink nodes.
        Ynk::usize S, T;

        //! The flow networ is partitioned such that the nodes with indices 0..EOLN
        //! (exclusive upper bound) will all be component nodes, and, conversely,
        //! nodes EOLN and up will *not* be component nodes. Used for determining
        //! which branch to take in discharge().
        Ynk::usize EOLN;

        //! Construct with number of nodes.
        //!
        //! @param [[usize]] N the number of nodes in the network
        PushRelabelNetwork (Ynk::usize N);

        //! Sets up source-to-component and component-to-sink nodes, etc.
        //!
        //! Should be called immediately after S and T have been set.
        void stabilize ();

        //! Ready the network for an iteration.
        //!
        //! Sets up labels, excesses, rolls, currents, zeroes flows, and preps the starting
        //! active nodes, as well as setting up active_sets
        void ready ();

        //! Run the computation.
        void compute ();

        //! Checks if a node is active.
        //! @return bool true if the node is active, false if it is not.
        bool is_active (Ynk::usize u);

        //! Attempts to add the node `u` to active_sets.
        void try_activate (Ynk::usize u);

        //! Do a push operation from u to v.
        void push (Ynk::usize u, Ynk::usize v);

        //! Do a relabel operation on u.
        void relabel (Ynk::usize u);

        //! Discharge u.
        void discharge (Ynk::usize u);

        // These methods provide an interface for working with the special indexing
        // scheme used in _flows and _capacities.

        //! Gets the amount of flow from u to v
        Ynk::i64 flow (Ynk::usize u, Ynk::usize v);

        //! Sets the amount of flow from u to v to new_flow.
        void flow (Ynk::usize u, Ynk::usize v, Ynk::i64 new_flow);

        //! Gets the flow capacity from u to v.
        Ynk::i64 cap (Ynk::usize u, Ynk::usize v);

        //! Sets the flow capacity from u to v to new_capacity.
        void cap (Ynk::usize u, Ynk::usize v, Ynk::i64 new_capacity);
    };
}

#endif /* !@__ART_PUSH_RELABEL */
