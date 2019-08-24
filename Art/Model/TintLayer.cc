//
// file art/model/TintLayer.cc
// author Maximilien M. Cura
//

#include <Art/Model/Model.hh>

using namespace Ynk;
using namespace Art;

TintLayer::TintLayer (Art::Vec2i size, Art::Brush * brush)
    : size { size }
    , brush { brush }
    , pln (1 + size.space->op_vec_product (size) + brush->_pr_num_nodes ())
{}

TintLayer::~TintLayer ()
{}
