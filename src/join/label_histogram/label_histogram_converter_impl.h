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

/// \file join/label_histogram/label_histogram_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into a collection 
/// of label histograms. 

#pragma once

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::create_histogram(
    const std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection) {

  // for each tree in the tree collection
  for (const auto& tree: trees_collection) {
    // stores the number of nodes per label
    std::unordered_map<int, int> label_histogram;
    // stores the number of nodes per label
    int tree_size = 0;
    // traverse tree and store number of nodes per label
    create_label_histrogram(tree, label_histogram, tree_size);
    // add label histogram to collection
    histogram_collection.emplace_back(tree_size, label_histogram);
  }
}

template<typename Label>
void Converter<Label>::create_label_histrogram(
    const node::Node<Label>& tree_node, 
    std::unordered_map<int, int>& label_histogram, 
    int& tree_size) {

  // do recursively for all children
  for (const auto& child: tree_node.get_children()) {
    create_label_histrogram(child, label_histogram, tree_size);
  }

  Label key = tree_node.label();
  std::string label_str = tree_node.label().to_string();

  // lookup key in token_map
  typename std::unordered_map<Label, int, labelhash>::const_iterator 
                              label_in_map = label_id_map_.find(key);
  // if label not in map
  if(label_in_map == label_id_map_.end())
    // add label id to map
    label_id_map_[key] = label_id_++;

  // increase label count for current node
  ++label_histogram[label_id_map_[key]];
  // increase tree size
  ++tree_size;
}

template<typename Label>
int Converter<Label>::get_number_of_labels() const {
  return label_id_;
}
