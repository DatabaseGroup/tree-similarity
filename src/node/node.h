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

/// \file node/node.h
///
/// \details
/// Contains the declaration of the Node class (represents a node in a tree).

#ifndef TREE_SIMILARITY_NODE_NODE_H
#define TREE_SIMILARITY_NODE_NODE_H

#include <vector>
#include <memory>

#include <iostream>

namespace node {

/// \class Node
///
/// \details
/// Represents a node in a tree. Every node holds some satellite data
/// representing the label (Label).
/// The type of the label is parameterized using Label and can be of
/// arbitrary type as long as the requirements for computing the tree edit
/// distance are met. We refer to the Costs class (in this file) for further
/// information on the requirements.
/// The children of a node are stored in a dynamic array of Node pointers. This
/// class provides all necessary methods for the tree algorithms. All fields are
/// declared
/// 
/// \remark
/// Typically, the user of this framework does not need to modify this class by
/// any means. If one decides to modify this class it may actually break some
/// tree algorithms, so be careful.
///
/// \tparam Label Satellite data associated with the node.
template <class Label>
class Node {
/// Types and type aliases
public:
  using Reference = Label&;
  using ConstReference = const Label&;

  using SizeType = typename std::vector<Node<Label>>::size_type;

/// Member functions
public:
  Node(ConstReference label);

  /// Retrieves current number of children.
  ///
  /// \return Current number of children (i.e., entries in children_).
  SizeType children_number() const;

  ///
  ConstReference label() const;

  /// Adds a child at last position.
  ///
  /// \param child Pointer to the node to be added.
  void add_child(const Node<Label>& child);

/// Member variables
public:
  /// Pointers to all children of this node.
  std::vector<Node<Label>> children_;

  /// Data representing the label of this node. Only this Node object owns it.
  Label label_;

/// Member functions
private:
  /// Computes the size of the subtree rooted at this node iteratively,
  /// resp. recursively.
  ///
  /// \return Size of the subtree rooted at this node (incl. this node).
  /// @{
  //SizeType subtree_size_iterative() const;
  //SizeType subtree_size_recursive() const;
  /// @}
};

#include "node_impl.h"

}

#endif // TREE_SIMILARITY_NODE_NODE_H
