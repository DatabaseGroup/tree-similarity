// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter
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

/// \file lookup/index/two_stage_inv_list.h
///
/// \details
/// Implements a two-stage candidate index. Level 1 is the label of a node, 
/// level 2 is the position in the global token frequency order for a certain 
/// tree.

#pragma once

#include <cmath>
#include <climits>
#include <algorithm>
#include <vector>
#include <map>
#include <unordered_set>
#include <queue>
#include "../../node/node.h"
#include "inverted_list_element.h"
#include "label_set_element.h"

namespace lookup {

class TwoStageInvertedList {
// Member functions.
public:
  /// Constructor.
  TwoStageInvertedList(long int labels);
  /// Builds an inverted list index on the labels and the positions of a given 
  /// label set collection. 
  ///
  /// \param sets_collection Label set collection.
  /// \param join_candidates Resultset that contains all join candidates.
  /// \param distance_threshold Given similarity threshold.
  void build(std::vector<std::pair<int, 
      std::vector<label_set_converter_index::LabelSetElement>>>& sets_collection);
  /// Returns a set of candidates (set IDs) that have a common element in the 
  /// prefix. 
  ///
  /// \param label_id ID of the current node.
  /// \param postorder_id Postorder ID of the current node.
  /// \param distance_threshold Given similarity threshold.
  void lookup(long int& q_label_id, long int descendants, long int ancestors, 
    int& q_tree_size, std::unordered_set<long int>& candidates, 
    const double distance_threshold);
  long int get_number_of_pre_candidates() const;
  /// Sets the number of precandidates.
  void set_number_of_pre_candidates(const long int pc);
  /// Returns the number of precandidates.
  ///
  /// \return The number of lookups in the inverted list.
  long int get_number_of_il_lookups() const;
  /// Sets the number of index lookups.
  void set_number_of_il_lookups(const long int il);
// Member variables.
private:
  /// Number of precandidates.
  long int pre_candidates_;
  /// Number of lookups in the inverted list.
  long int il_lookups_;
  /// Number of different labels.
  long int nr_of_labels_;
  /// Inverted list index.
  std::vector<InvertedListElement> il_index_;
};

// Implementation details.
#include "two_stage_inverted_list_impl.h"

}
