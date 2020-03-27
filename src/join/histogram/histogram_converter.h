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

/// \file join/histogram/histogram_converter.h
///
/// \details
/// Takes a collection of trees and and converts them into a histogram of label, 
/// leaf distance, and degree values. 

#pragma once

#include <unordered_map>
#include "node.h"
#include "string_label.h"

namespace histogram_converter {

template <typename Label>
class Converter {
// Member struct.
public:
  /// Hashfunction for labels.
  struct labelhash {
    size_t operator()(const Label& l1) const {
      return std::hash<std::string>()(l1.to_string());
    }
  };
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
    std::vector<std::pair<int, std::unordered_map<int, int>>>& label_histogram_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& degree_histogram_collection,
    std::vector<std::pair<int, std::unordered_map<int, int>>>& leaf_distance_histogram_collection);
  /// Returns the number of labels of a node in a tree collection.
  ///
  /// \return The the number of labels of a node in a tree collection.
  int get_number_of_labels() const;
// Member variables.
private:
  /// Counter to give unique IDs to labels.
  int label_id_ = 0;
  // Map a label to a unique integer.
  typename std::unordered_map<Label, int, labelhash> label_id_map_;
// Member functions.
private:
  /// Recursively transforms a tree into three histograms. Each element holds a value 
  /// of the histogram.
  ///
  /// \param tree_node Current node of a tree.
  /// \param label_histogram Vector of histogram values.
  int create_histrograms(
    const node::Node<Label>& tree_node, 
    std::unordered_map<int, int>& label_histogram, 
    std::unordered_map<int, int>& degree_histogram, 
    std::unordered_map<int, int>& leaf_dist_histogram, 
    int& tree_size);
};

// Implementation details.
#include "histogram_converter_impl.h"
}
