// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik, and Nikolaus Augsten
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
/// Contains two class declarations:
///  1. Node (represents a node in a tree)
///  2. Costs (a basic cost model)

#ifndef TREE_SIMILARITY_NODE_NODE_H
#define TREE_SIMILARITY_NODE_NODE_H

#include <vector>
#include <memory>

namespace node {

/// \class Node
///
/// \details
/// Represents a node in a tree. Every node holds some satellite data
/// representing the label (_Label).
/// The type of the label is parameterized using _Label and can be of
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
/// \tparam _Label Satellite data associated with the node.
template <class _Label>
class Node {
public:
  /*using LabelUniquePtr = std::unique_ptr<_Label>;
  using LabelReference = _Label&;
  using ConstLabelReference = const LabelReference;

  using NodeUniquePtr = std::unique_ptr<Node<_Label>>;
  using NodeReference = Node<_Label>&;
  using ConstNodeReference = const NodeReference;

  using SizeType = typename std::vector<NodeUniquePtr>::size_type;*/

public:
  Node(_Label data);

  /// Retrieves current number of children.
  ///
  /// \return Current number of children (i.e., entries in children_).
  int children_number() const;

  ///
  const _Label& label() const;

  /// Adds a child at last position.
  ///
  /// \param child Pointer to the node to be added.
  void add_child(std::unique_ptr<Node<_Label>> child);

private:
  /// Pointers to all children of this node.
  std::vector<std::unique_ptr<Node<_Label>>> children_;

  /// Data representing the label of this node. Only this Node object owns it.
  _Label label_;

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
