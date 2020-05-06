// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
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

/// Contains the tools to index nodes of a tree into vectors.

#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <type_traits>
#include "label_dictionary.h"
#include "unit_cost_model.h"

namespace node {

/// Stores the size of the indexed tree.
class Constants {
  public: int tree_size_;
};

/// Stores label id of each node in a tree.
/**
 * Labels are inserted into a dictionary in their left-to-right preorder
 * appearance.
 * Indexed in left-to-right preorder.
 */
class PreLToLabelId {
  public: std::vector<int> prel_to_label_id_;
};

/// Stores label id of each node in a tree.
/**
 * Labels are inserted into a dictionary in their left-to-right preorder
 * appearance.
 * Indexed in left-to-right postorder.
 */
class PostLToLabelId {
  public: std::vector<int> postl_to_label_id_;
};

/// Stores label id of each node in a tree.
/**
 * Labels are inserted into a dictionary in their left-to-right preorder
 * appearance.
 * Indexed in right-to-left postorder.
 */
class PostRToLabelId {
  public: std::vector<int> postr_to_label_id_;
};

/// Stores subtree size of each node in a tree.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToSize {
  public: std::vector<int> postl_to_size_;
};

/// Stores subtree size of each node in a tree.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToSize {
  public: std::vector<int> prel_to_size_;
};

/// Stores left-to-right postorder id of the parent node.
/**
 * Indexed in left-to-right postorder.
 * `-1` represents no parent.
 */
class PostLToParent {
  public: std::vector<int> postl_to_parent_;
};

/// Stores left-to-right preorder id of the parent node.
/**
 * Indexed in left-to-right preorder.
 * `-1` represents no parent.
 */
class PreLToParent {
  public: std::vector<int> prel_to_parent_;
};

/// Stores left-to-right postorder id of the leftmost leaf descendant of a node.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToLLD {
  public: std::vector<int> postl_to_lld_;
};

/// Stores left-to-right preorder id of the leftmost leaf descendant of a node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToLLD {
  public: std::vector<int> prel_to_lld_;
};

/// Stores left-to-right preorder id of the rightmost leaf descendant of a node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToRLD {
  public: std::vector<int> prel_to_rld_;
};

/// Stores right-to-left postorder id of the rightmost leaf descendant of a node.
/**
 * Indexed in right-to-left postorder.
 * Depends on: PreLToSize, PostRToPreL, PreLToPostR, PreLToChildren.
 */
class PostRToRLD {
  public: std::vector<int> postr_to_rld_;
};

/// Stores left-to-right preorder id of each node.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToPreL {
  public: std::vector<int> postl_to_prel_;
};

/// Stores left-to-right postorder id of each node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToPostL {
  public: std::vector<int> prel_to_postl_;
};

/// Stores right-to-left preorder id of each node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToPreR {
  public: std::vector<int> prel_to_prer_;
};

/// Stores left-to-right preorder id of each node.
/**
 * Indexed in right-to-left preorder.
 */
class PreRToPreL {
  public: std::vector<int> prer_to_prel_;
};

/// Stores right-to-left postorder id of each node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToPostR {
  public: std::vector<int> prel_to_postr_;
};

/// Stores left-to-right preorder id of each node.
/**
 * Indexed in right-to-left postorder.
 */
class PostRToPreL {
  public: std::vector<int> postr_to_prel_;
};

/// Stores left-to-right postorder ids of each node's children.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToChildren {
  public: std::vector<std::vector<int>> postl_to_children_;
};

/// Stores left-to-right preorder ids of each node's children.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToChildren {
  public: std::vector<std::vector<int>> prel_to_children_;
};

/// Stores depth of every node in the tree.
/**
 * Indexed in left-to-right postorder.
 * Depth of the root node is 0.
 */
class PostLToDepth {
  public: std::vector<int> postl_to_depth_;
};

/// Stores left-to-right postorder id of the leftmost (first) child for each node.
/**
 * Indexed in left-to-right postorder.
 * '-1' represents no child.
 */
class PostLToLCh {
  public: std::vector<int> postl_to_lch_;
};

/// Stores the maximum depth for each subtree.
/**
 * Indexed in left-to-right postorder.
 * Depth of the root node is 0.
 */
class PostLToSubtreeMaxDepth {
  public: std::vector<int> postl_to_subtree_max_depth_;
};

/// Stores the left-to-right postorder id of the nearest keyroot ancestor.
/**
 * Indexed in left-to-right postorder.
 * The nearest keyroot ancestor of a keyroot node n is n.
 * Depends on: PostLToLCh, ListKR.
 */
