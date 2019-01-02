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

/// \file join/leaf_dist_histogram/leaf_dist_histogram_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into a collection 
/// of histograms. 

#ifndef TREE_SIMILARITY_JOIN_DEGREE_HISTOGRAM_DEGREE_HISTOGRAM_CONVERTER_IMPL_H
#define TREE_SIMILARITY_JOIN_DEGREE_HISTOGRAM_DEGREE_HISTOGRAM_CONVERTER_IMPL_H

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::create_histogram(
    const std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, std::unordered_map<unsigned int, unsigned int>>>& histogram_collection) {

  // for each tree in the tree collection
  for (const auto& tree: trees_collection) {
    // stores the number of nodes per leaf distance
    std::unordered_map<unsigned int, unsigned int> leaf_dist_histogram;
    // stores the number of nodes per leaf distance
    unsigned int tree_size = 0;
    // traverse tree and store number of nodes per leaf distance
    create_leaf_dist_histrogram(tree, leaf_dist_histogram, tree_size);
    // add leaf distance histogram to collection
    histogram_collection.emplace_back(tree_size, leaf_dist_histogram);
  }
}

template<typename Label>
int Converter<Label>::create_leaf_dist_histrogram(
    const node::Node<Label>& tree_node, 
    std::unordered_map<unsigned int, unsigned int>& leaf_dist_histogram, 
    unsigned int& tree_size) {

  // the leaf distance is the minimum leaf distance of a nodes children + 1
  unsigned int min_child_leaf_dist = -1;
  // do recursively for all children
  for (const auto& child: tree_node.get_children()) {
    int child_dist = create_leaf_dist_histrogram(child, leaf_dist_histogram, tree_size);
    if(min_child_leaf_dist > child_dist || min_child_leaf_dist == -1)
      min_child_leaf_dist = child_dist;
  }
  // the leaf distance is the minimum leaf distance of a nodes children + 1
  ++min_child_leaf_dist;
  // increase leaf distance count for current node
  ++leaf_dist_histogram[min_child_leaf_dist];
  // store maximum leaf distance of the collection
  if(min_child_leaf_dist > max_leaf_distance_)
    max_leaf_distance_ = min_child_leaf_dist;
  // increase tree size
  ++tree_size;

  return min_child_leaf_dist;
}

template<typename Label>
const unsigned int Converter<Label>::get_maximum_leaf_dist() const {
  return max_leaf_distance_;
}

#endif // TREE_SIMILARITY_JOIN_DEGREE_HISTOGRAM_DEGREE_HISTOGRAM_CONVERTER_IMPL_H