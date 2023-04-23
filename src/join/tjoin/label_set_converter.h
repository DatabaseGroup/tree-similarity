// The MIT License (MIT)
// Copyright (c) 2017 Thomas Huetter
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

/// \file join/tjoin/label_set_converter.h
///
/// \details
/// Takes a collection of trees and and converts them into a collection of label 
/// sets. A label set consists of labels and each label holds all nodes with that 
/// label. The labels are substituted with their inverted label frequency number. 
/// The labels in the sets are sorted by the global inverted frequency ordering 
/// of the input collection. 

#pragma once

#include <vector>
#include <unordered_map>
#include "../../node/node.h"
#include "../../label/string_label.h"
#include "label_set_element.h"

namespace label_set_converter {

template <typename Label>
class Converter {
// Member struct.
public:
  /// Comparator for pair of set elements by their id value.
  struct LabelSetElementcomparator {
    bool operator()(const label_set_converter::LabelSetElement& a, 
                    const label_set_converter::LabelSetElement& b) const { 
      return a.id < b.id; 
    }
  } LabelSetElementComparator;
  /// Comparator for pair of integers according to their first value.
  struct paircomparator {
    bool operator()(const std::pair<int, int>& a, 
                    const std::pair<int, int>& b) const { 
      return a.first < b.first; 
    }
  } pairComparator;
  /// Comparator for vectors of integers according to their size.
  struct vectorcomparator {
    bool operator()(const std::vector<int>& a, 
                    const std::vector<int>& b) const { 
      return a.size() < b.size(); 
    }
  } vectorComparator;
  /// Equality operator for tokencount objects.
  struct eqLabelSetElement {
    bool operator()(const label_set_converter::LabelSetElement& a, 
                    const label_set_converter::LabelSetElement& b) const { 
      return a.id == b.id; 
    }
  };
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
  /// Converts a given tree to a set. A unique integer is assigned for each 
  /// label according to its frequency.
  ///
  /// \param tree_collection A collection of trees.
  /// \param sets_collection A collection of sets.
  void assignFrequencyIdentifiers(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection);
  /// Returns the number of different labels in the given tree collection.
  ///
  /// \return The number of different labels in the given tree collection.
  int get_number_of_labels() const;
// Member variables.
private:
  /// Counter to give unique IDs to the tokens.
  int next_token_id_ = 0;
  /// Counter to give unique IDs to the tokens.
  int actual_depth_ = 0;
  /// Counter to give unique IDs to the tokens.
  int actual_pre_order_number_ = 0;
// Member functions.
private:
  /// Recursively transforms a tree into a label set. Each label is an integers.
  ///
  /// \param tree_node Node of a tree.
  /// \param tree_size Size of the tree rooted at tree_node.
  /// \param token_map {(token, tokcnt) -> id}.
  /// \param record Set of tokens.
  /// \return subtree size of the actual node
  int create_record(const node::Node<Label>& tree_node, int& postorder_id, int tree_size,
    std::unordered_map<Label, int, labelhash>& token_map,
    std::unordered_map<int, label_set_converter::LabelSetElement>& record_labels);
};

// Implementation details.
#include "label_set_converter_impl.h"

}