class PostLToKRAncestor {
  public: std::vector<int> postl_to_kr_ancestor_;
};

// Stores preorder id of the first leaf node to the left/right.
/**
 * prel_to_ln_: left-to-right preorder of the first leaf to the left.
 * prer_to_ln_: right-to-left preorder of the first leaf to the right.
 * `-1` represents no such node.
 * Depends on: PreLToSize, PreRToPreL.
 */
class PreToLn {
  public: std::vector<int> prel_to_ln_;
  public: std::vector<int> prer_to_ln_;
};

// Stores true if a node is leftmost child of its parent.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToTypeLeft {
  public: std::vector<bool> prel_to_type_left_;
};

// Stores true if a node is rightmost child of its parent.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToTypeRight {
  public: std::vector<bool> prel_to_type_right_;
};

// Stores cost of a single-path function for each node [1, Section 5.2].
/**
 * prel_to_cost_all_: spf_A - single-path function using inner path
 * prel_to_cost_left_: spf_L - single-path function using left path
 * prel_to_cost_right_: spf_R - single-path function using right path
 * Indexed in left-to-right preorder.
 */
class PreLToSpfCost {
  public: std::vector<long long int> prel_to_cost_all_;
  public: std::vector<long long int> prel_to_cost_left_;
  public: std::vector<long long int> prel_to_cost_right_;
};

// Stores cost of deleting/inserting entire subtree for each node.
/**
 * prel_to_subtree_del_cost_: cost of deleting entire subtree
 * prel_to_subtree_ins_cost_: cost of inserting entire subtree
 * Indexed in left-to-right preorder.
 */
class PreLToSubtreeCost {
  public: std::vector<double> prel_to_subtree_del_cost_;
  public: std::vector<double> prel_to_subtree_ins_cost_;
};

/// Stores postorder ids of the keyroot nodes in the tree.
/**
 * Sorted in left-to-right postorder.
 */
class ListKR {
  public: std::vector<int> list_kr_;
};

/// For each depth value stores left-to-right postorder ids of nodes with that depth.
/**
 * Indexed in depth value.
 */
class InvertedListDepthToPostL {
  public: std::vector<std::vector<int>> inverted_list_depth_to_postl_;
};

/// For each label in the source tree, stores postorder ids of nodes that carry it.
/**
 * Each list is sorted in postorder.
 *
 * The inverted list is a hash map because we use a global LabelDictionary to
 * assign labels ids for an entire tree collection.
 *
 * The postorder ids of nodes carrying a specific label are stored in a
 * vector.
 */
class InvertedListLabelIdToPostL {
  public: std::unordered_map<int, std::vector<int>> inverted_list_label_id_to_postl_;
};

/// Tree index for Zhangh and Shasha algorithm.
class TreeIndexZhangShasha :
  public Constants,
  public PostLToLabelId,
  public PostLToLLD,
  public ListKR
{};

/// Tree index for APTED algorithm.
class TreeIndexAPTED :
  public Constants,
  public PreLToLabelId,
  public PostLToLabelId,
  public PostRToLabelId,
  public PreLToSize,
  public PreLToParent,
  public PreLToChildren,
  public PostLToLLD,
  public PostRToRLD,
  public PreLToLLD,
  public PreLToRLD,
  public PreToLn,
  public PreLToTypeLeft,
  public PreLToTypeRight,
  public PreLToPreR,
  public PreRToPreL,
  public PreLToPostL,
  public PostLToPreL,
  public PreLToPostR,
  public PostRToPreL,
  public PreLToSpfCost,
  public PreLToSubtreeCost
{};

/// Tree index for Touzet Baseline algorithm.
class TreeIndexTouzetBaseline :
  public Constants,
  public PostLToLabelId,
  public PostLToSize,
  public PostLToDepth
{};

/// Tree index for Touzet Depth Pruning algorithm.
class TreeIndexTouzetDepthPruning :
  public TreeIndexTouzetBaseline,
  public PostLToSubtreeMaxDepth,
  public InvertedListDepthToPostL
{};

/// Tree index for Touzet Depth Pruning with truncated tree fix algorithm.
class TreeIndexTouzetDepthPruningTruncatedTreeFix :
  virtual public TreeIndexTouzetBaseline,
  public PostLToSubtreeMaxDepth
{};

/// Tree index for Touzet KR Loop algorithm.
class TreeIndexTouzetKRLoop :
  public TreeIndexTouzetBaseline,
  public ListKR,
  public PostLToLCh
{};

/// Tree index for Touzet KR Set algorithm.
class TreeIndexTouzetKRSet :
  virtual public TreeIndexTouzetBaseline,
  public PostLToLCh,
  public PostLToKRAncestor,
  // missing dependency for PostLToKRAncestor
  public ListKR
{};

