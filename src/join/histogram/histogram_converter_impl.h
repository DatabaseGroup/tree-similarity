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

/// \file join/histogram/histogram_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into collections 
/// of label, leaf distance, and degree histograms. 

#pragma once

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::create_histogram(
    const std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& label_histogram_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& degree_histogram_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& leaf_distance_histogram_collection) {

  // for each tree in the tree collection
  for (const auto& tree: trees_collection) {
    // stores the number of nodes per label
    std::unordered_map<int, int> label_histogram;
    // stores the number of nodes per leaf distance
    std::unordered_map<int, int> leaf_dist_histogram;
    // stores the number of nodes per degree
    std::unordered_map<int, int> degree_histogram;
    // size of the current tree
    int tree_size = 0;
    // traverse tree and store number of nodes per label
    create_histrograms(tree, label_histogram, degree_histogram, leaf_dist_histogram, tree_size);
    // add label histogram to collection
    label_histogram_collection.emplace_back(tree_size, label_histogram);
    // add degree histogram to collection
    degree_histogram_collection.emplace_back(tree_size, degree_histogram);
    // add leaf distance histogram to collection
    leaf_distance_histogram_collection.emplace_back(tree_size, leaf_dist_histogram);
  }
}

template<typename Label>
int Converter<Label>::create_histrograms(
    const node::Node<Label>& tree_node, 
    std::unordered_map<int, int>& label_histogram, 
    std::unordered_map<int, int>& degree_histogram, 
    std::unordered_map<int, int>& leaf_dist_histogram, 
    int& tree_size) {

  // count number of children
  int number_of_children = 0;
  // the leaf distance is the minimum leaf distance of a nodes children + 1
  int max_child_leaf_dist = 0;

  // do recursively for all children
  for (const auto& child: tree_node.get_children()) {
    int child_dist = create_histrograms(child, label_histogram, degree_histogram, leaf_dist_histogram, tree_size);
    if(max_child_leaf_dist < child_dist)
      max_child_leaf_dist = child_dist;
    ++number_of_children;
  }

  // handle leaf distance histogram
  // the leaf distance is the minimum leaf distance of a nodes children + 1
  ++max_child_leaf_dist;
  // increase leaf distance count for current node
  ++leaf_dist_histogram[max_child_leaf_dist];

  // handle degree histogram - increase degree count for current node
  ++degree_histogram[number_of_children];

  // handle label histogram
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

  return max_child_leaf_dist;
}

template<typename Label>
int Converter<Label>::get_number_of_labels() const {
  return label_id_;
}
