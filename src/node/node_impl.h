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

/// \file node/node_impl.h
///
/// \details
/// Contains the implementation of the Node class.

#pragma once

template<class Label>
Node<Label>::Node(ConstReference label) : label_(label) {}

template<class Label>
const typename Node<Label>::SizeType Node<Label>::children_count() const {
  return children_.size();
}

template<class Label>
typename Node<Label>::ConstReference Node<Label>::label() const {
  return label_;
}

template<class Label>
Node<Label>& Node<Label>::add_child(Node<Label> child) {
  children_.push_back(child);
  return children_.back();
}

template<class Label>
const std::vector<Node<Label>>& Node<Label>::get_children() const {
  return children_;
}

template<class Label>
int Node<Label>::get_tree_size() const {
  int size = 1;
  for (const auto& child : children_) {
    size += child.get_tree_size();
  }
  return size;
}

template<class Label>
const std::vector<std::string> Node<Label>::get_all_labels() const {
  std::vector<std::string> labels;
  get_all_labels_recursion(labels);
  return labels;
}
template<class Label>
void Node<Label>::get_all_labels_recursion(std::vector<std::string>& labels) const {
  labels.push_back(label_.to_string());
  for (const auto& child : children_) {
    child.get_all_labels_recursion(labels);
  }
}

template<class Label>
bool Node<Label>::is_leaf() const {
  return children_.size() == 0;
}