/// Tree index for String Edit Distance algorithm.
class TreeIndexSED :
  public Constants,
  public PreLToLabelId,
  public PostLToLabelId
{};

/// Tree index for Constrained Tree Edit Distance agorithm.
class TreeIndexCTED :
  public Constants,
  public PostLToLabelId,
  public PostLToChildren,
  public PostLToParent
{};

/// Tree index for SED and CTED agorithms.
class TreeIndexSCTED :
  public Constants,
  public PreLToLabelId,
  public PostLToLabelId,
  public PostLToChildren,
  public PostLToParent
{};

/// Tree index for LGM upper bound.
class TreeIndexLGM :
  public Constants,
  public PostLToLabelId,
  public PostLToSize,
  public PostLToDepth,
  public PostLToParent,
  public PostLToPreL,
  public PreLToPostL,
  public InvertedListLabelIdToPostL
{};

/// All tree indexes. Used for correctness tests and prototyping.
class TreeIndexAll :
  public Constants,
  public PreLToLabelId,
  public PostLToLabelId,
  public PostRToLabelId,
  public PostLToSize,
  public PreLToSize,
  public PostLToPreL,
  public PreLToPostL,
  public PreLToPreR,
  public PreRToPreL,
  public PreLToPostR,
  public PostRToPreL,
  public PostLToChildren,
  public PreLToChildren,
  public PostLToLLD,
  public PreLToLLD,
  public PreLToRLD,
  public PostRToRLD,
  public PostLToParent,
  public PreLToParent,
  public PostLToDepth,
  public PostLToLCh,
  public PostLToSubtreeMaxDepth,
  public PostLToKRAncestor,
  public PreToLn,
  public PreLToTypeLeft,
  public PreLToTypeRight,
  public PreLToSpfCost,
  public PreLToSubtreeCost,
  public ListKR,
  public InvertedListDepthToPostL,
  public InvertedListLabelIdToPostL
{};

/// Main method to index a tree.
/**
 * It executes further recursive tree traversal. It also resets the indexes
 * and sets their length.
 *
 * \param ti Tree index.
 * \param n Root node of the input tree.
 * \param ld LabelDictionary to collect node labels and assign their ids.
 */
template <typename TreeIndex, typename Label, typename CostModel>
void index_tree(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, const CostModel& cm);

/// Recursive tree traversal method that indexes a tree.
/**
 * Based on TreeIndex class, only index members of TreeIndex are filled in.
 *
 * \param ti Tree index.
 * \param n Root node of the input tree.
 * \param ld LabelDictionary to collect node labels and assign their ids.
 * \param cm CostModel - used for PreLToSubtreeCost indexes.
 * \param start_preorder Variable to hold current preorder id; modified during the recursion.
 * \param start_postorder Variable to hold current postorder id; modified during the recursion.
 * \param start_depth Variable to hold current depth of a node.
 * \param subtree_max_depth Variable to hold current maximum depth of a subtree; modified during the recursion.
 * \param parent_preorder The left-to-right preorder id of a parent node.
 * \param is_rightmost_child Is true if the node is the rightmost child of its parent.
 * \return Subtree size rooted at node n.
 */
template <typename TreeIndex, typename Label, typename CostModel>
int index_tree_recursion(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, const CostModel& cm,
    int& start_preorder, int& start_postorder,
    int start_depth, int& subtree_max_depth,
    int parent_preorder, bool is_rightmost_child);

// TODO: Combine fill_kr_ancestors, fill_ln, and fill_rld into a single
//       function. Check the base type inside.

/// Fills in the PostLToKRAncestor index.
/**
 * See description of the class PostLToKRAncestor.
 */
void fill_kr_ancestors(std::vector<int>& kr_ancestors,
    const std::vector<int>& postl_to_lch, const std::vector<int>& list_kr);

/// Fills in the PreToLn indexes.
/**
 * See description of the classes PreToLn.
 */
void fill_ln(std::vector<int>& prel_to_ln,
    std::vector<int>& prer_to_ln,
    const std::vector<int>& prel_to_size,
    const std::vector<int>& prer_to_prel);

/// Fills in the PostRToRLD indexes.
/**
 * See description of the classes PostRToRLD.
 */
void fill_rld(std::vector<int>& postr_to_rld,
    const std::vector<int>& prel_to_size,
    const std::vector<int>& postr_to_prel,
    const std::vector<int>& prel_to_postr,
    const std::vector<std::vector<int>>& prel_to_children);

// Implementation details
#include "tree_indexer_impl.h"

}
