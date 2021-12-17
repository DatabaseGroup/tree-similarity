// The MIT License (MIT)
// Copyright (c) 2018 Thomas Huetter.
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

/// \file join/tjoin/label_set_element.h
///
/// \details
/// Structure a label set element. Containing the id, number of nodes per label, 
/// number of nodes until the actual element and positional information per node.

#pragma once

#include <vector>

namespace label_set_converter {

/// This is an element holding positional data of a set element.
struct StructuralVector {
  /// Postorder ID of the label in the according tree.
  int postorder_id = 0;
  /// Number of nodes to the left of a node.
  int number_nodes_left = 0;
  /// Number of nodes to the right of a node.
  int number_nodes_right = 0;
  /// Number of ancestors of a node.
  int number_nodes_ancestor = 0;
  /// Number of descendants of a node.
  int number_nodes_descendant = 0;
  /// Constructor.
  StructuralVector();
  /// Constructor.
  StructuralVector(int postorder_id, 
          int number_nodes_left, int number_nodes_right, 
          int number_nodes_ancestor, int number_nodes_descendant);
};

label_set_converter::StructuralVector::StructuralVector() {}

label_set_converter::StructuralVector::StructuralVector(
  int postorder_id, int number_nodes_left, 
  int number_nodes_right, int number_nodes_ancestor, int number_nodes_descendant)
    : postorder_id(postorder_id),
      number_nodes_left(number_nodes_left), 
      number_nodes_right(number_nodes_right),
      number_nodes_ancestor(number_nodes_ancestor), 
      number_nodes_descendant(number_nodes_descendant) {}


/// This is an element holding relevant data of a set.
struct LabelSetElement {
  /// Token ID.
  int id = 0;
  /// Weight of a token.
  int weight = 0;
  /// Weight of a token.
  int weight_so_far = 0;
  /// Overlap so far for a specific set.
  std::vector<label_set_converter::StructuralVector> struct_vect;
  /// Constructor.
  LabelSetElement();
  /// Constructor.
  LabelSetElement(int id, int weight);
  /// Constructor.
  LabelSetElement(int id, int weight, 
      std::vector<label_set_converter::StructuralVector> &&struct_vect);
};

label_set_converter::LabelSetElement::LabelSetElement() {}

label_set_converter::LabelSetElement::LabelSetElement(int id, int weight)
    : id(id), weight(weight) {}

label_set_converter::LabelSetElement::LabelSetElement(int id, int weight, 
      std::vector<label_set_converter::StructuralVector> &&struct_vect)
    : id(id), weight(weight), struct_vect(std::move(struct_vect)) {}
}
