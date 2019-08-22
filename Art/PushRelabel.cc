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
    arc->flow += delta;
    arc->inverse->flow -= delta;
    u->excess -= delta;
    v->excess += delta;
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
    return u->label = minimum_neighbouring_label + 1;
}

Node * PushRelabelNetwork::poll_excess ()
{
    for (usize i = 0; i < N; i++) {
        Node * v = this->nodes[i];
        // Put the source & target checks first. The excess check might require
        // forcing a page fault, 'cause it's more indirected
        if (v != this->source && v != this->target && v->excess > 0) {
            return v;
        }
    }
    return nullptr;
}

Node * PushRelabelNetwork::poll_active ()
{
    for (usize i = 0; i < N; i++) {
        Node * v = this->nodes[i];
        if (v != this->source && v != this->target && v->excess > 0 && v->label <= N) {
            return v;
        }
    }
    return nullptr;
}

void PushRelabelNetwork::discharge (Node * u)
{
    while (u->excess > 0 && u->label <= N) {
        if (u->current_arc < 9_uz) {
            Node * v;
            if (u->current_arc == 0_uz) {
                v = this->target;
            } else {
                isize offset = this->offsets[(u->current_arc - 1 + u->arc_roll) % 8];
                v            = this->nodes[max (offset + u->id, 0) % N];
            }
            Arc * arc = this->edge (u, v);
            if (arc->residual_capacity () && u->label - 1 == v->label) {
                this->push (u, v);
            } else {
                u->current_arc++;
            }
        } else {
            this->relabel (u);
            u->current_arc = 0;
            u->arc_roll    = static_cast<usize> (arc4random_uniform ((uint32_t)9));
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

usize PushRelabelNetwork::compute ()
{
    this->ready ();

    Node * u;
    while ((u = this->poll_active ()) != nullptr) {
        this->discharge (u);
    }

    return static_cast<usize> (this->target->excess);
}

Arc * PushRelabelNetwork::edge (Node * u, Node * v)
{
    return this->arcs[u->id][v->id];
}
