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

/// Contains the declaration of a basic cost model, i.e., the unit costs.

#pragma once

#include "node.h"
#include "label_dictionary.h"

namespace cost_model {

// TODO: Deprecated. Substitute with UnitCostModelLD.
template <class Label>
struct UnitCostModel {
    int ren(const node::Node<Label>& node1, const node::Node<Label>& node2) const;
    int del(const node::Node<Label>& node) const;
    int ins(const node::Node<Label>& node) const;
};

/// Represents the unit cost model to be used for the distance computation.
/**
 * Costs are generic for different node classes. A cost model has to provide
 * three cost functions:
 * - ren(label_id_1, label_id_2): cost of renaming label with label_id_1 to
 *   label with label_id_2.
 * - del(label_id): cost of deleting node with label_id.
 * - ins(label_id): cost of inserting node with label_id.
 * All three cost functions must return a double.
 */
template <typename Label>
struct UnitCostModelLD {
  /// Label dictionary in case the labels are needed for cost calculations.
  const label::LabelDictionary<Label>& ld_;
  
  /// Constructor. Takes LabelDictionary.
  UnitCostModelLD(label::LabelDictionary<Label>& ld);
  
  /// Basic rename cost function (unit cost model).
  /**
   * \param label_id_1 Source label id.
   * \param label_id_2 Destination label id.
   * \return Double cost of renaming label_id_1 to label_id_1.
   */
  double ren(const int label_id_1, const int label_id_2) const;
  
  /// Basic delete cost function.
  /**
   * \param label_id Label id to be deleted.
   * \return Double cost of deleting a node with label_id.
   */
  double del(const int label_id) const;
  
  /// Basic insert cost function.
  /**
   * \param label_id Label id to be inserted.
   * \return Double cost of inserting a node with label_id.
   */
  double ins(const int label_id) const;
};

// Implementational details
#include "unit_cost_model_impl.h"

}
