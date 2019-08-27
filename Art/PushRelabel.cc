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
    , square_side { (_isize)std::sqrt (n.inner_ - 2) }
    , reverse_offset_width { 3 + (_isize)std::sqrt (n.inner_ - 2) * 2 }
    , ___zero { 0 }
{
    Art::Warn (Fmt::format ("Push-relabel network with {} nodes", n));
    flows      = new i64 *[n];
    capacities = new i64 *[n];
    for (usize i = 0; i < n; i++) {
        flows[i]      = new i64[10];
        capacities[i] = new i64[10];
    }

    labels   = new usize[n];
    excesses = new i64[n];
    currents = new usize[n];
    rolls    = new usize[n];
    offsets  = new isize[8];

    reverse_offsets = new isize[reverse_offset_width];
    u8 p            = 0;
    for (isize ix = -1; ix <= 1; ix++) {
        for (isize iy = -1; iy <= 1; iy++) {
            if (!(ix == 0 && iy == 0)) {
                isize offset = ix + (iy * square_side);
                // reverse_offsets[square_side + 1 + offset] = p;
                offsets[p++] = offset;
            }
        }
    }
    Art::Warn (Fmt::format ("square_side of {}", square_side));
    std::printf ("\t___zero is %p\n", &___zero);
    reverse_offsets[0]                             = 0;
    reverse_offsets[1]                             = 1;
    reverse_offsets[2]                             = 2;
    reverse_offsets[square_side - 1]               = 3;
    reverse_offsets[square_side + 1]               = 4;
    reverse_offsets[square_side + square_side - 1] = 5;
    reverse_offsets[square_side + square_side]     = 6;
    reverse_offsets[square_side + square_side + 1] = 7;

    labeled_sets = new std::list<Ynk::usize>[n];
}

void PRN::stabilize ()
{
    delete[] flows[S];
    delete[] flows[T];

    flows[S] = new i64[N];
    flows[T] = new i64[N];

    delete[] capacities[S];
    delete[] capacities[T];

    capacities[S] = new i64[N];
    capacities[T] = new i64[N];
}

i64 & PRN::flow (usize u, usize v)
{
    if (u == S || u == T)
        return flows[u][v];
    // S & T may be in the offset set, but we want to take care of them here in case they aren't
    if (v == S)
        return flows[u][0];
    if (v == T)
        return flows[u][1];
    if (u == v)
        return ___zero;
    isize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset >= 0 && offset < reverse_offset_width) {
        return flows[u][reverse_offsets[offset] + 2];
    } else {
        return ___zero;
    }
    // isize offset = (isize)v - (isize)u;
    // for (u8 i = 0; i < 8; i++) {
    //     if (offsets[i] == offset)
    //         return flows[u][2 + i];
    // }
    // return ___zero;
}

i64 & PRN::capacity (usize u, usize v)
{
    if (u == S || u == T)
        return capacities[u][v];
    // S & T may be in the offset set, but we want to take care of them here in case they aren't
    if (v == S)
        return capacities[u][0];
    if (v == T)
        return capacities[u][1];
    if (u == v)
        return ___zero;
    isize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset >= 0 && offset < reverse_offset_width) {
        return capacities[u][reverse_offsets[offset] + 2];
    } else {
        return ___zero;
    }
    // isize offset = (isize)v - (isize)u;
    // for (u8 i = 0; i < 8; i++) {
    //     if (offsets[i] == offset)
    //         return capacities[u][2 + i];
    // }
    // return ___zero;
}

void PRN::ready ()
{
    for (usize i = 0; i < N; i++) {
        currents[i] = T;
        if (i < EOLN)
            rolls[i] = arc4random_uniform (9);
        labels[i]   = 0;
        excesses[i] = 0;
    }

    for (usize i = 0; i < N; i++) {
        for (usize j = 0; j < N; j++) {
            flow (i, j) = 0;
        }
    }

    labels[S] = N;
    for (usize i = 0; i < N; i++) {
        if (capacity (S, i) > 0)
            excesses[S] += capacity (S, i);
        push (S, i);
    }

    // HL
    for (usize i = 0; i < N; i++) {
        labeled_sets[i].clear ();
    }

    this->highest_label = N;
    for (usize i = 0; i < N; i++) {
        if (is_active (i)) {
            try_activate (i);
        }
    }
    // Apparently supposed to do this?
    this->labeled_sets[0].push_back (S);

    // END HL
}

void PRN::compute ()
{
    // isize u;
    // while ((u = poll_active ()) != -1) {
    //     usize w = 0_uz + u;
    //     discharge (w);
    // }

    // HL

    while (highest_label >= 0_iz) {
        usize u;
        if (labeled_sets[highest_label].size ()) {
            u = labeled_sets[highest_label].front ();
            labeled_sets[highest_label].pop_front ();
            discharge (u);
        } else {
            highest_label--;
        }
    }

    // END HL
}

isize PRN::poll_active ()
{
    for (usize i = 0; i < N; i++) {
        if (is_active (i))
            return 0_iz + i;
    }
    return -1;
}

bool PRN::is_active (usize i)
{
    return excesses[i] > 0 && i != T && i != S && labels[i] < N;
}

// HL

void PRN::try_activate (usize i)
{
    if (is_active (i)) {
        labeled_sets[labels[i]].push_back (i);
        if (labels[i] > highest_label)
            highest_label = -(-labels[i]);
    }
}

// END HL

void PRN::push (usize u, usize v)
{
    i64 delta = std::min (excesses[u], capacity (u, v) - flow (u, v));
    // println ("PUSH {} ({}:{}+{})->({}:{}+{})@{}/{}", delta, u, labels[u], excesses[u], v, labels[v], excesses[v], flows[u][v], capacities[u][v]);
    flow (u, v) += delta;
    flow (v, u) -= delta;
    excesses[u] -= delta;
    excesses[v] += delta;

    try_activate (v);
}

void PRN::relabel (usize u)
{
    // String tmp      = Fmt::format ("RELABEL {}:{}+{}", u, labels[u], excesses[u]);
    usize label_min = labels[u];
    for (usize v = 0; v < N; v++) {
        if (capacity (u, v) > flow (u, v))
            label_min = std::min (label_min, labels[v]);
    }
    // usize old_label = labels[u];
    labels[u] = label_min + 1;
    // String out      = Fmt::format ("{} -> {}", tmp, labels[u]);
    // if (labels[u] != old_label)
    // println (out);
    try_activate (u);
}

void PRN::discharge (usize u)
{
    // println ("======== \x1b[34mDISCHARGE\x1b[0m {}:{}+{}", u, labels[u], excesses[u]);
    while (excesses[u] > 0 && labels[u] <= N) {
        usize v;
        if (u <= EOLN)
            if (currents[u] < 9_uz) {
                if (currents[u] == 0_uz)
                    v = T;
                else
                    v = std::max (u + offsets[(currents[u] - 1 + rolls[u]) % 8], 0_uz) % N;
                if (capacity (u, v) > flow (u, v) && labels[u] > labels[v]) {
                    push (u, v);
                } else
                    currents[u]++;
            } else {
                relabel (u);
                currents[u] = 0;
                rolls[u]    = arc4random_uniform (9);
                // return;
            }
        else if (currents[u] < N) {
            v = currents[u];
            if (capacity (u, v) > flow (u, v) && labels[u] > labels[v])
                push (u, v);
            else
                currents[u]++;
        } else {
            relabel (u);
            currents[u] = 0;
            // return;
        }
    }
}
