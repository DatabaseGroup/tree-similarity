// The MIT License (MIT)
// Copyright (c) 2018, 2019 Mateusz Pawlik
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

#pragma once

#include <vector>
#include <list>
#include <unordered_map>
#include <cmath>
#include "../node/node.h"
#include "../label/label_dictionary.h"
#include "../node/tree_indexer.h"
#include "../ted/ted_algorithm.h"

namespace ted_ub {

/**
 * Implements label guided mapping (LGM) algorithm.
 *
 * NOTE: Works with unit cost only.
 *
 * NOTE: only node::TreeIndexLGM or its superset can be used with LGMTreeIndex.
 *
 * TODO: Expand the description.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexLGM>
class LGMTreeIndex : public ted::TEDAlgorithm<CostModel, TreeIndex> {

  // Base class members made visible for this class.
  using ted::TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::ted;
  using ted::TEDAlgorithm<CostModel, TreeIndex>::c_;

public:
  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  double ted(const TreeIndex& t1, const TreeIndex& t2);
  /// Computes the TED value of the LGM upper bound with similarity_threshold value.
  /**
   * If size lower bound is greater than threshold, infinity is returned.
   *
   * NOTE: Here, only unit cost model is allowed.
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param k Maximum value of the tree edit distance.
   * \return TED value of the LGM upper bound.
   */
  double ted_k(const TreeIndex& t1, const TreeIndex& t2, const int k);
  /// Calculates the unit cost of passed mapping.
  /**
   * NOTE: The greedy_ub algorithm assumes unit cost. However, this function
   *       could work with any cost model.
   *
   * \param mapping A mapping.
   * \result Cost of mapping using CostModel.
   */
  double mapping_cost(const TreeIndex& t1, const TreeIndex& t2,
      const std::vector<std::pair<int, int>>& mapping) const;
  /// Produces a TED mapping greedily matching nodes with equal labels.
  /**
   * Each node x is matched to node y, such that y has the same label as
   * x, and the lower bound for (x,y) does not exceed the threshold k.
   *
   * Moreover, each consecutive matched pair is ensured not to violate TED
   * mapping conditions.
   *
   * It is a two-step approach. First a one-to-one mapping is produced
   * greedily. Then, to_ted_mapping is used to revise the initial one-to-one
   * to a TED mapping.
   *
   * NOTE: In the worst case, it may have quadratic complexity due to high k,
   *       and evaluating lower bounds for many nodes. 
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param k Maximum value of the tree edit distance.
   * \result A TED mapping where only nodes with equal labels are matched.
   */
  std::vector<std::pair<int, int>> lb_mapping(const TreeIndex& t1,
      const TreeIndex& t2, const int k);
  /// Fills gaps in a TED mappind with additional node pairs.
  /**
   * Computes a TED mapping where:
   * - as many as possible nodes with equal labels are mapped (lb_mapping),
   * - as many nodes as possible are renamed,
   * - the resulting mapping is ensured to be a TED mapping.
   *
   * NOTE: It has linear runtime complexity.
   *
   * NOTE: Internally, it executes lb_mapping_new and fill_gaps_in_mapping_no_left.
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param k Maximum value of the tree edit distance.
   * \result A TED mapping with possibly many node pairs mapped.
   */
  std::vector<std::pair<int, int>> lb_mapping_fill_gaps(const TreeIndex& t1,
      const TreeIndex& t2, const int k);
  /// Initializes the internals of the algorithm for testing intermediate steps.
  /**
   * When testing intermediate steps of the algorithm, `t2_label_il_` and
   * `t2_label_il_start_pos_` have to be initialized for the input trees.
   * This function is also called in `ted_k`.
   *
   * \param t2 TreeIndex of destination tree.
   */
  void init(const TreeIndex& t2);

private:
  /// For every label id, stores the starting position of traversing the
  /// corresponding vector in t2_label_il_. Due to 2*k+1 window depending on
  /// the postorder ids of two nodes that we try to match.
  std::unordered_map<int, int> t2_label_il_start_pos_;
  /// Local copy of label inverted list.
  /// We need it because otherwise we can't modify it.
  std::unordered_map<int, std::vector<int>> t2_label_il_;
  /// Takes a one-to-one mapping and revises it such that the output is a valid
  /// TED mapping.
  /// The solution is based on (a) increasing postorder ids of the nodes in
  /// consecutive mapped pairs, and (b) counting the number of mapped
  /// descendants and nodes to the left so far.
  ///
  /// NOTE: The complexity is supposed to be linear. Further analysis is
  ///       necessary.
  ///
  /// \param mapping A one-to-one mapping.
  /// \return A revised mapping that is a valid TED mapping.
  std::vector<std::pair<int, int>> to_ted_mapping(
      const TreeIndex& t1, const TreeIndex& t2,
      const std::vector<std::pair<int, int>>& mapping) const;
  /// Updates mapped descendants counters when node is not mapped.
  /// NOTE: Use only when node ids are ordered.
  void update_desc_when_not_mapped(const int node,
      std::vector<int>& count_mapped_desc, const std::vector<int>& parent,
      const int input_size) const;
  /// Updates mapped descendants counters when node is mapped.
  /// NOTE: Use only when node ids are ordered.
  void update_desc_when_mapped(const int node,
      std::vector<int>& count_mapped_desc, const std::vector<int>& parent,
      const int input_size) const;
  /// Updates mapped descendants counters when node is not mapped.
  /// NOTE: Can be uses when node ids are not ordered.
  ///       Fixes an update bug.
  void update_prop_desc_when_not_mapped(const int node,
      std::vector<int>& count_mapped_desc,
      std::vector<int>& propagate_mapped_desc_count,
      const std::vector<int>& parent,
      const int input_size) const;
  /// Updates mapped descendants counters when node is mapped.
  /// NOTE: Can be uses when node ids are not ordered.
  ///       Fixes an update bug.
  void update_prop_desc_when_mapped(const int node,
      std::vector<int>& count_mapped_desc,
      std::vector<int>& propagate_mapped_desc_count,
      const std::vector<int>& parent,
      const int input_size) const;
  /// Reads a TED mapping and counts for each node the number of its mapped
  /// ancestors. Stores the values in the passed vectors.
  ///
  /// NOTE: Requires mapping to be TED mapping sorted on postorder ids.
  /// NOTE: Requires linear time in both tree size.
  ///
  /// TODO: Maybe there is a better way of doing it.
  ///
  /// \param mapping
  /// \param t1_count_mapped_anc
  /// \param t2_count_mapped_anc
  void get_mapped_ancestors_counts(const TreeIndex& t1, const TreeIndex& t2,
      std::vector<std::pair<int, int>>& mapping,
      std::vector<int>& t1_count_mapped_anc,
      std::vector<int>& t2_count_mapped_anc) const;
  /// Verifies if nodes i and j are in the corresponding regions with respect
  /// to the beginnings and ends of the gaps.
  /// Used in fill_gaps_in_mapping.
  bool if_in_corresponding_regions(const TreeIndex& t1, const TreeIndex& t2,
      int t1_begin_gap, int i, int t1_end_gap,
      int t2_begin_gap, int j, int t2_end_gap) const;
  /// Given a TED mapping (in our scenario, nodes with equal labels are mapped),
  /// fills in the gaps in mappings if possible.
  /// Only nodes that satisfy the similarity threshold k are mapped.
  /// The labels are disregarded here. The goal is to add possibly many pairs.
  /// The resulting mapping is a TED mapping.
  ///
  /// \param mapping A TED mapping with possibly some gaps to fill in.
  /// \param k Similarity threshold.
  /// \result A TED mapping, with possibly more pairs than in mapping. 
  std::vector<std::pair<int, int>> fill_gaps_in_mapping(
      const TreeIndex& t1, const TreeIndex& t2,
      std::vector<std::pair<int, int>>& mapping, const int k);
  /// Verifies if subtrees T1_x and T2_y are k-relevant.
  /**
   * T1_x and T2_y are k-relevant if
   * |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k.
   *
   * NOTE: x and y are increased by one due to node indexing starting with 0.
   *
   * \param x postorder id of a node in source tree T1.
   * \param y postorder id of a node in destination tree T2.
   * \param k maximum number of structural canges.
   * \return True if subtrees T1_x and T2_y are k-relevant, and false otherwise.
   */
  bool k_relevant(const TreeIndex& t1, const TreeIndex& t2, const int x,
      const int y, const int k) const;
};

// Implementation details.
#include "lgm_tree_index_impl.h"

}
