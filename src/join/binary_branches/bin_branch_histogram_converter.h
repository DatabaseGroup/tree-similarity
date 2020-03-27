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

/// \file join/binary_branches/bin_branch_histogram_converter.h
///
/// \details
/// Takes a collection of trees and and converts them into histograms of binary 
/// branches. The histogram stores the number of a certain binary branch. 


#pragma once

#include <unordered_map>
#include "node.h"
#include "string_label.h"

namespace bin_branch_histogram_converter {

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
  /// Returns the maximum degree of a node in a tree collection.
  ///
  /// \return The the maximum degree of a node in a tree collection.
  int get_number_of_bb() const;
// Member variables.
private:
  /// Counter to give unique IDs for each binary branch.
  int bb_id_ = 0;
  // Map a binary branch to a unique integer.
  typename std::unordered_map<std::string, int> bb_id_map_;
  /// String that is used as empty label. Therefore, no label should contain this string. 
  std::string empty_string_ = "!§$%&/()=?";
// Member functions.
private:
  /// Recursively transforms a tree into a histogram. Each element holds a value 
  /// of the histogram.
  ///
  /// \param tree_node Current node of a tree.
  /// \param degree_histogram Vector of histogram values.
  void create_bin_branch_histrogram(
    const node::Node<Label>& tree_node, 
    std::string& right_sibling_label, 
    std::unordered_map<int, int>& degree_histogram, 
    int& tree_size);
};

// Implementation details.
#include "bin_branch_histogram_converter_impl.h"
}
