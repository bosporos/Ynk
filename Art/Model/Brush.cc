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
    return 1;
}

void Brush::_pr_attach (PushRelabelNetwork * prn, Vec2i space)
{
    // Bristles are bristles_index...prn.N-1
    for (usize i = 0; i < num_bristles; i++) {
        // @TODO: Add code to base outflow amount on bristle contact
        Vec3d tip_position = bristles[i].tip_offset + position;
        if (tip_position[2] <= 0) {
            i64 x = static_cast<_i64> (std::round (tip_position[0]));
            i64 y = static_cast<_i64> (std::round (tip_position[1]));
            if (x >= 0 && x < space[0] && y >= 0 && y < space[1])
                // Connect bristle to paper
                prn->capacity (prn->S, 0_uz + x + space[1] * y) = Math::min (256_u64, this->bristles[i].tint_remaining);
        }
    }
}
