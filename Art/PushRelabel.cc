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

    u8 p = 0;
    for (isize ix = -1; ix <= 1; ix++) {
        for (isize iy = -1; iy <= 1; iy++) {
            if (!(ix == 0 && iy == 0)) {
                offsets[p++] = ix + (iy * square_side);
            }
        }
    }

    reverse_offsets = new usize[reverse_offset_width];

    for (usize i = 0; i < reverse_offset_width; i++)
        reverse_offsets[i] = 8;
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
    delete[] _flows[S];
    delete[] _flows[T];
    delete[] _capacities[S];
    delete[] _capacities[T];

    _flows[S]      = new i64[N];
    _flows[T]      = new i64[N];
    _capacities[S] = new i64[N];
    _capacities[T] = new i64[N];
}

i64 PRN::flow (usize u, usize v)
{
    if (u == S || u == T) {
        return _flows[u][v];
    }
    if (v == S)
        return _flows[u][0];
    if (v == T)
        return _flows[u][1];
    if (u == v)
        return 0_i64;
    isize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset >= 0 && offset < reverse_offset_width) {
        if (reverse_offsets[offset] != 8)
            return _flows[u][2 + reverse_offsets[offset]];
        else
            return 0_i64;
    }
    return 0_i64;
}

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
    isize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset >= 0 && offset < reverse_offset_width) {
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
    isize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset >= 0 && offset < reverse_offset_width) {
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
    isize offset = square_side + 1 + (isize)v - (isize)u;
    if (offset >= 0 && offset < reverse_offset_width) {
        if (reverse_offsets[offset] != 8)
            _capacities[u][2 + reverse_offsets[offset]] = c;
    }
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
        if (i == S || i == T) {
            for (usize j = 0; j < N; j++) {
                flow (i, j, 0);
            }
        } else {
            flow (i, T, 0);
            flow (i, T, 0);
            for (usize j = 0; j < 8; j++) {
                usize k = i + offsets[j];
                if (k < N)
                    flow (i, k, 0);
            }
        }
    }

    labels[S] = N;
    for (usize i = 0; i < N; i++) {
        if (cap (S, i) > 0)
            excesses[S] += cap (S, i);
        push (S, i);
    }

    for (usize i = 0; i < N; i++) {
        labeled_sets[i].clear ();
    }

    this->highest_label = 0;
    for (usize i = 0; i < N; i++) {
        if (is_active (i)) {
            try_activate (i);
        }
    }
    this->labeled_sets[0].push_back (S);
}

void PRN::compute ()
{
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
}

bool PRN::is_active (usize i)
{
    return excesses[i] > 0 && i != T && i != S && labels[i] < N;
}

void PRN::try_activate (usize i)
{
    if (is_active (i)) {
        labeled_sets[labels[i]].push_back (i);
        if (labels[i] > highest_label)
            highest_label = -(-labels[i]);
    }
}

void PRN::push (usize u, usize v)
{
    i64 delta = std::min (excesses[u], cap (u, v) - flow (u, v));
    // println ("PUSH {} ({}:{}+{})->({}:{}+{})@{}/{}", delta, u, labels[u], excesses[u], v, labels[v], excesses[v], flows[u][v], capacities[u][v]);
    flow (u, v, flow (u, v) + delta);
    flow (v, u, flow (v, u) - delta);
    excesses[u] -= delta;
    excesses[v] += delta;

    try_activate (v);
}

void PRN::relabel (usize u)
{
    // String tmp      = Fmt::format ("RELABEL {}:{}+{}", u, labels[u], excesses[u]);
    usize label_min = labels[u];
    // for (usize v = 0; v < N; v++) {
    //     if (capacities[u][v] > flows[u][v])
    //         label_min = std::min (label_min, labels[v]);
    // }
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
    while (excesses[u] > 0 && labels[u] < N) {
        usize v;
        if (u != S && u != T) {
            if (currents[u] < 9) {
                if (currents[u] == 0)
                    v = T;
                else
                    v = std::max (u + offsets[(currents[u] - 1 + rolls[u]) % 8], 0_uz) % N;
                if (cap (u, v) > flow (u, v) && labels[u] == labels[v] + 1) {
                    push (u, v);
                } else
                    currents[u]++;
            } else {
                relabel (u);
                currents[u] = 0;
                rolls[u]    = arc4random_uniform (9);
                return;
            }
        } else if (currents[u] < N) {
            v = currents[u];
            if (cap (u, v) > flow (u, v) && labels[u] == labels[v] + 1)
                push (u, v);
            else
                currents[u]++;
        } else {
            relabel (u);
            currents[u] = 0;
            return;
        }
    }
}
