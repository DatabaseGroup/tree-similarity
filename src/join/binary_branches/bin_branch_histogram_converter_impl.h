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

/// \file join/binary_branches/bin_branch_histogram_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into a collection 
/// of binary branch histograms. 

#pragma once

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::create_histogram(
    const std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection) {

  // for each tree in the tree collection
  for (const auto& tree: trees_collection) {
    // stores the number of nodes per binary branch
    std::unordered_map<int, int> bb_histogram;
    // stores the number of nodes per binary branch
    int tree_size = 0;
    // traverse tree and store number of nodes per binary branch
    create_bin_branch_histrogram(tree, empty_string_, bb_histogram, tree_size);
    // add binary branch histogram to collection
    histogram_collection.emplace_back(tree_size, bb_histogram);
  }
}

template<typename Label>
void Converter<Label>::create_bin_branch_histrogram(
    const node::Node<Label>& tree_node, 
    std::string& right_sibling_label, 
    std::unordered_map<int, int>& bb_histogram, 
    int& tree_size) {

  auto& children = tree_node.get_children();
  int nr_of_children = children.size();
  std::string bb_string = tree_node.label().to_string();

  // append left most child label
  if(nr_of_children > 0)
    bb_string += children[0].label().to_string();
  else
    bb_string += empty_string_;
  // append right sibling label
  bb_string += right_sibling_label;

  // lookup binary branch in bb_map
  typename std::unordered_map<std::string, int>::const_iterator 
                              bb_in_map = bb_id_map_.find(bb_string);
  // if binary branch not in map
  if(bb_in_map == bb_id_map_.end())
    // add binary branch id to map
    bb_id_map_[bb_string] = bb_id_++;

  // increase binary branch count for current node
  ++bb_histogram[bb_id_map_[bb_string]];

  // do recursively for all children
  for (int i = 0; i < nr_of_children; i++) {
    std::string rs_string = empty_string_;
    if(i < nr_of_children - 1)
      rs_string = children[i+1].label().to_string();
    create_bin_branch_histrogram(children[i], rs_string, bb_histogram, tree_size);
  }
  // increase tree size
  ++tree_size;
}

template<typename Label>
int Converter<Label>::get_number_of_bb() const {
  return bb_id_;
}
