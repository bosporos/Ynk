//
// file art/Model.hh
// author Maximilien M. Cura
//

#ifndef __ART_MODEL
#define __ART_MODEL

#include <Art/PushRelabel.hh>

namespace Art {

    //
    // LAYER COMPONENTS
    //

    struct WaterLayerComponent
    {
        Ynk::u64 hydrosaturation;
        Ynk::u64 maximal_hydrosaturation;
    };

    struct TintLayerComponent
    {
        Ynk::u64 chromosaturation;
        Ynk::u64 maximal_chronosaturation;

        Ynk::u32 tint;
    };

    struct PaperLayerComponent
    {
        Ynk::u64 saturability;

        Ynk::u64 tint_weight;
        Ynk::u32 tint;
    };

    //
    // LAYERS
    //

    struct WaterLayer
    {
        Ynk::usize width, height;
        WaterLayerComponent ** components;

        PushRelabelNetwork pln;
    };

    struct TintLayer
    {
        Ynk::usize width, height;
        TintLayerComponent ** components;

        PushRelabelNetwork pln;
    };

    struct PaperLayer
    {
        Ynk::usize width, height;
        PaperLayerComponent ** components;
    };

    //
    // BRUSH
    //

    struct Bristle
    {
    };

    struct Brush
    {
        Ynk::usize num_bristles;
        Bristle * bristles;
    };

    //
    // MODEL
    //

    struct Model
    {
        WaterLayer water_layer;
        TintLayer tint_layer;
        PaperLayer paper_layer;

        Brush brush;
    };
}

#endif /* !@__ART_MODEL */
