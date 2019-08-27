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
    // color, quantity
    quantity += rhs.quantity;
    double q2 = (double)quantity * (double)quantity;
    u8 end_alpha;
    if (q2 <= TLAYER_TQ_EP2)
        end_alpha = std::min (0xff_u8, static_cast<u8> (q2 / TLAYER_TQ_EP1));
    // if ((q2 / TLAYER_TQ_EP1) < 255)
    // end_alpha = 0;
    // end_alpha = 0;
    else {   // q2 > TLAYER_TQ_EP1
        double q3 = static_cast<u8> (q2 / TLAYER_TQ_EP1);
        end_alpha = static_cast<u8> (255.0 * std::exp (-q3 / TLAYER_TQ_EP1));
    }
    // end_alpha = 255;
    // u8 end_alpha      = 0_u8 + std::min (0xff_u8, static_cast<u8> (((double)quantity * (double)quantity + (double)quantity) / 412.0));
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
    // N/A
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
    for (i64 x = 0; x < w; x++) {
        for (i64 y = 0; y < h; y++) {
            Ynk::usize i = _pr_index (x, y);
            // Lateral capacity settings
            for (i64 ix = (x != 0 ? -1 : 0); ix <= (x != w - 1 ? 1 : 0); ix++) {
                for (i64 iy = (y != 0 ? -w : 0_i64); iy <= (y != h - 1 ? w : 0_i64); iy += w) {
                    if (LIKELY (!(ix == 0 && iy == 0))) {
                        Ynk::usize j = _pr_index (x, y) + ix + iy;
                        if (wl->prn.flow (i, j) > 0) {
                            Ynk::i64 cap = static_cast<Ynk::_i64> (
                                static_cast<long double> (wl->prn.flow (i, j)) * 0.75L);
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
            components[y][x]->maximal_moment_chromosaturation = static_cast<long double> (pl->components[y][x]->saturability)
                * (std::exp (-(static_cast<long double> (components[y][x]->tint.quantity) / TLAYER_TQ_EP0))) * (std::exp (-(static_cast<long double> (wl->components[y][x]->hydrosaturation) / TLAYER_TQ_EP0)));
        }
    }

    // Attach the brush
    this->brush->_pr_attach (&prn, size);

    // Zero flows, final prep
    prn.ready ();
}

void Art::TintLayer::_pr_run ()
{
    prn.compute ();
}

void Art::TintLayer::_pr_accrete (Art::PaperLayer * pl, Art::WaterLayer * wl)
{
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
