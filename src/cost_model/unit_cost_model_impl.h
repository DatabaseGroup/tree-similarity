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
/// Contains the implementation of a basic cost model, i.e., the unit costs.

#ifndef TREE_SIMILARITY_COST_MODEL_UNIT_COST_MODEL_IMPL_H
#define TREE_SIMILARITY_COST_MODEL_UNIT_COST_MODEL_IMPL_H

template <class _Label>
int UnitCostModel<_Label>::ren(
    const node::Node<_Label>& node1,
    const node::Node<_Label>& node2) {
  if (node1.label() == node2.label()) {
    return 0;
  }

  return 1;
}

template <class _Label>
int UnitCostModel<_Label>::del(const node::Node<_Label>& node) {
  return 1;
}

template <class _Label>
int UnitCostModel<_Label>::ins(const node::Node<_Label>& node) {
  return 1;
}

#endif // TREE_SIMILARITY_COST_MODEL_UNIT_COST_MODEL_IMPL_H
