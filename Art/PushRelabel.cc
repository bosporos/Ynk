//
// file art/pushrelabel.cc
// author Maximilien M. Cura
//

#include <Art/Art.hh>
#include <Art/PushRelabel.hh>

#include <cmath>
#include <algorithm>
#include <stdlib.h> /* arc4random_uniform */

using namespace Art;
using namespace Ynk;

using PRN = Art::PushRelabelNetwork;

PRN::PushRelabelNetwork (usize n)
    : N { n }
    , square_side { (_usize)std::sqrt (n.inner_ - 2) }
    , reverse_offset_width { 3 + 2 * (_usize)std::sqrt (n.inner_ - 2) }
{
    Art::Warn (Fmt::format ("Push-relabel network with {} nodes", n));
    _flows      = new i64 *[n];
    _capacities = new i64 *[n];
    for (usize i = 0; i < n; i++) {
        _flows[i]      = new i64[10];
        _capacities[i] = new i64[10];
    }

    labels   = new usize[n];
    excesses = new i64[n];
    currents = new usize[n];
    rolls    = new usize[n];
    offsets  = new isize[8];

    // Precalculate offsets:
    // Each component (marked X) in the component grid has the following valid
    // neighbours (marked O), in addition to the source and sink nodes:
    //
    //   O O O   (-1,-1) (0,-1) (+1,-1)
    //   O X O   (-1, 0)        (+1, 0)
    //   O O O   (-1,+1) (0,+1) (+1,+1)
    //
    // We skip offsetting the centre node, so we use a separated counter
    u8 p = 0;
    for (isize ix = -1; ix <= 1; ix++) {
        for (isize iy = -1; iy <= 1; iy++) {
            if (!(ix == 0 && iy == 0)) {
                offsets[p++] = ix + (iy * square_side);
            }
        }
    }

    // Precalculate reverse_offsets
    // Basically, because the offset might be negative, and negatives can't be used
    // as array indices, we have to make sure all the offsets are converted to a
    // positive value; the easiest way to do it is by adding using a sizing of
    // reverse_offsets[3 + 2 * pixel_grid_dimension] (Note: possible off-by-one
    // overhead on 3)-- so, we just add square_side to + 1 to the offset value,
    // and that seems to work
    reverse_offsets = new usize[reverse_offset_width];

    // Debugging purposes
    for (usize i = 0; i < reverse_offset_width; i++)
        reverse_offsets[i] = 8;
    // These are the ones that matter:
    reverse_offsets[0]                             = 0;   // Offset: -w-1 = (-1,-1)
    reverse_offsets[1]                             = 1;   // Offset: -w = (0,-1)
    reverse_offsets[2]                             = 2;   // Offset: -w+1 = (1,-1)
    reverse_offsets[square_side]                   = 3;   // Offset: -1 = (-1,0)
    reverse_offsets[square_side + 2]               = 4;   // Offset: +1 = (+1,0)
    reverse_offsets[square_side + square_side]     = 5;   // Offset: +w-1 = (-1,+1)
    reverse_offsets[square_side + square_side + 1] = 6;   // Offset: +w = (0,+1)
    reverse_offsets[square_side + square_side + 2] = 7;   // Offset: +w+1 (+1,+1)

    // Initialize the active_sets
    active_sets = new std::list<Ynk::usize>[n];
}

void PRN::stabilize ()
{
    // Basically, the source and sink nodes are the exception to the 10-neighbouring,
    // node rule, but since they're not set at construction time, we do it here,
    // and tell the client to call this.
    delete[] _flows[S];
    delete[] _flows[T];
    delete[] _capacities[S];
    delete[] _capacities[T];

    _flows[S]      = new i64[N];
    _flows[T]      = new i64[N];
    _capacities[S] = new i64[N];
    _capacities[T] = new i64[N];
}

// Note to self: this is called every iteration of the model, so speed should be maximized
void PRN::ready ()
{
    // Zero the current arcs, the current arc rolls, the labels, and the excesses
    for (usize i = 0; i < N; i++) {
        // Start from the sink (if component), or from the 0th node (if not)
        currents[i] = 0;
        // Set up the current arc roll if it's a component
        if (i < EOLN)
            rolls[i] = arc4random_uniform (9);
        // Zero the labels and excesses
        labels[i]   = 0;
        excesses[i] = 0;
    }

    // Zero all the flows
    for (usize i = 0; i < N; i++) {
        if (i == S) {
            // Source node needs *all* flows zeroed
            for (usize j = 0; j < N; j++) {
                flow (i, j, 0);
            }
        } else if (i == T) {
            // Same for sink node, but flows *to*, not flows *from*
            for (usize j = 0; j < N; j++) {
                flow (j, i, 0);
            }
        } else {
            // Just go through the offsets, and zero the flows
            for (usize j = 0; j < 8; j++) {
                usize k = i + offsets[j];
                if (k < N)
                    flow (i, k, 0);
            }
        }
    }

    this->highest_label = 0;
    for (usize i = 0; i < N; i++) {
        active_sets[i].clear ();
    }

    // Source node is N-labeled
    labels[S] = N;
    // Go through and set up the initial active node set
    for (usize i = 0; i < N; i++) {
        // All nodes (s,u) where c(s,u)>0 will get f(s,u)=c(s,u)
        if (cap (S, i) > 0)
            excesses[S] += cap (S, i);
        // Using `push` takes care of "activating" the active nodes i.e. putting
        // them into `active_sets`
        push (S, i);
    }

    // Cherkassky and Goldberg said to do this?
    // "During the initialization s is placed in B_0..."
    this->active_sets[0].push_back (S);
}

