//
// file art/model/PaperLayer.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>

#include <Ynk/Math/Math.hh>

using namespace Ynk;
using Art::Model;

// Paper Layer Component

Art::PaperLayerComponent::PaperLayerComponent (u64 saturability,
                                               Tint base)
    : saturability { saturability }
    , tint { base }
{}

void Art::PaperLayerComponent::add_tint (YNK_UNUSED Art::Tint addition)
{
}

// Paper Configuration

u64 Art::PaperConfiguration::saturability_real (YNK_UNUSED Art::Vec2i pos)
{
    return 0x800;
}

Art::Tint Art::PaperConfiguration::paper_color_real (YNK_UNUSED Art::Vec2i pos)
{
    return Tint {
        UX::hsva (349, 0.07, 0.96, 1.0),
        80
        // 0
    };
}

// Paper Layer

Art::PaperLayer::PaperLayer (Art::Vec2i size, Art::PaperConfiguration pc)
    : size { size }
    , components { nullptr }
{
    this->components = new PaperLayerComponent **[size[1]];
    for (i64 i = 0; i < size[1]; i++) {
        this->components[i] = new PaperLayerComponent *[size[0]];
        for (i64 j = 0; j < size[0]; j++) {
            this->components[i][j] = new PaperLayerComponent (
                pc.saturability_real (size.space->create_vec ({ j, i })),
                pc.paper_color_real (size.space->create_vec ({ j, i })));
        }
    }
}

Art::PaperLayer::~PaperLayer ()
{
    for (i64 i = 0; i < size[1]; i++) {
        for (i64 j = 0; j < size[0]; j++) {
            delete this->components[i][j];
        }
        delete[] this->components[i];
    }
    delete[] this->components;
}
