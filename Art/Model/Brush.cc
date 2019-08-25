//
// file art/model/Brush.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>
#include <Ynk/Math/Math.hh>

#include <cmath>

using namespace Ynk;
using namespace Art;

Bristle::Bristle (Vec3d v3, u64 t_r)
    : tip_offset { v3 }
    , tint_remaining { t_r }
{}

Brush::Brush (Brush && husk)
    : num_bristles { husk.num_bristles }
    , bristles { husk.bristles }
    , position { husk.position }
    , ink { husk.ink }
{
    husk.bristles = nullptr;
}

Brush::Brush (Bristle * bristles, usize n_bristles, UX::RGBA ink)
    : num_bristles { n_bristles }
    , bristles { bristles }
    , position { Art::Space3d::instance (SpaceType::Cartesian)->create_vec ({ 0, 0, 0 }) }
    , ink { ink }
{}

usize Brush::_pr_num_nodes ()
{
    return this->num_bristles + 1;
}

void Brush::_pr_attach (PushRelabelNetwork * prn, usize bristles_index, Vec2i space)
{
    usize source = prn->source->id;
    // Bristles are bristles_index...prn.N-1
    for (usize i = bristles_index; i < (bristles_index + num_bristles); i++) {
        // @TODO: Add code to base outflow amount on bristle contact
        prn->arcs[source][i]->capacity = Math::min (256_u64, this->bristles[i - bristles_index].tint_remaining);
        Vec3d tip_position             = bristles[i - bristles_index].tip_offset + position;
        if (tip_position[2] <= 0) {
            i64 x = static_cast<_i64> (std::round (tip_position[0]));
            i64 y = static_cast<_i64> (std::round (tip_position[1]));
            if (x >= 0 && x < space[0] && y >= 0 && y < space[1])
                // Connect bristle to paper
                prn->arcs[i][x + space[1] * y]->capacity = 256;
        } else {
            // Dead ends take up valuable time, so prevent that
            prn->arcs[source][i]->capacity = 0;
        }
    }
}