void PRN::compute ()
{
    // Go through until there are active nodes left

    // Precise expression:
    //   while \exists u \in B_b, discharge(u)

    // discharge() will *never* reduce the value of highest_label, but it *may*
    // increase it to some value that is less than N; so, we implement the following
    // algorithm:
    //
    // initialization (in [[ready]]):
    //    all active nodes are in active_sets, and v.v.
    //    highest_label is set to 0
    // loop:
    //    while the highest label is greater than or equal to 0
    //    do:
    //      - reduce the highest label if there are no active nodes in the highest-label
    //          category (because discharge may create a "peak" that needs to be
    //          descended from)
    //      - pick an active node with label highest_label, and discharge it, otherwise
    while (highest_label >= 0_iz) {
        if (active_sets[highest_label].size ()) {
            // If there are any nodes with the highest label, discharge them

            // pop_front() only removes; it doesn't grab the value;
            // since there's no compound operation, we use a temporary, because
            // we want to preserve ordering
            usize u = active_sets[highest_label].front ();
            active_sets[highest_label].pop_front ();
            discharge (u);
        } else {
            // If there are no nodes with the highest label, then reduce the highest label
            highest_label--;
        }
    }
}

bool PRN::is_active (usize i)
{
    // Node is active iff:
    //  net inflow > net outflow (i.e. there's excess that can be pushed)
    //  it's not the source or sink
    //  its label is less than N (this is used to exclude nodes whose excesses cannot be pushed to others, i.e. nodes that are in some measure "dead ends")
    return excesses[i] > 0 && i != T && i != S && labels[i] < N;
}

void PRN::try_activate (usize i)
{
    // If it's active
    if (is_active (i)) {
        // Add it to the appropriate active_set
        active_sets[labels[i]].push_back (i);
        // Modify highest_label as necessary
        if ((labels[i] - highest_label) > 0_uz)
            highest_label = -(-labels[i]);
    }
}

void PRN::push (usize u, usize v)
{
    // Calculate the pushed amount of flow
    // Because of the way the algorithm is structured, pushes must be saturating (I think?)
    i64 delta = std::min (excesses[u], cap (u, v) - flow (u, v));

    // Debugging
    //println ("PUSH {} ({}:{}+{})->({}:{}+{})@{}/{}", delta, u, labels[u], excesses[u], v, labels[v], excesses[v], flows[u][v], capacities[u][v]);

    // Apply the delta to the network
    flow (u, v, flow (u, v) + delta);
    flow (v, u, flow (v, u) - delta);
    excesses[u] -= delta;
    excesses[v] += delta;

    // Try to activate the node that was just pushed to, because it may have acquired
    // excess that could be pushed to other nodes
    try_activate (v);
}

void PRN::relabel (usize u)
{
    // Note to self: if we ever use the multi-node brush concept, then go back and
    // fix this to do EOLN checking

    // Just go through the 10 neighbours.
    usize label_min = labels[u];
    if (cap (u, S) > flow (u, S))
        label_min = std::min (label_min, labels[S]);
    if (cap (u, T) > flow (u, T))
        label_min = std::min (label_min, labels[T]);
    for (usize v = 0; v < 8; v++) {
        // exploit unsignedness
        usize w = u + offsets[v];
        if (w < N)
            if (cap (u, w) > flow (u, w))
                label_min = std::min (label_min, labels[w]);
    }
    // Debugging
    //String beginning      = Fmt::format ("RELABEL {}:{}+{}", u, labels[u], excesses[u]);
    labels[u] = label_min + 1;
    // Debugging
    //String out      = Fmt::format ("{} -> {}", beginning, labels[u]);
    //println (out);

    // Since relabel() is only ever called on nodes that are being discharge()'d,
    // and discharge()'d nodes are always removed from the active_sets before being
    // discharged (to prevent duplicate nodes in different label categories), we
    // try to reactivate the node here, if possible, with the *new* label.
    try_activate (u);
}

