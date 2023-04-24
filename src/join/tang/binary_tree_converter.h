// The MIT License (MIT)
// Copyright (c) 2018 Thomas Huetter
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

/// \file join/tang/binary_tree_converter.h
///
/// \details
/// Takes a collection of trees and and converts them into a collection of binary  
/// trees. Knuth's algorithm is applied to create a left child - right ancestor 
/// (LC-RS) binary tree. 

#pragma once

#include <vector>
#include <unordered_map>
#include "../../node/binary_node.h"
#include "../../label/string_label.h"

namespace binary_tree_converter {

template <typename Label>
class Converter {
// Member functions.
public:
  /// Constructor.
  Converter();
  /// Converts a given collection of trees to a collection of binary trees.
  ///
  /// \param tree_collection A collection of trees.
  /// \param binary_trees_collection A collection of binary trees.
  void convert(std::vector<node::Node<Label>>& trees_collection,
      std::vector<node::BinaryNode<Label>>& binary_trees_collection);
// Member functions.
private:
  /// Recursively transforms a tree into a binary tree.
  ///
  /// \param tree_node Current node of the original tree. 
  ///                  Initially, the root node of the original tree.
  /// \param binary_tree_node Current node of the binary tree. 
  ///                         Initially, the root node of the binary tree.
  void create_binary_tree(const node::Node<Label>& tree_node,
      node::BinaryNode<Label>& binary_tree_node);
};

// Implementation details.
#include "binary_tree_converter_impl.h"

}

