//
// file art/pushrelabel.cc
// author Maximilien M. Cura
//

#include <Art/PushRelabel.hh>

#include <tgmath.h>
#include <stdlib.h> /* arc4random_uniform */

using namespace Art;
using namespace Ynk;

i64 min (i64 a, i64 b) { return a < b ? a : b; }
isize max (isize a, isize b) { return a > b ? a : b; }

Node::Node (usize id)
    : id { id }
    , label { 0 }
    , arc_roll { 0 }
    , current_arc { 0 }
    , excess { 0 }
{}

Node::Node (Node const & orig)
    : id { orig.id }
    , label { orig.label }
    , arc_roll { orig.arc_roll }
    , current_arc { orig.current_arc }
    , excess { orig.excess }
{}

Arc::Arc (Node * src, Node * target)
    : source { src }
    , target { target }
    , inverse { nullptr }
    , capacity { 0 }
    , flow { 0 }
{}

Arc::Arc (Arc const & orig)
    : source { orig.source }
    , target { orig.target }
    , inverse { orig.inverse }
    , capacity { orig.capacity }
    , flow { orig.flow }
{}

i64 Arc::residual_capacity ()
{
    return this->capacity - this->flow;
}

i64 PushRelabelNetwork::push (Node * u, Node * v)
{
    Arc * arc = this->edge (u, v);
    i64 delta = min (u->excess, arc->residual_capacity ());
    print_err ("Pushing {} along {}", delta, *arc);
    arc->flow += delta;
    arc->inverse->flow -= delta;
    u->excess -= delta;
    v->excess += delta;
    println_err ("-> {}", *arc);
    return delta;
}

usize PushRelabelNetwork::relabel (Node * u)
{
    usize minimum_neighbouring_label = u->label;
    for (usize i = 0; i < N; i++) {
        Arc * arc = this->edge (u, this->nodes[i]);
        if (arc != nullptr && arc->residual_capacity ()) {
            if (arc->target->label < minimum_neighbouring_label)
                minimum_neighbouring_label = arc->target->label;
        }
    }
    println_err ("Relabeling {} -> {}", *u, minimum_neighbouring_label + 1);
    u->label = minimum_neighbouring_label + 1;
    return u->label;
}

Node * PushRelabelNetwork::poll_excess ()
{
    for (usize i = 0; i < N; i++) {
        Node * v = this->nodes[i];
        // Put the source & sink checks first. The excess check might require
        // forcing a page fault, 'cause it's more indirected
        if (v != this->source && v != this->sink && v->excess > 0) {
            return v;
        }
    }
    return nullptr;
}

Node * PushRelabelNetwork::poll_active ()
{
    usize r { arc4random_uniform (N) };
    // usize r { 0 };
    usize q = 0;
    for (usize i = 0;; i++) {
        Node * v = this->nodes[i % N];
        if (v != this->source && v != this->sink && v->excess > 0 && v->label < N) {
            if (q == r)
                return v;
            q++;
        }
    }
    return nullptr;
}

void PushRelabelNetwork::discharge_layer (Node * u)
{
    while (u->excess > 0 && u->label < N) {
        if (u->current_arc < 9_uz) {
            Node * v;
            if (u->current_arc == 0_uz) {
                v = this->sink;
            } else {
                isize offset = this->offsets[(u->current_arc - 1 + u->arc_roll) % 8];
                v            = this->nodes[max (offset + u->id, 0) % N];
            }
            Arc * arc = this->edge (u, v);
            if (arc->residual_capacity () && u->label > v->label) {
                this->push (u, v);
            } else {
                u->current_arc++;
            }
        } else {
            // if (u->id == 0_uz)
            //     println_err ("[F {}\n {}\n {}\n {}]",
            //                  *arcs[0][1],
            //                  *arcs[0][4],
            //                  *arcs[0][5],
            //                  *edge (u, sink));
            usize old_lbl = u->label;
            this->relabel (u);
            // no change in label when relabeling, so break out
            if (old_lbl == u->label)
                return;
            // if (u->label == 2_uz && ol == 2_uz) {
            //     for (isize i = 0; i < (_isize)N.inner_; i++) {
            //         println_err ("NODE {}", *nodes[i]);
            //     }
            //     for (isize i = -1; i < (_isize)N.inner_; i++) {
            //         if (i == -1) {
            //             std::printf ("   ");
            //             for (isize j = 0; j < (_isize)N.inner_; j++) {
            //                 if (j < 10)
            //                     std::printf (" ");
            //                 print ("  {} ", j);
            //             }
            //             std::printf ("\n");
            //             continue;
            //         }
            //         for (isize j = -1; j < (_isize)N.inner_; j++) {
            //             if (j == -1) {
            //                 if (i < 10)
            //                     std::printf (" ");
            //                 print ("{} ", i);
            //                 continue;
            //             }
            //             if (arcs[i][j]->flow.inner_ != -128 && arcs[i][j]->flow.inner_ != -256)
            //                 std::printf (" ");
            //             if (arcs[i][j]->flow.inner_ != -128
            //                 && arcs[i][j]->flow.inner_ != -256
            //                 && arcs[i][j]->flow.inner_ != 256
            //                 && arcs[i][j]->flow.inner_ != 128
            //                 && arcs[i][j]->flow.inner_ != -96
            //                 && arcs[i][j]->flow.inner_ != -64
            //                 && arcs[i][j]->flow.inner_ != -32)
            //                 std::printf (" ");
            //             if (!arcs[i][j]->flow.inner_)
            //                 std::printf (" ");
            //             std::printf ("%lli ", arcs[i][j]->flow.inner_);
            //         }
            //         std::printf ("\n");
            //     }
            //     panic ("DEAD (RELABEL LOOP)");
            // }
            u->current_arc = 0;
            u->arc_roll    = static_cast<usize> (arc4random_uniform ((uint32_t)9));
            return;
        }
    }
}