void PRN::discharge (usize u)
{
    // Debugging
    //println ("======== \x1b[34mDISCHARGE\x1b[0m {}:{}+{}", u, labels[u], excesses[u]);

    // Basically, while u is still active.
    //
    // Detailed explanation:
    // Since discharge() is only applied to active nodes, and "active" precludes
    // the conditions u=S and u=T, and since the source and sink node do not change
    // over the course of this process, we can exclude those conditions in this loop
    // and check instead the mutable elements, the label and the excess.
    //
    // Since only the relabel() operation can change the label, and the relabel
    // operation is only applied before the loop breaks, we don't need to constantly
    // check the label; we only need to check the excess
    while (excesses[u] > 0) {
        if (currents[u] < 9) {
            usize v;
            // Make sure that the sink is always checked first (because currents[u]
            // is always reset to 0), so that water isn't just gliding across the
            // page
            if (currents[u] == 0)
                v = T;
            else
                // Use the offset LUT, clamped to [0,N) (that's basically all it does- it looks more complicated than it is)
                v = std::max (u + offsets[(currents[u] - 1 + rolls[u]) % 8], 0_uz) % N;

            if (cap (u, v) > flow (u, v) && labels[u] == labels[v] + 1) {
                // If there is residual capacity on the edge, and the labeling is admissible,
                // perform a push operation
                push (u, v);
            } else
                // Otherwise, just increment the current edge
                currents[u]++;
        } else {
            // Relabel the discharged node, which'll also take care of re-activating
            // it if needs be
            relabel (u);
            // Reset the current edge
            currents[u] = 0;
            // arc4random_uniform(N) returns in [0,N)
            rolls[u] = arc4random_uniform (9);

            return;
        }
    }
}

// ==============================================================
// INTERFACES FOR THE _flows AND _capacities ADJACENCY MATRICES
// ==============================================================

i64 PRN::flow (usize u, usize v)
{
    if (u == S || u == T) {
        // For u=S and u=T, _flows[u] has N entries, so we can do this simply:
        return _flows[u][v];
    }
    if (v == S)
        // Target vertex is source, which is automatically _flows[u][0]
        return _flows[u][0];
    if (v == T)
        // Target vertex is the sink, which is automatically _flows[u][1]
        return _flows[u][1];
    if (u == v)
        // Don't permit loops
        return 0_i64;
    // Run a look-up in the reverse_offsets table, and grab that
    // This actually uses a bit of trickery, in conjunction with the range check
    usize offset = square_side + 1 + (isize)v - (isize)u;
    // Range check: because signed integers are almost always represented as two's
    // complement, signed integers, when converted into unsigned integers, will
    // be greater (unsigned), then the absolute value of their signed form.
    // Thus, since the value that goes into offset is a signed integer, that's cast to
    // an unsigned one, and because the range check permits [0,reverse_offset width),
    // we can say that if the value was negative, the unsigned cast of it will be
    // greater than reverse_offset_width, and only need the following check:
    if (offset < reverse_offset_width) {
        // Validity check
        if (reverse_offsets[offset] != 8)
            // Grab and return
            return _flows[u][2 + reverse_offsets[offset]];
        else
            // Invalid -> 0
            return 0_i64;
    }
    // -> 0
    return 0_i64;
}

// From this point onwards, everything is basically the same as in flow(u,v)

i64 PRN::cap (usize u, usize v)
{
    if (u == S || u == T)
        return _capacities[u][v];
    if (v == S)
        return _capacities[u][0];
    if (v == T)
        return _capacities[u][1];
    if (u == v)
        return 0_i64;
    usize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset < reverse_offset_width) {
        if (reverse_offsets[offset] != 8)
            return _capacities[u][2 + reverse_offsets[offset]];
        else
            return 0_i64;
    }
    return 0_i64;
}

void PRN::flow (usize u, usize v, i64 f)
{
    if (u == S || u == T)
        _flows[u][v] = f;
    if (v == S)
        _flows[u][0] = f;
    if (v == T)
        _flows[u][1] = f;
    usize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset < reverse_offset_width) {
        if (reverse_offsets[offset] != 8)
            _flows[u][2 + reverse_offsets[offset]] = f;
    }
}

void PRN::cap (usize u, usize v, i64 c)
{
    if (u == S || u == T)
        _capacities[u][v] = c;
    if (v == S)
        _capacities[u][0] = c;
    if (v == T)
        _capacities[u][1] = c;
    usize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset < reverse_offset_width) {
        if (reverse_offsets[offset] != 8)
            _capacities[u][2 + reverse_offsets[offset]] = c;
    }
}
