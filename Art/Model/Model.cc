//
// file art/Model/Model.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>

#include <Ynk/Lang/Move.hh>

using namespace Ynk;

using Art::Model;

Model::Model (Art::Brush brush, Art::Vec2i paper_size)
    : size { paper_size }
    , space { paper_size.space }
    , water_layer (paper_size, &brush)
    , tint_layer (paper_size, &brush)
    , paper_layer (paper_size, ART_DEFAULT_PAPER_CONFIGURATION)
    , brush { Ynk::Move (brush) }
{}
