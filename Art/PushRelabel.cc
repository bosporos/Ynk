//
// file art/pushrelabel.cc
// author Maximilien M. Cura
//

#include <Art/PushRelabel.hh>

#include <cmath>
#include <algorithm>
#include <stdlib.h> /* arc4random_uniform */

using namespace Art;
using namespace Ynk;

using PRN = Art::PushRelabelNetwork;

PRN::PushRelabelNetwork (usize n)
    : N { n }
{
    flows      = new i64 *[n];
    capacities = new i64 *[n];
    for (usize i = 0; i < n; i++) {
        flows[i]      = new i64[n];
        capacities[i] = new i64[n];
    }

    labels   = new usize[n];
    excesses = new i64[n];
    currents = new usize[n];
    rolls    = new usize[n];
    offsets  = new isize[8];

    const usize square_side = std::sqrt (n.inner_ - 2);
    u8 p                    = 0;
    for (isize ix = -1; ix <= 1; ix++) {
        for (isize iy = -1; iy <= 1; iy++) {
            if (!(ix == 0 && iy == 0)) {
                offsets[p++] = ix + (iy * square_side);
            }
        }
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
        for (usize j = 0; j < N; j++) {
            flows[i][j] = 0;
        }
    }

    labels[S] = N;
    for (usize i = 0; i < N; i++) {
        if (capacities[S][i] > 0)
            excesses[S] += capacities[S][i];
        push (S, i);
    }
}

void PRN::compute ()
{
    isize u;
    while ((u = poll_active ()) != -1) {
        usize w = 0_uz + u;
        discharge (w);
    }
}

isize PRN::poll_active ()
{
    for (usize i = 0; i < N; i++) {
        if (excesses[i] > 0 && i != T && i != S && labels[i] <= N)
            return 0_iz + i;
    }
    return -1;
}

void PRN::push (usize u, usize v)
{
    i64 delta = std::min (excesses[u], capacities[u][v] - flows[u][v]);
    // println ("PUSH {} ({}:{}+{})->({}:{}+{})@{}/{}", delta, u, labels[u], excesses[u], v, labels[v], excesses[v], flows[u][v], capacities[u][v]);
    flows[u][v] += delta;
    flows[v][u] -= delta;
    excesses[u] -= delta;
    excesses[v] += delta;
}

void PRN::relabel (usize u)
{
    // String tmp      = Fmt::format ("RELABEL {}:{}+{}", u, labels[u], excesses[u]);
    usize label_min = labels[u];
    for (usize v = 0; v < N; v++) {
        if (capacities[u][v] > flows[u][v])
            label_min = std::min (label_min, labels[v]);
    }
    // usize old_label = labels[u];
    labels[u] = label_min + 1;
    // String out      = Fmt::format ("{} -> {}", tmp, labels[u]);
    // if (labels[u] != old_label)
    // println (out);
}

void PRN::discharge (usize u)
{
    // println ("======== \x1b[34mDISCHARGE\x1b[0m {}:{}+{}", u, labels[u], excesses[u]);
    while (excesses[u] > 0 && labels[u] <= N) {
        usize v;
        if (u <= EOLN)
            if (currents[u] < 9) {
                if (currents[u] == 0)
                    v = T;
                else
                    v = std::max (u + offsets[(currents[u] - 1 + rolls[u]) % 8], 0_uz) % N;
                if (capacities[u][v] > flows[u][v] && labels[u] > labels[v]) {
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
            if (capacities[u][v] > flows[u][v] && labels[u] > labels[v])
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
