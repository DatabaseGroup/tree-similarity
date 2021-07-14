// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter.
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

/// \file lookup/index/label_set_element.h
///
/// \details
/// Structure a label set element. Containing the id, number of nodes per label, 
/// number of nodes until the actual element and positional information per node.

#pragma once

#include <vector>

namespace label_set_converter_index {
/// This is an element holding relevant data of a set.
struct LabelSetElement {
  /// Token ID.
  long int id = 0;
  /// Postorder ID of the label in the according tree.
  long int postorder_id = 0;
  /// Number of ancestors.
  long int ancestors = 0;
  /// Number of descendants.
  long int descendants = 0;
  /// Weight of a token.
  int weight = 0;
  /// Weight of a token.
  int weight_so_far = 0;
  /// Constructor.
  LabelSetElement();
  /// Constructor.
  LabelSetElement(long int id, int weight);
  /// Constructor.
  LabelSetElement(long int id, long int postorder_id, int weight);
  /// Constructor.
  LabelSetElement(long int id, long int postorder_id, int weight, long int ancestors, long int descendants);
};

label_set_converter_index::LabelSetElement::LabelSetElement() {}

label_set_converter_index::LabelSetElement::LabelSetElement(long int id, int weight)
    : id(id), weight(weight) {}

label_set_converter_index::LabelSetElement::LabelSetElement(long int id, long int postorder_id, int weight)
    : id(id), postorder_id(postorder_id), weight(weight) {}

label_set_converter_index::LabelSetElement::LabelSetElement(long int id, long int postorder_id, int weight, long int ancestors, long int descendants)
    : id(id), postorder_id(postorder_id), ancestors(ancestors), descendants(descendants), weight(weight) {}
}
