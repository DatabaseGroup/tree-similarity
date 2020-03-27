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

/// \file join/leaf_dist_histogram/leaf_dist_histogram_converter.h
///
/// \details
/// Takes a collection of trees and and converts them into a histogram of leaf 
/// distance values. The histogram stores the number of nodes with a certain 
/// leaf distance value. 

#pragma once

#include <unordered_map>
#include "node.h"
#include "string_label.h"

namespace leaf_dist_histogram_converter {

template <typename Label>
class Converter {
// Member functions.
public:
  /// Constructor.
  Converter();
  /// Converts a given tree to a histogram. Each element of the vector contains 
  /// a value of the histogram.
  ///
  /// \param tree_collection A collection of trees.
  /// \param histogram_collection A collection of histograms.
  void create_histogram(
    const std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& histogram_collection);
  /// Returns the maximum leaf distance of a node in a tree collection.
  ///
  /// \return The the maximum leaf distance of a node in a tree collection.
  int get_maximum_leaf_dist() const;
// Member variables.
private:
  /// Maximum leaf distance of a node of all trees.
  int max_leaf_distance_ = 0;
// Member functions.
private:
  /// Recursively transforms a tree into a histogram. Each element holds a value 
  /// of the histogram.
  ///
  /// \param tree_node Current node of a tree.
  /// \param leaf_dist_histogram Vector of histogram values.
  int create_leaf_dist_histrogram(
    const node::Node<Label>& tree_node, 
    std::unordered_map<int, int>& leaf_dist_histogram, int& tree_size);
};

// Implementation details.
#include "leaf_dist_histogram_converter_impl.h"
}
