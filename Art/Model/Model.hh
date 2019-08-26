//
// file art/Model.hh
// author Maximilien M. Cura
//

#ifndef __ART_MODEL
#define __ART_MODEL

#include <Art/PushRelabel.hh>
#include <Ynk/Geometry/Space.hh>
#include <Ynk/Num/NativeFloats.hh>
#include <Ynk/UX/Color.hh>

namespace Art {
    typedef Ynk::Vec<3, Ynk::_f64> Vec3d;
    typedef Ynk::Space<3, Ynk::_f64> Space3d;

    typedef Ynk::Vec<2, Ynk::i64> Vec2i;
    typedef Ynk::Space<2, Ynk::i64> Space2i;

    struct Tint
    {
        Ynk::UX::RGBA color;
        Ynk::u64 quantity;
    };

    //
    // BRUSH
    //

    struct Bristle
    {
        Vec3d tip_offset;
        // Handled by TintLayer
        Ynk::u64 tint_remaining;

        Bristle (Vec3d, Ynk::u64);
    };

    struct Brush
    {
        Ynk::usize num_bristles;
        Bristle * bristles;

        Vec3d position;
        Ynk::UX::RGBA ink;

        Brush (Bristle *, Ynk::usize, Ynk::UX::RGBA);
        Brush (Brush && husk);

        Ynk::usize _pr_num_nodes ();
        void _pr_attach (PushRelabelNetwork *, Vec2i);
    };

    //
    // LAYER COMPONENTS
    //

    struct WaterLayerComponent
    {
        Ynk::u64 hydrosaturation;
        Ynk::u64 standing_water;
        // Computed from prev. two + PaperLayerComponent::saturability
        Ynk::u64 maximal_moment_hydrosaturation;

        WaterLayerComponent ();
    };

    struct TintLayerComponent
    {
        Ynk::u64 chromosaturation;
        Ynk::u64 maximal_moment_chronosaturation;

        Tint tint;
    };

    struct PaperLayerComponent
    {
        Ynk::u64 saturability;

        Tint tint;

        PaperLayerComponent (Ynk::u64, Tint);
        void add_tint (Tint tint);
    };

    //
    // LAYERS
    //

    struct PaperConfiguration
    {
        Ynk::u64 saturability_real (Art::Vec2i);
        Tint paper_color_real (Art::Vec2i);
    };

#define ART_DEFAULT_PAPER_CONFIGURATION PaperConfiguration ()

    struct PaperLayer
    {
        Art::Vec2i size;
        PaperLayerComponent *** components;

        PaperLayer (Art::Vec2i, PaperConfiguration);
        ~PaperLayer ();
    };

/* maximum amt of water that can move along a given edge on the paper surface */
#define WLAYER_SLIDE_QTY 128
/* controls standing water moment hydrosaturability effect */
#define WLAYER_SAT_EP0 96
/* maximal amount on (u,t) edges is EP1 * paper::saturability */
#define WLAYER_SAT_EP1 1
#define WLAYER_T_EP2 64
#define WLAYER_DRY_RATE 0.9L

    struct WaterLayer
    {
        Art::Vec2i size;
        Brush * brush;
        WaterLayerComponent *** components;

        PushRelabelNetwork prn;

        Ynk::usize _pr_sink_index;

        WaterLayer (Art::Vec2i, Brush * brush);
        ~WaterLayer ();

        void _pr_construct (PaperLayer *);
        void _pr_ready ();
        void _pr_run ();
        void _pr_accrete (PaperLayer *);

        inline Ynk::usize _pr_index (Ynk::i64 x, Ynk::i64 y) { return (x + (y.inner_ * size[1].inner_)).inner_; }
        inline Vec2i _pr_deindex (Ynk::usize i)
        {
            return size.space->create_vec ({ i.inner_ % size[1].inner_, i.inner_ / size[1].inner_ });
        }
    };

    struct TintLayer
    {
        Art::Vec2i size;
        Brush * brush;
        TintLayerComponent *** components;

        PushRelabelNetwork pln;

        TintLayer (Art::Vec2i, Brush * brush);
        ~TintLayer ();
    };

    struct Model
    {
        Vec2i size;
        Space2i * space;

        WaterLayer water_layer;
        TintLayer tint_layer;
        PaperLayer paper_layer;

        Brush brush;

        Model (Brush brush, Vec2i paper_size);
    };
}

#endif /* !@__ART_MODEL */
