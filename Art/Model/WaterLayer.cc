//
// file art/model/WaterLayer.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>
#include <Ynk/Math/Math.hh>

#include <cmath>
#include <algorithm>

using namespace Ynk;

// Layer Component

Art::WaterLayerComponent::WaterLayerComponent ()
    : hydrosaturation { 0 }
    , maximal_moment_hydrosaturation { 0 }
{}

// Layer Proper

Art::WaterLayer::WaterLayer (Art::Vec2i size, Brush * brush)
    : size { size }
    , brush { brush }
    , components { nullptr }
    , prn (1 + size.space->op_vec_product (size) + brush->_pr_num_nodes ())
    , _pr_sink_index ((size[0] * size[1]).inner_)
{
    // Initialize all the values--
    // create the water layer component grid
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
    prn.stabilize ();
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

void Art::WaterLayer::_pr_construct (YNK_UNUSED Art::PaperLayer * pl)
{
    // Construct the spine of the flow network, , i.e. the grid layer ~ the intragrid
    // edges
    i64 w = this->size[0], h = this->size[1];
    for (i64 x = 0; x < w; x++) {
        for (i64 y = 0; y < h; y++) {
            for (i64 ix = (x != 0 ? -1 : 0); ix <= (x != w - 1 ? 1 : 0); ix++) {
                for (i64 iy = (y != 0 ? -w : 0_i64); iy <= (y != h - 1 ? w : 0_i64); iy += w) {
                    if (LIKELY (!(ix == 0 && iy == 0))) {
                        prn.cap (_pr_index (x, y), _pr_index (x, y) + ix + iy, WLAYER_SLIDE_QTY);
                        prn.cap (_pr_index (x, y) + ix + iy, _pr_index (x, y), WLAYER_SLIDE_QTY);
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
        Art::Vec2i pos                 = _pr_deindex (i);
        Art::WaterLayerComponent * wlc = this->components[pos[1]][pos[0]];
        // Set up the sink-terminal edges
        prn.cap (i, _pr_sink_index, 0_i64 + wlc->maximal_moment_hydrosaturation);
        // Reset bristle-to-water arcs ~ sink-originating arcs
        for (usize j = _pr_sink_index; j < prn.N; j++) {
            if (prn.cap (j, i) > 0) {
                prn.cap (j, i, 0);
            }
        }
    }

    // Attach the brush
    brush->_pr_attach (&prn, size);
    // Zero flows, prep & all
    prn.ready ();
}

void Art::WaterLayer::_pr_run ()
{
    // Really simple passthrough fn
    prn.compute ();
}

void Art::WaterLayer::_pr_accrete (Art::PaperLayer * pl)
{
    // Accrete the water
    i64 w = size[0], h = size[1];
    Ynk::i64 quantities[size[1].inner_][size[1].inner_];
    for (i64 y = 0; y < h; y++) {
        for (i64 x = 0; x < w; x++) {
            quantities[y][x] = prn.flow (_pr_index (x, y), _pr_sink_index);

            // -(-u64) -> i64
            i64 sat_delta = Math::min (quantities[y][x], -(-components[y][x]->maximal_moment_hydrosaturation));

            components[y][x]->hydrosaturation += sat_delta;

            // Drying process...
            components[y][x]->hydrosaturation = (long double)components[y][x]->hydrosaturation * WLAYER_DRY_RATE;

            // The saturation equation
            if (components[y][x]->hydrosaturation) {
                components[y][x]->maximal_moment_hydrosaturation = WLAYER_SAT_EP1 * pl->components[y][x]->saturability / WLAYER_T_EP2;
                components[y][x]->maximal_moment_hydrosaturation = static_cast<long double> (components[y][x]->maximal_moment_hydrosaturation)
                    * (std::exp (-(static_cast<long double> (components[y][x]->hydrosaturation) / WLAYER_T_EP3)));
            } else {
                components[y][x]->maximal_moment_hydrosaturation = WLAYER_SAT_EP1 * pl->components[y][x]->saturability / WLAYER_T_EP2;
            }
        }
    }
}
