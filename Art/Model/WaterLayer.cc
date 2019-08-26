//
// file art/model/WaterLayer.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>
#include <Ynk/Math/Math.hh>

#include <cmath>
#include <algorithm>

using namespace Ynk;

using Art::Model;

Art::WaterLayerComponent::WaterLayerComponent ()
    : hydrosaturation { 0 }
    , standing_water { 0 }
    , maximal_moment_hydrosaturation { 0 }
{}

Art::WaterLayer::WaterLayer (Art::Vec2i size, Brush * brush)
    : size { size }
    , brush { brush }
    , components { nullptr }
    , prn (1 + size.space->op_vec_product (size) + brush->_pr_num_nodes ())
    , _pr_sink_index ((size[0] * size[1]).inner_)
{
    this->components = new WaterLayerComponent **[size[1]];
    for (i64 i = 0; i < size[1]; i++) {
        this->components[i] = new WaterLayerComponent *[size[0]];
        for (i64 j = 0; j < size[0]; j++) {
            this->components[i][j] = new WaterLayerComponent ();
        }
    }
    prn.S    = _pr_sink_index + 1;
    prn.T    = _pr_sink_index;
    prn.EOLN = _pr_sink_index;
}

Art::WaterLayer::~WaterLayer ()
{
    for (i64 i = 0; i < this->size[1]; i++) {
        for (i64 j = 0; j < this->size[0]; j++) {
            delete this->components[i][j];
        }
        delete[] this->components[i];
    }
    delete[] this->components;
}

void Art::WaterLayer::_pr_construct (Art::PaperLayer * pl)
{
    i64 w = this->size[0], h = this->size[1];
    for (i64 x = 0; x < w; x++) {
        for (i64 y = 0; y < h; y++) {
            for (i64 ix = (x != 0 ? -1 : 0); ix <= (x != w - 1 ? 1 : 0); ix++) {
                for (i64 iy = (y != 0 ? -w : 0_i64); iy <= (y != h - 1 ? w : 0_i64); iy += w) {
                    if (LIKELY (!(ix == 0 && iy == 0))) {
                        prn.capacities[_pr_index (x, y)][_pr_index (x, y) + ix + iy] = WLAYER_SLIDE_QTY;
                        prn.capacities[_pr_index (x, y) + ix + iy][_pr_index (x, y)] = WLAYER_SLIDE_QTY;
                    }
                }
            }
        }
    }

    // Sink links & brush attachment are handled per-iteration in _pr_ready();
}

void Art::WaterLayer::_pr_ready ()
{
    // Assume components have already been updated

    // Basically, we have to reset the (v,t) arc capacities, and fix them all
    for (usize i = 0; i < this->_pr_sink_index; i++) {
        Art::Vec2i pos                    = _pr_deindex (i);
        Art::WaterLayerComponent * wlc    = this->components[pos[1]][pos[0]];
        prn.capacities[i][_pr_sink_index] = wlc->maximal_moment_hydrosaturation;
        // Reset bristle-to-water arcs
        for (usize j = _pr_sink_index; j < prn.N; j++) {
            if (prn.capacities[j][i] > 0) {
                prn.capacities[j][i] = 0;
            }
        }
    }

    brush->_pr_attach (&prn, size);
    // Zero flows, prep & all
    prn.ready ();
}

void Art::WaterLayer::_pr_run ()
{
    prn.compute ();
}

void Art::WaterLayer::_pr_accrete (Art::PaperLayer * pl)
{
    i64 w = size[0], h = size[1];
    Ynk::i64 quantities[size[1].inner_][size[1].inner_];
    for (i64 y = 0; y < h; y++) {
        for (i64 x = 0; x < w; x++) {
            quantities[y][x] = prn.flows[_pr_index (x, y)][_pr_sink_index];

            // -(-u64) -> i64
            i64 sat_delta         = Math::min (quantities[y][x], -(-components[y][x]->maximal_moment_hydrosaturation));
            i64 sat_reverse_delta = quantities[y][x] - sat_delta;

            components[y][x]->hydrosaturation += sat_delta;
            components[y][x]->standing_water += sat_reverse_delta;

            components[y][x]->hydrosaturation = (long double)components[y][x]->hydrosaturation * WLAYER_DRY_RATE;

            if (components[y][x]->hydrosaturation) {
                // components[y][x]->maximal_moment_hydrosaturation
                //     /* more residual saturation capacity = higher moment saturability */
                //     = WLAYER_SAT_EP1 * (((pl->components[y][x]->saturability / WLAYER_T_EP2 - components[y][x]->hydrosaturation) / components[y][x]->hydrosaturation) + 1)
                //     /* more standing water = less moment saturability */
                //     * WLAYER_SAT_EP0 / (WLAYER_SAT_EP0 + components[y][x]->standing_water);

                // base value
                components[y][x]->maximal_moment_hydrosaturation = WLAYER_SAT_EP1 * pl->components[y][x]->saturability / WLAYER_T_EP2;
                components[y][x]->maximal_moment_hydrosaturation = static_cast<long double> (components[y][x]->maximal_moment_hydrosaturation)
                    * (std::exp (-(static_cast<long double> (components[y][x]->hydrosaturation) / 64.0L)));
            } else {
                components[y][x]->maximal_moment_hydrosaturation = WLAYER_SAT_EP1 * pl->components[y][x]->saturability / WLAYER_T_EP2;
            }
        }
    }

    // ~~Drying processes are run by the Model~~
}
