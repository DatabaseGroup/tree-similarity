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

#pragma once

#include <vector>
#include <string>

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
// Types and type aliases
public:
  using Reference = Label&;
  using ConstReference = const Label&;

  using SizeType = typename std::vector<Node<Label>>::size_type;

// Member functions
public:
  Node(ConstReference label);

  /// Returns the number of children of this node.
  ///
  /// \return Number of children (i.e., entries in children_).
  const SizeType children_count() const;

  /// Retrieves the label.
  ///
  /// \return The label of this node.
  ConstReference label() const;

  /// Adds a child at last position.
  /// TODO: Verify this definition. Can we improve it with & or &&? It is
  ///       used to build the tree in a parser.
  /// TODO: Verify if child is copied or moved.
  ///
  /// \param Node to be added.
  Node<Label>& add_child(Node<Label> child);

  /// Returns the reference to children_ vector.
  ///
  /// \return Reference to children_ vector.
  const std::vector<Node<Label>>& get_children() const;

  /// Calculates the size (number of nodes) in tree rooted at this. It traverses
  /// the entire subtree recursively.
  ///
  /// \return Size of subtree rooted at this.
  int get_tree_size() const;

  /// Traverses the tree and collects string representations of all labels.
  ///
  /// \return Vector with labels of all nodes.
  const std::vector<std::string> get_all_labels() const;
  void get_all_labels_recursion(std::vector<std::string>& labels) const;

  /// Verifies if this node is a leaf.
  ///
  /// \return True if this node is leaf, false otherwise.
  bool is_leaf() const;

// Member variables
private:
  /// All children of this node.
  std::vector<Node<Label>> children_;

  /// Data representing the label of this node. Only this Node object owns it.
  Label label_;
};

// Implementation details
#include "node_impl.h"

}