void PushRelabelNetwork::discharge_general (Node * u)
{
    while (u->excess > 0 && u->label < N) {
        if (u->current_arc < N) {
            Node * v  = this->nodes[u->current_arc];
            Arc * arc = this->edge (u, v);
            if (arc->residual_capacity () && u->label - 1 == v->label) {
                this->push (u, v);
            } else {
                u->current_arc++;
            }
        } else {
            this->relabel (u);
            u->current_arc = 0;
        }
    }
}

PushRelabelNetwork::PushRelabelNetwork (usize n)
    : N { n }
{
    this->nodes = new Node *[N];
    this->arcs  = new Arc **[N];
    for (usize i = 0; i < N; i++) {
        this->nodes[i] = new Node (i);
    }
    for (usize i = 0; i < N; i++) {
        this->arcs[i] = new Arc *[N];
        for (usize j = 0; j < N; j++) {
            this->arcs[i][j] = new Arc (this->nodes[i], this->nodes[j]);
        }
    }
    for (usize i = 0; i < N; i++) {
        for (usize j = 0; j < N; j++) {
            this->arcs[i][j]->inverse = this->arcs[j][i];
        }
    }
    usize square_side = std::sqrt (n.inner_ - 2);
    u8 p              = 0;
    for (isize ix = -1; ix <= 1; ix++) {
        for (int iy = -1; iy <= 1; iy++) {
            if (!(ix == 0 && iy == 0)) {
                this->offsets[p++] = ix + (iy * square_side);
            }
        }
    }
}

void PushRelabelNetwork::reset_capacities ()
{
    for (usize i = 0; i < N; i++) {
        for (usize j = 0; j < N; j++) {
            this->arcs[i][j]->capacity = 0;
        }
    }
}

void PushRelabelNetwork::ready ()
{
    for (usize x = 0; x < N; x++) {
        for (usize y = 0; y < N; y++) {
            if (this->arcs[x][y] != nullptr) {
                this->arcs[x][y]->flow = 0;
            }
        }
    }
    for (usize i = 0; i < N; i++) {
        this->nodes[i]->excess      = 0;
        this->nodes[i]->label       = 0;
        this->nodes[i]->current_arc = 0;
        this->nodes[i]->arc_roll    = static_cast<usize> (arc4random_uniform ((uint32_t)9));
    }

    this->source->label = N;
    for (usize i = 0; i < N; i++) {
        i64 capacity = this->edge (this->source, this->nodes[i])->capacity;
        if (capacity > 0)
            this->source->excess += capacity;
    }
    for (usize i = 0; i < N; i++) {
        this->push (this->source, this->nodes[i]);
    }
}

void PushRelabelNetwork::discharge (Node * u)
{
    if (u->id < sink->id) {
        // println_err ("Discharging layer node: {}", *u);
        this->discharge_layer (u);
    } else {
        println_err ("Discharging general node: {}", *u);
        this->discharge_general (u);
    }
}

usize PushRelabelNetwork::compute ()
{
    Node * u;
    while ((u = this->poll_active ()) != nullptr) {
        this->discharge (u);
    }

    return static_cast<usize> (this->sink->excess);
}

Arc * PushRelabelNetwork::edge (Node * u, Node * v)
{
    return this->arcs[u->id][v->id];
}
