//
// file art/model/TintLayer.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>
#include <Ynk/Math/Math.hh>

#include <cmath>
#include <algorithm>

using namespace Ynk;
using namespace Art;

#define TLAYER_TQ_EP1 412.0
#define TLAYER_TQ_EP2 TLAYER_TQ_EP1 * 255

// Tint lerp'ing

void Art::Tint::blend (Art::Tint rhs)
{
    // Equivalent to q0+q1
    quantity += rhs.quantity;
    // (q0 + q1)^2
    double q2 = (double)quantity * (double)quantity;
    // final alpha value a_{final}
    u8 end_alpha;
    // The alpha fallaway function.
    if (q2 <= TLAYER_TQ_EP1)
        end_alpha = std::min (255_u8, static_cast<u8> (q2 / TLAYER_TQ_EP1));
    else {   // q2 > TLAYER_TQ_EP1
        double q3 = (q2 - TLAYER_TQ_EP1) / TLAYER_TQ_EP1;
        end_alpha = static_cast<u8> (255.0 * std::exp (-q3 / TLAYER_TQ_EP1));
    }
    color             = color.lerp (rhs.color, static_cast<float> (rhs.quantity) / static_cast<float> (quantity));
    color.iargb.alpha = end_alpha;
}

// Layer Component

Art::TintLayerComponent::TintLayerComponent ()
    : maximal_moment_chromosaturation {}
    , tint { UX::RGBA (0, 0, 0, 0), 0 }
{}

// Layer Proper

Art::TintLayer::TintLayer (Art::Vec2i size, Art::Brush * brush)
    : size { size }
    , brush { brush }
    , components { nullptr }
    , prn (1 + size.space->op_vec_product (size) + brush->_pr_num_nodes ())
    , _pr_sink_index ((size[0] * size[1]).inner_)
{
    // Set up the component layers and the push relabel networks
    this->components = new TintLayerComponent **[size[1]];
    for (i64 i = 0; i < size[1]; i++) {
        this->components[i] = new TintLayerComponent *[size[0]];
        for (i64 j = 0; j < size[0]; j++) {
            this->components[i][j] = new TintLayerComponent ();
        }
    }
    prn.S    = _pr_sink_index + 1;
    prn.T    = _pr_sink_index;
    prn.EOLN = _pr_sink_index;
    prn.stabilize ();
}

Art::TintLayer::~TintLayer ()
{
    // Teardown
    for (i64 i = 0; i < this->size[1]; i++) {
        for (i64 j = 0; j < this->size[0]; j++) {
            delete this->components[i][j];
        }
        delete[] this->components[i];
    }
    delete[] this->components;
}

void Art::TintLayer::_pr_construct (YNK_UNUSED Art::PaperLayer * pl, YNK_UNUSED Art::WaterLayer * wl)
{
    // Can't set up intragrid edges beforehand like you can in WaterLayer, because
    // those are set up on a per-iteration basis because they depend on the water layer
    for (i64 i = 0; i < size[1]; i++) {
        for (i64 j = 0; j < size[0]; j++) {
            components[i][j]->tint                   = pl->components[i][j]->tint;
            components[i][j]->tint.color.iargb.alpha = 0;
        }
    }
}

void Art::TintLayer::_pr_ready (YNK_UNUSED Art::PaperLayer * pl, Art::WaterLayer * wl)
{
    i64 w = this->size[0], h = this->size[1];
    // Set up the flow network
    for (i64 x = 0; x < w; x++) {
        for (i64 y = 0; y < h; y++) {
            Ynk::usize i = _pr_index (x, y);
            // Lateral capacity settings
            for (i64 ix = (x != 0 ? -1 : 0); ix <= (x != w - 1 ? 1 : 0); ix++) {
                for (i64 iy = (y != 0 ? -w : 0_i64); iy <= (y != h - 1 ? w : 0_i64); iy += w) {
                    if (LIKELY (!(ix == 0 && iy == 0))) {
                        Ynk::usize j = _pr_index (x, y) + ix + iy;
                        if (wl->prn.flow (i, j) > 0) {
                            // c_tint(u,v) = 0.75|f_water(u,v)|
                            Ynk::i64 cap = std::abs (static_cast<Ynk::_i64> (
                                static_cast<long double> (wl->prn.flow (i, j)) * 0.75L));
                            // Bidirectional
                            prn.cap (i, j, cap);
                            prn.cap (j, i, cap);
                        } else {
                            prn.cap (i, j, 0);
                        }
                    }
                }
            }
            // Take care of (v,t) capacities
            Art::Vec2i pos                = _pr_deindex (i);
            Art::TintLayerComponent * tlc = this->components[pos[1]][pos[0]];
            prn.cap (i, _pr_sink_index, 0_i64 + tlc->maximal_moment_chromosaturation);
            // Reset bristle arcs
            for (usize j = this->_pr_sink_index; j < prn.N; j++) {
                if (prn.cap (j, i) > 0) {
                    prn.cap (j, i, 0);
                }
            }
            // maximal moment chromosaturation equation
            // Increases exponentially with
            components[y][x]->maximal_moment_chromosaturation = static_cast<long double> (pl->components[y][x]->saturability)
                * (std::exp (-(static_cast<long double> (components[y][x]->tint.quantity) / TLAYER_TQ_EP0)))
                * (std::exp (-(static_cast<long double> (wl->components[y][x]->hydrosaturation) / TLAYER_TQ_EP0)));
        }
    }

    // Attach the brush
    this->brush->_pr_attach (&prn, size);

    // Zero flows, final prep
    prn.ready ();
}

void Art::TintLayer::_pr_run ()
{
    // Simple passthrough
    prn.compute ();
}

void Art::TintLayer::_pr_accrete (YNK_UNUSED Art::PaperLayer * pl, YNK_UNUSED Art::WaterLayer * wl)
{
    // Accrete the tint onto the paper
    i64 w = size[0], h = size[1];
    Ynk::i64 quantities[size[1].inner_][size[0].inner_];
    for (i64 y = 0; y < h; y++) {
        for (i64 x = 0; x < w; x++) {
            quantities[y][x] = prn.flow (_pr_index (x, y), _pr_sink_index);

            i64 addition_quantity = Math::min (quantities[y][x], -(-components[y][x]->maximal_moment_chromosaturation));
            Art::Tint addition { brush->ink, 0_u64 + addition_quantity };
            components[y][x]->tint.blend (addition);
        }
    }
}
