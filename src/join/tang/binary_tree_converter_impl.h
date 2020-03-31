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

/// \file join/tang/binary_tree_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into a collection 
/// of binary trees.

#pragma once

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::convert(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<node::BinaryNode<Label>>& binary_trees_collection) {
  // for each tree in the tree collection
  for(auto tree: trees_collection) {
    // create root in binary tree
    node::BinaryNode<Label> binary_tree(tree.label());
    // convert tree to a binary tree recursively
    create_binary_tree(tree, binary_tree);
    // add binary tree to collection of binary tree
    binary_trees_collection.push_back(std::move(binary_tree));
  }
}

template<typename Label>
void Converter<Label>::create_binary_tree(
    const node::Node<Label>& tree_node,
    node::BinaryNode<Label>& binary_tree_node) {
  // marks position where the next node needs to be inserted
  node::BinaryNode<Label> *current_binary_node = &binary_tree_node;
  // used to distinguish the first child (i==0) from the other siblings
  bool first_child = true;

  // handle all children
  for(const auto& child: tree_node.get_children()) {
    // create a copy of the node in the original tree and add it as a child in the binary tree
    auto binary_child = std::make_unique<node::BinaryNode<Label>>(child.label());
    
    node::BinaryNode<Label>* binary_child_new_pointer;
    // only the first child is added as a left child, all others are added as right children
    if(first_child)
      binary_child_new_pointer = current_binary_node->add_left_child(binary_child);
    else
      binary_child_new_pointer = current_binary_node->add_right_child(binary_child);
    // recursive call with child in the original tree and the child in the binary tree
    create_binary_tree(child, *binary_child_new_pointer);

    // update current node in the binary tree, where the next node is inserted
    if(first_child) // only the first child is added as a left child
      current_binary_node = current_binary_node->get_left_child();
    else // all others are added as right children
      current_binary_node = current_binary_node->get_right_child();

    first_child = false;
  }
}
