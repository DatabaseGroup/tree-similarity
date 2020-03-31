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

/// \file node/binary_node.h
///
/// \details
/// Contains the declaration of the BinaryNode class (represents a node in a 
/// binary tree).

#pragma once

#include <string>

namespace node {

/// \class BinaryNode
///
/// \details
/// Represents a node in a binary tree. Every node holds some satellite data
/// representing the label (Label).
/// The type of the label is parameterized using Label and can be of
/// arbitrary type as long as the requirements for computing the tree edit
/// distance are met. We refer to the Costs class (in this file) for further
/// information on the requirements.
/// A node has at most two children, stored at two BinaryNode objects. This
/// class provides all necessary methods for the tree algorithms. All fields are
/// declared
///
/// \remark
/// Typically, the user of this framework does not need to modify this class by
/// any means. If one decides to modify this class it may actually break some
/// tree algorithms, so be careful.
///
/// \tparam Label Satellite data associated with the binary node.
template <class Label>
class BinaryNode {
// Types and type aliases
public:
  using Reference = Label&;
  using ConstReference = const Label&;

  using SizeType = typename std::vector<Node<Label>>::size_type;

// Member functions
public:
  BinaryNode(ConstReference label);

  /// Returns the number of children of this binary node.
  ///
  /// \return Number of children (i.e., entries in children_).
  const SizeType children_count() const;

  /// Retrieves the label.
  ///
  /// \return The label of this binary node.
  ConstReference label() const;

  /// Adds a copy of left_child as left child and returns its reference.
  ///
  /// \return Reference to the left child of this binary node.
  BinaryNode<Label>* add_left_child(std::unique_ptr<BinaryNode<Label>>& node);

  /// Adds a copy of right_child as right child and returns its reference.
  ///
  /// \return Reference to the right child of this binary node.
  BinaryNode<Label>* add_right_child(std::unique_ptr<BinaryNode<Label>>& node);

  /// Returns true if this binary node has a left child.
  ///
  /// \return True if this binary node has a left child.
  bool has_left_child();

  /// Returns true if this binary node has a right child.
  ///
  /// \return True if this binary node has a right child.
  bool has_right_child();

  /// Returns the left child of this binary node.
  ///
  /// \return The left child of this binary node.
  BinaryNode<Label>* get_left_child();

  /// Returns the right child of this binary node.
  ///
  /// \return The right child of this binary node.
  BinaryNode<Label>* get_right_child();

  /// Returns the reference to children_ vector.
  ///
  /// \return Reference to children_ vector.
  const std::vector<BinaryNode<Label>>& get_children() const;

  /// Calculates the size (number of binary nodes) in tree rooted at this. 
  /// It traverses the entire subtree recursively.
  ///
  /// \return Size of subtree rooted at this.
  int get_tree_size() const;

  /// Traverses the tree and collects string representations of all labels.
  ///
  /// \return Vector with labels of all binary nodes.
  const std::vector<std::string> get_all_labels() const;
  void get_all_labels_recursion(std::vector<std::string>& labels) const;

  /// Prints the tree in inorder.
  void print_tree_inorder() const;

  /// Prints the tree in postorder.
  void print_tree_postorder() const;

  /// Returns a vector that contains all nodes of a tree in postorder.
  void get_node_postorder_vector(std::vector<BinaryNode<Label>*>& postorder_vector);

  /// Returns a string that contains the labels of this node and its children.
  ///
  /// \return String containing the labels of the top twig.
  const std::string get_top_twig_labels() const;

  /// Verifies if this binary node is a leaf.
  ///
  /// \return True if this binary node is leaf, false otherwise.
  bool is_leaf() const;

  /// Set subgraph size.
  void set_subgraph_size(int subgraph_size);

  /// Get subgraph size.
  int get_subgraph_size();

  /// Set detached.
  void set_detached(int detached);

  /// Get detached.
  int get_detached();

  /// Set postorder id.
  void set_postorder_id(int postorder_id);

  /// Get postorder id.
  int get_postorder_id();

// Member variables
private:
  /// Left child of this node.
  std::unique_ptr<BinaryNode<Label>> left_child_ = std::unique_ptr<BinaryNode<Label>>(nullptr);
  /// Right child of this node.
  std::unique_ptr<BinaryNode<Label>> right_child_ = std::unique_ptr<BinaryNode<Label>>(nullptr);
  /// Data representing the label of this binary node. Only this 
  /// BinaryNode object owns it.
  Label label_;

  /// Needed for partition-based similarity join
  /// Subgraph size
  int subgraph_size_ = 0;
  /// Number of nodes that have been cut off in the subtree rooted at this node
  int detached_ = 0;
  /// Number of nodes that have been cut off in the subtree rooted at this node
  int postorder_id_ = 0;
};

// Implementation details
#include "binary_node_impl.h"

}
