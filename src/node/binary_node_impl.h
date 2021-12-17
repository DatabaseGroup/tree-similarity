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

/// \file node/binary_impl.h
///
/// \details
/// Contains the implementation of the BinaryNode class.

#pragma once

template<class Label>
BinaryNode<Label>::BinaryNode(ConstReference label) : label_(label) {}

template<class Label>
const typename BinaryNode<Label>::SizeType BinaryNode<Label>::children_count() const {
  int nr_of_children= 0;
  if(left_child_ != nullptr) ++nr_of_children;
  if(right_child_ != nullptr) ++nr_of_children;
  return nr_of_children;
}

template<class Label>
typename BinaryNode<Label>::ConstReference BinaryNode<Label>::label() const {
  return label_;
}

template<class Label>
BinaryNode<Label>* BinaryNode<Label>::add_left_child(std::unique_ptr<BinaryNode<Label>>& node) {
  left_child_ = std::move(node);
  return left_child_.get();
}

template<class Label>
BinaryNode<Label>* BinaryNode<Label>::add_right_child(std::unique_ptr<BinaryNode<Label>>& node) {
  right_child_ = std::move(node);
  return right_child_.get();
}

template<class Label>
bool BinaryNode<Label>::has_left_child() {
  return left_child_ != nullptr;
}

template<class Label>
bool BinaryNode<Label>::has_right_child() {
  return right_child_ != nullptr;
}

template<class Label>
BinaryNode<Label>* BinaryNode<Label>::get_left_child() {
  return left_child_.get();
}

template<class Label>
BinaryNode<Label>* BinaryNode<Label>::get_right_child() {
  return right_child_.get();
}

template<class Label>
int BinaryNode<Label>::get_tree_size() const {
  int size = 1;

  if(left_child_ != nullptr)
    size += left_child_->get_tree_size();
  if(right_child_ != nullptr)
    size += right_child_->get_tree_size();

  return size;
}

template<class Label>
const std::vector<std::string> BinaryNode<Label>::get_all_labels() const {
  std::vector<std::string> labels;

  get_all_labels_recursion(labels);
  return labels;
}

template<class Label>
void BinaryNode<Label>::get_all_labels_recursion(std::vector<std::string>& labels) const {
  labels.push_back(label_.to_string());
  if(left_child_ != nullptr)
    left_child_->get_all_labels_recursion(labels);
  if(right_child_ != nullptr)
    right_child_->get_all_labels_recursion(labels);
}

template<class Label>
void BinaryNode<Label>::print_tree_inorder() const {
  std::cout << label_.to_string();
  if(left_child_ != nullptr) {
    std::cout << "{";
    left_child_->print_tree_inorder();
    std::cout << "}";
  }
  if(right_child_ != nullptr) {
    std::cout << "{";
    right_child_->print_tree_inorder();
    std::cout << "}";
  }
}

template<class Label>
void BinaryNode<Label>::print_tree_postorder() const {
  if(left_child_ != nullptr) {
    std::cout << "{";
    left_child_->print_tree_postorder();
    std::cout << "}";
  }
  if(right_child_ != nullptr) {
    std::cout << "{";
    right_child_->print_tree_postorder();
    std::cout << "}";
  }
  std::cout << label_.to_string();
}

template<class Label>
void BinaryNode<Label>::get_node_postorder_vector(std::vector<BinaryNode<Label>*>& postorder_vector) {
  if(left_child_ != nullptr)
    left_child_->get_node_postorder_vector(postorder_vector);
  if(right_child_ != nullptr)
    right_child_->get_node_postorder_vector(postorder_vector);

  postorder_vector.push_back(this);
}

template<class Label>
const std::string BinaryNode<Label>::get_top_twig_labels() const {
  // get string that concatenates the labels of the current node and its children
  std::string twig_labels = label_.to_string();
  if(left_child_ != nullptr)
    twig_labels.append(left_child_->label().to_string());
  if(right_child_ != nullptr)
    twig_labels.append(right_child_->label().to_string());

  return twig_labels;
}

template<class Label>
bool BinaryNode<Label>::is_leaf() const {
  return (left_child_ != nullptr && right_child_ != nullptr);
}

template<class Label>
void BinaryNode<Label>::set_subgraph_size(int subgraph_size) {
  subgraph_size_ = subgraph_size;
}

template<class Label>
int BinaryNode<Label>::get_subgraph_size() {
  return subgraph_size_;
}

template<class Label>
void BinaryNode<Label>::set_detached(int detached) {
  detached_ = detached;
}

template<class Label>
int BinaryNode<Label>::get_detached() {
  return detached_;
}

template<class Label>
void BinaryNode<Label>::set_postorder_id(int postorder_id) {
  postorder_id_ = postorder_id;
}

template<class Label>
int BinaryNode<Label>::get_postorder_id() {
  return postorder_id_;
}
