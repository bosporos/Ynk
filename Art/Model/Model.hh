//
// file art/Model.hh
// author Maximilien M. Cura
//

#ifndef __ART_MODEL
#define __ART_MODEL

#include <Art/PushRelabel/PushRelabel.hh>
#include <Ynk/Geometry/Space.hh>
#include <Ynk/Num/NativeFloats.hh>
#include <Ynk/UX/Color.hh>

namespace Art {
    // Some boilerplate;

    // Vec3d is used by the Brush and Bristle classes, because they exist in
    // continuous 3-dimensional space
    typedef Ynk::Vec<3, Ynk::_f64> Vec3d;
    typedef Ynk::Space<3, Ynk::_f64> Space3d;

    // Whereas the component layers exist in discrete 2-dimensional space.
    typedef Ynk::Vec<2, Ynk::i64> Vec2i;
    typedef Ynk::Space<2, Ynk::i64> Space2i;

    struct Tint
    {
        Ynk::UX::RGBA color;
        Ynk::u64 quantity;

        void blend (Tint);
    };

    //
    // BRUSH
    //

    struct Bristle
    {
        // Position of the tip of the bristle in 3d space, using the Brush's position
        // as the origin
        Vec3d tip_offset;
        // Handled by TintLayer
        Ynk::u64 tint_remaining;

        Bristle (Vec3d, Ynk::u64);
    };

    struct Brush
    {
        // Number of bristles
        Ynk::usize num_bristles;
        // The actual bristles themselves
        Bristle * bristles;

        // The position of the brush
        Vec3d position;
        // The ink that the brush uses
        Ynk::UX::RGBA ink;

        Brush (Bristle *, Ynk::usize, Ynk::UX::RGBA);
        // C++ magic;
        Brush (Brush && husk);

        // Number of nodes taken up by the brush in the flow network.
        // Written for a future possible implementation that allows transfer between bristles within the brush
        // that *would* need nodes of its owns
        Ynk::usize _pr_num_nodes ();
        // Attach the brush to a flow network
        void _pr_attach (PushRelabelNetwork *, Vec2i);
    };

    //
    // LAYER COMPONENTS
    //

    // The layer components are all simple, raw data structures

    struct WaterLayerComponent
    {
        // The amount of water saturated into the paper
        Ynk::u64 hydrosaturation;
        // The maximal moment hydrosaturation of the component
        Ynk::u64 maximal_moment_hydrosaturation;

        WaterLayerComponent ();
    };

    struct TintLayerComponent
    {
        // Maximal moment chromosaturation
        Ynk::u64 maximal_moment_chromosaturation;
        // Tint of the component
        Tint tint;

        TintLayerComponent ();
    };

    struct PaperLayerComponent
    {
        // The base saturability of the paper
        Ynk::u64 saturability;
        // The base coloration of the paper
        Tint tint;

        PaperLayerComponent (Ynk::u64, Tint);
    };

    //
    // LAYERS
    //

    // Internal abstraction structure; basically just sets up the values in a PaperLayer
    struct PaperConfiguration
    {
        Ynk::u64 saturability_real (Art::Vec2i);
        Tint paper_color_real (Art::Vec2i);
    };

#define ART_DEFAULT_PAPER_CONFIGURATION PaperConfiguration ()

    // The paperlayer is inert-- there's not a lot that's happening here
    struct PaperLayer
    {
        // Size of the layer
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
#define WLAYER_T_EP3 64.0
#define WLAYER_DRY_RATE 0.942L

    struct WaterLayer
    {
        Art::Vec2i size;
        Brush * brush;
        WaterLayerComponent *** components;

        PushRelabelNetwork prn;

        // Index of the sink in the push-relabel network
        Ynk::usize _pr_sink_index;

        WaterLayer (Art::Vec2i, Brush * brush);
        ~WaterLayer ();

        // Initializer
        void _pr_construct (PaperLayer *);
        // Readies the network for an iteration
        void _pr_ready ();
        // Runs the network
        void _pr_run ();
        // Accretes the flow network onto the water component layer
        // Needs the paperlayer for the saturability function
        void _pr_accrete (PaperLayer *);

        // For converting between indexed nodes in the flow network and positions on screen
        inline Ynk::usize _pr_index (Ynk::i64 x, Ynk::i64 y) { return (x + (y.inner_ * size[1].inner_)).inner_; }
        inline Vec2i _pr_deindex (Ynk::usize i) { return size.space->create_vec ({ i.inner_ % size[1].inner_, i.inner_ / size[1].inner_ }); }
    };

#define TLAYER_TQ_EP0 64.0L

    // TintLayer has essentially the exact same structure as WaterLayer.
    // The only differences are the _pr_* functions, which additionally need
    // to take the water layer as a parameter because tint is intrinsically
    // dependent on water-- tint can only flow through areas of the paper that
    // have been saturated with water
    struct TintLayer
    {
        Art::Vec2i size;
        Brush * brush;
        TintLayerComponent *** components;

        PushRelabelNetwork prn;

        Ynk::usize _pr_sink_index;

        TintLayer (Art::Vec2i, Brush * brush);
        ~TintLayer ();

        void _pr_construct (PaperLayer *, WaterLayer *);
        void _pr_ready (PaperLayer *, WaterLayer *);
        void _pr_run ();
        void _pr_accrete (PaperLayer *, WaterLayer *);

        // For converting between indexed nodes in the flow network and positions on screen
        inline Ynk::usize _pr_index (Ynk::i64 x, Ynk::i64 y) { return (x + (y.inner_ * size[1].inner_)).inner_; }
        inline Vec2i _pr_deindex (Ynk::usize i) { return size.space->create_vec ({ i.inner_ % size[1].inner_, i.inner_ / size[1].inner_ }); }
    };
}

#endif /* !@__ART_MODEL */
