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

/// \file join/degree_histogram/degree_histogram_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into a collection 
/// of degree histograms. 

#pragma once

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::create_histogram(
    const std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection) {

  // for each tree in the tree collection
  for (const auto& tree: trees_collection) {
    // stores the number of nodes per degree
    std::unordered_map<int, int> degree_histogram;
    // stores the number of nodes per degree
    int tree_size = 0;
    // traverse tree and store number of nodes per degree
    create_degree_histrogram(tree, degree_histogram, tree_size);
    // add degree histogram to collection
    histogram_collection.emplace_back(tree_size, degree_histogram);
  }
}

template<typename Label>
void Converter<Label>::create_degree_histrogram(
    const node::Node<Label>& tree_node, 
    std::unordered_map<int, int>& degree_histogram, 
    int& tree_size) {

  // count number of children
  int number_of_children = 0;
  // do recursively for all children
  for (const auto& child: tree_node.get_children()) {
    create_degree_histrogram(child, degree_histogram, tree_size);
    ++number_of_children;
  }
  // increase degree count for current node
  ++degree_histogram[number_of_children];
  // store maximum degree of the collection
  if(number_of_children > max_degree_)
    max_degree_ = number_of_children;
  // increase tree size
  ++tree_size;
}

template<typename Label>
int Converter<Label>::get_maximum_degree() const {
  return max_degree_;
}
