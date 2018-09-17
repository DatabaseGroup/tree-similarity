// The MIT License (MIT)
// Copyright (c) 2018 Mateusz Pawlik
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

/// \file ted_ub/greedy_ub.h
///
/// \details
/// Implements greedy mapping of the matching labels with the assumption that
/// similar trees should match many labels and should have similar structure.
/// NOTE: Works with unit cost only.
/// TODO: Expand.

#ifndef TREE_SIMILARITY_TED_UB_GREEDY_UB_H
#define TREE_SIMILARITY_TED_UB_GREEDY_UB_H

#include <vector>
#include <list>
#include <unordered_map>
#include <memory>
#include "node.h"
#include "label_dictionary.h"
#include <iostream>

namespace ted_ub {

template <typename Label, typename CostModel>
class GreedyUB {
// Member struct.
public:
  /// Holds internal items of the algorithms that are tested for correctness.
  struct TestItems {
    const std::vector<std::vector<int>>& t1_label_il;
    const std::vector<int>& t1_post_to_pre;
    const std::vector<int>& t1_pre_to_post;
    const std::vector<int>& t1_parent;
    const std::vector<int>& t1_rl;
    const std::vector<int>& t1_depth;
    const std::vector<int>& t1_size;
    const label::LabelDictionary<Label>& dict;
  };
// Member functions.
public:
  /// Constructor. Creates the cost model based on the template.
  GreedyUB();
  /// Verifies if the greedy label upper bound between trees t1 and t1 is
  /// smaller or equal than the similarity_threshold value.
  ///
  /// NOTE: Here, only unit cost model is allowed. similarity_threshold is
  ///       converted to integer value using std::ceil and static cast.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param similarity_threshold Maximum value of the tree edit distance.
  /// \return Greedy label upper bound value if it smaller or equal than the
  ///         similarity_threshold, and std::numeric_limits<double>::infinity()
  ///         otherwise.
  double verify(const node::Node<Label>& t1, const node::Node<Label>& t2,
      double similarity_threshold);
  /// Computes the TED value of the greedy label upper bound between trees t1
  /// and t1 given a similarity_threshold value.
  ///
  /// NOTE: Here, only unit cost model is allowed.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Maximum value of the tree edit distance.
  /// \return TED value of the greedy label upper bound.
  double greedy_ub_ted(const node::Node<Label>& t1, const node::Node<Label>& t2,
      const int k);
  /// NOTE: Left to test against previous versions of the functions.
  double greedy_ub_ted_deprecated(const node::Node<Label>& t1, const node::Node<Label>& t2,
      const int k);
  /// Calculates the unit cost of passed mapping.
  ///
  /// NOTE: The greedy_ub algorithm assumes unit cost. However, this function
  ///       could work with any cost model.
  ///
  /// \param mapping A mapping.
  /// \result Cost of mapping using CostModel.
  double mapping_cost(const std::vector<std::pair<int, int>>& mapping) const;
  /// Produces a TED mapping greedily matching nodes with equal labels.
  /// Each node x is matched to node y, such that y has the same label as
  /// x, and the lower bound for (x,y) does not exceed the threshold k. 
  ///
  /// Moreover, each consecutive matched pair is ensured not to violate TED
  /// mapping conditions.
  ///
  /// It is a two-step approach. First a one-to-one mapping is produced
  /// greedily. Then, to_ted_mappingis used to revise the initial one-to-one
  /// to a TED mapping.
  ///
  /// NOTE: In the worst case, it may have quadratic complexity due to
  ///       evaluating lower bounds for many nodes. 
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Similarity threshold.
  /// \result A TED mapping where only nodes with equal labels are matched.
  std::vector<std::pair<int, int>> lb_mapping(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Deprecated. Kept for experiments. Uses hashmap-based inverted list for
  /// labels matching.
  std::vector<std::pair<int, int>> lb_mapping_deprecated(const node::Node<Label>& t1,
      const node::Node<Label>& t2, const int k);
  /// Computes a TED mapping where:
  /// - as many as possible nodes with equal labels are mapped,
  /// - as many nodes as possible are renamed,
  /// - the resulting mapping is ensured to be a TED mapping.
  ///
  /// NOTE: It has linear runtime complexity.
  ///
  /// NOTE: Internally, it executes lb_mapping_new and fill_gaps_in_mapping_no_left.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  /// \param k Similarity threshold.
  /// \result A TED mapping with possibly many node pairs mapped.
  std::vector<std::pair<int, int>> lb_mapping_fill_gaps(
      const node::Node<Label>& t1, const node::Node<Label>& t2, const int k);
  /// Deprecated. Kept for experiments. Uses old filling gaps that have errors
  /// and use number of mapped nodes to the left.
  /// TODO: When deleted, delete also class fields and update node indexing.
  std::vector<std::pair<int, int>> lb_mapping_fill_gaps_deprecated(
      const node::Node<Label>& t1, const node::Node<Label>& t2, const int k);
  /// Creates a TestItems object and returns it (by value).
  ///
  /// \return A TestItem object.
  const TestItems get_test_items() const;
  /// If the TED algorithm has been executed, returns the number of subproblems
  /// encountered during that execution.
  ///
  /// \return The number of subproblems acountered in the last TED computation.
  const unsigned long long int get_subproblem_count() const;
  /// For compatibility with Touzet only.
  const unsigned long long int get_top_y_update_count() const;
// Member variables.
private:
  /// The size of the source tree.
  int t1_input_size_;
  /// The size of the destination tree.
  int t2_input_size_;
  /// Stores pointers to nodes of the source tree. Indexed in postorder.
  // NOTE: We use reference_wrapper for const references to nodes. For now, we
  // decided not to use raw pointers. Smart pointers introduce ownership, but
  // this vector is only an observer from a logical point of view.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t1_node_;
  /// Stores pointers to nodes of the destination tree. Indexed in postorder.
  std::vector<std::reference_wrapper<const node::Node<Label>>> t2_node_;
  /// Stores preorder id of each node in the source tree. Indexed in postorder.
  std::vector<int> t1_post_to_pre_;
  /// Stores preorder id of each node in the source tree. Indexed in postorder.
  std::vector<int> t2_post_to_pre_;
  /// Stores postorder id of each node in the source tree. Indexed in preorder.
  std::vector<int> t1_pre_to_post_;
  /// Stores postorder id of each node in the destination tree. Indexed in preorder.
  std::vector<int> t2_pre_to_post_;
  /// Stores the postorder id of the parent for each node of the source
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_parent_;
  /// Stores the postorder id of the parent for each node of the destination
  /// tree. '-1' represents no parent.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_parent_;
  /// Stores for each node x of the source tree the postorder id of the
  /// first leaf node to the righ of x. '-1' represents no such node.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_rl_;
  /// Stores for each node x of the destination tree the postorder id of the
  /// first leaf node to the righ of x. '-1' represents no such node.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_rl_;
  /// Stores the depth for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_depth_;
  /// Stores the depth for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_depth_;
  /// Stores the subtree size for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_size_;
  /// Stores the subtree size for each node of the destination tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t2_size_;
  /// For each label in the source tree, stores postorder ids of nodes that
  /// carry it (the list is sorted in postorder).
  ///
  /// The inverted list is a vector because we use LabelDictionary to assign
  /// labels ids. Then, we know how many different labels are there. The size
  /// of this vector in the works case is the sum of input trees sizes.
  ///
  /// The postorder ids of nodes carrying a specific label are stored in a
  /// vector. We can do this because instead of removing the mapped node from
  /// a list, we mark it as mapped (using '-1').
  std::vector<std::vector<int>> t1_label_il_;
  /// See t1_label_il_.
  std::vector<std::vector<int>> t2_label_il_;
  /// For every label id, stores the starting position of traversing the
  /// corresponding vector in t2_label_il_. Due to 2*k+1 window depending on
  /// the postorder ids of two nodes that we try to match.
  std::vector<int> t2_label_il_start_pos_;
  /// Stores Label to int id translation of all labels in both input trees.
  label::LabelDictionary<Label> dict_;
  /// Stores the label id from dict_ for each node of the source tree.
  /// Indexed in postorder ids starting with 0.
  std::vector<int> t1_label_;
  /// See t1_label_.
  std::vector<int> t2_label_;
  /// Deprecated. Kept for experiments. Old inverted list based on a hash map.
  std::unordered_map<std::string, std::list<int>> t1_label_il_hash_;
  std::unordered_map<std::string, std::list<int>> t2_label_il_hash_;
  /// Cost model.
  const CostModel c_;
  /// Subproblem counter - for experiments only. Counts the number of
  /// non-trivial values filled in fd_ matrix: subproblems where both forests
  /// are not empty.
  unsigned long long int subproblem_counter;
  /// For compatibility with Touzet only.
  unsigned long long int top_y_update_counter = 0;
// Member functions.
private:
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
      const std::vector<std::pair<int, int>>& mapping) const;
  /// Deprecated. Kept for experiments. Incorrectly updates the number of
  /// mapped descendants.
  /// TODO: When deleted, delete also class fields and update node indexing.
  std::vector<std::pair<int, int>> to_ted_mapping_deprecated(
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
  /// Updates mapped descendants and nodes to the left counters when node is
  /// not mapped.
  void update_desc_and_left_when_not_mapped(const int node,
      std::vector<int>& count_mapped_desc, std::vector<int>& count_mapped_left,
      const std::vector<int>& parent, const std::vector<int>& rl,
      const std::vector<int>& size, const std::vector<int>& post_to_pre,
      const int input_size) const;
  /// Updates mapped descendants and nodes to the left counters when node is
  /// mapped.
  void update_desc_and_left_when_mapped(const int node,
      std::vector<int>& count_mapped_desc, std::vector<int>& count_mapped_left,
      const std::vector<int>& parent, const std::vector<int>& rl,
      const std::vector<int>& size, const std::vector<int>& post_to_pre,
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
  void get_mapped_ancestors_counts(std::vector<std::pair<int, int>>& mapping,
      std::vector<int>& t1_count_mapped_anc,
      std::vector<int>& t2_count_mapped_anc) const;
  /// Verifies if nodes i and j are in the corresponding regions with respect
  /// to the beginnings and ends of the gaps.
  /// Used in fill_gaps_in_mapping.
  bool if_in_corresponding_regions(int t1_begin_gap, int i, int t1_end_gap,
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
      std::vector<std::pair<int, int>>& mapping, const int k);
  /// Deprecated. Kept for experiments. 
  std::vector<std::pair<int, int>> fill_gaps_in_mapping_deprecated(
      std::vector<std::pair<int, int>>& mapping, const int k) const;
  /// Resets and initialises algorithm's internal data structures and constants.
  /// Has to be called before computing the distance.
  ///
  /// \param t1 Source tree.
  /// \param t2 Destination tree.
  void init(const node::Node<Label>& t1, const node::Node<Label>& t2);
  /// Indexes the nodes of an input tree. Wrapper for the recursive
  /// index_nodes_recursion.
  /// Call this method to index nodes.
  ///
  /// \param root The root node of the tree to index.
  /// \param label_il Inverted list of labels to node postorder ids.
  /// \param nodes Vector of postorder ids to references to nodes.
  /// \param post_to_pre Translation vector from postorder to preorder id.
  void index_nodes(const node::Node<Label>& root,
                   std::vector<std::vector<int>>& label_il,
                   std::unordered_map<std::string, std::list<int>>& label_il_hash,
                   std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                   std::vector<int>& post_to_pre,
                   std::vector<int>& pre_to_post,
                   std::vector<int>& parent,
                   std::vector<int>& depth,
                   std::vector<int>& size,
                   std::vector<int>& label);
  /// Traverses an input tree rooted at root recursively and collects
  /// information into index structures.
  ///
  /// \param root The root node of the tree to index.
  /// \param label_il Inverted list of labels to node postorder ids.
  /// \param nodes Vector of postorder ids to references to nodes.
  /// \param post_to_pre Translation vector from postorder to preorder id.
  /// \param start_postorder Stores the postorder id of a node during traversal.
  /// \param start_preorder Stores the preorder id of a node during traversal.
  int index_nodes_recursion(const node::Node<Label>& root,
                             std::vector<std::vector<int>>& label_il,
                             std::unordered_map<std::string, std::list<int>>& label_il_hash,
                             std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                             std::vector<int>& post_to_pre,
                             std::vector<int>& pre_to_post,
                             std::vector<int>& parent,
                             std::vector<int>& depth,
                             std::vector<int>& size,
                             std::vector<int>& label,
                             int& start_postorder, int& start_preorder,
                             unsigned int start_depth);
  /// Collects the first leaf node to the right of every node of input tree.
  ///
  /// \param input_size Size of the input tree.
  /// \param nodes Vector of postorder ids to references to nodes.
  /// \param rl Vector with first leaf nodes to the right.
  void post_traversal_indexing(const int input_size,
                               const std::vector<std::reference_wrapper<const node::Node<Label>>>& nodes,
                               std::vector<int>& rl);
  /// Verifies if subtrees T1_x and T2_y are k-relevant.
  ///
  /// T1_x and T2_y are k-relevant if
  /// |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k.
  ///
  /// NOTE: x and y are increased by one due to node indexing starting with 0.
  ///
  /// \param x postorder id of a node in source tree T1.
  /// \param y postorder id of a node in destination tree T2.
  /// \param k maximum number of structural canges.
  /// \return True if subtrees T1_x and T2_y are k-relevant, and false otherwise.
  bool k_relevant(const int x, const int y, const int k) const;
};

// Implementation details.
#include "greedy_ub_impl.h"

}

#endif // TREE_SIMILARITY_TED_UB_GREEDY_UB_H
