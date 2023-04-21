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

/// \file join/tjoin/candidate_index.h
///
/// \details
/// Implements a candidate index that efficiently and effectively returns tree 
/// pairs that satisfy the structural filter. 

#pragma once

#include <cmath>
#include <climits>
#include <algorithm>
#include <vector>
#include <unordered_map>
#include <queue>
#include "../../node/node.h"
#include "inverted_list_element.h"
#include "set_data.h"
#include "label_set_element.h"

namespace candidate_index {

class CandidateIndex {
// Member functions.
public:
  /// Constructor.
  CandidateIndex();
  /// Returns a set of candidates that satisfy the structural filter. 
  ///
  /// \param sets_collection Collection of all sets.
  /// \param join_candidates Resultset that contains all join candidates.
  /// \param number_of_labels Number of all different labels in all sets. 
  ///        Needed to allocate the inverted list index.
  /// \param distance_threshold Given similarity threshold.
  void lookup(
      std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection,
      std::vector<std::pair<int, int>>& join_candidates,
      const int number_of_labels, 
      const double distance_threshold);
  /// Returns the resultset.
  ///
  /// \return Pairs in the resultset.
  std::vector<std::pair<int, int>> get_result_pairs();
  /// Returns the number of pairs in the resultset.
  ///
  /// \return The number of pairs in the resultset.
  long int get_number_of_result_pairs() const;
  /// Returns the number of precandidates.
  ///
  /// \return The number of precandidates.
  long int get_number_of_pre_candidates() const;
  /// Sets the number of precandidates.
  void set_number_of_pre_candidates(const long int pc);
  /// Returns the number of precandidates.
  ///
  /// \return The number of lookups in the inverted list.
  long int get_number_of_il_lookups() const;
// Member variables.
private:
  /// Number of precandidates.
  long int pre_candidates_;
  /// Number of lookups in the inverted list.
  long int il_lookups_;
// Member functions.
private:
  /// Given two sets of nodes, the structural mapping identifies the number of 
  /// tau-valid node pairs.
  ///
  /// \param sv_r All nodes with a specific label in set r.
  /// \param sv_s All nodes with a specific label in set s.
  /// \param distance_threshold Given distance threshold.
  /// \return number of tau-valid node pairs in sv_r and sv_s.
  int structural_mapping(
    label_set_converter::LabelSetElement& sv_r, 
    label_set_converter::LabelSetElement& sv_s,
    const double distance_threshold);
  /// Verify for a given candidate pair r and s if it passes the structural filter.
  ///
  /// \param r First set of the candidate pair.
  /// \param s Second set of the candidate pair.
  /// \param t Distance threshold.
  /// \param olap Overlap up to position pr and ps.
  /// \param pr Position after prefix or overlap of r.
  /// \param ps Position after prefix or overlap of s.
  /// \param distance_threshold Given similarity threshold.
  /// \return Returns true if r and s satisfy the structural filter.
  bool structural_filter(std::vector<label_set_converter::LabelSetElement>& r, 
      std::vector<label_set_converter::LabelSetElement>& s, 
      const double t, int olap, int pr, int ps, 
      int distance_threshold, int maxr, int maxs);
};

// Implementation details.
#include "candidate_index_impl.h"

}
