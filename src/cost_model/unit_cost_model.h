// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy 
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights 
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell 
// copies of the Software, and to permit persons to whom the Software is 
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, 
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE 
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER 
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file cost_model/unit_cost_model.h
///
/// \details
/// Contains the declaration of a basic cost model, i.e., the unit costs.

#ifndef TREE_SIMILARITY_COST_MODEL_UNIT_COST_MODEL_H
#define TREE_SIMILARITY_COST_MODEL_UNIT_COST_MODEL_H

#include "node.h"

namespace cost_model {

/// \class UnitCosts
///
/// \details
/// Represents the unit cost model to be used for the distance computation.
/// Costs are generic for different node classes. A cost model has to provide
/// three cost functions:
///  - ren(n1, n2); cost of updating the data (label) of n1 to the data (label)
///    of n2.
///  - del(n); cost of deleting node n.
///  - ins(n); cost of inserting node n.
/// All three cost functions must return an integer.
template <class _Label>
struct UnitCostModel {
    /// Basic rename cost function (unit cost model).
    ///
    /// \param node1 The node to be renamed.
    /// \param node2  The node having the desired name.
    /// \return Integer cost of renaming node1 to node2.
    int ren(const node::Node<_Label>& node1, const node::Node<_Label>& node2);

    /// Basic delete cost function.
    ///
    /// \param node The node to be deleted.
    /// \return Integer cost of deleting node.
    int del(const node::Node<_Label>& node);

    /// Basic insert cost function.
    ///
    /// \param node The node to be inserted.
    /// \return Integer cost of inserting node.
    int ins(const node::Node<_Label>& node);
};

/// Implementational details
#include "unit_cost_model_impl.h"

}

#endif // TREE_SIMILARITY_COST_MODEL_UNIT_COST_MODEL_H
