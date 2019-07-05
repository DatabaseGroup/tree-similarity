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
#include <string>
#include <type_traits>
#include "label_dictionary.h"

namespace node {

/// Stores the size of the indexed tree.
class Constants {
  public: unsigned int tree_size_;
};

/// Stores label id of each node in a tree.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToLabelId {
  public: std::vector<unsigned int> postl_to_label_id_;
};

/// Stores subtree size of each node in a tree.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToSize {
  public: std::vector<unsigned int> postl_to_size_;
};

/// Stores subtree size of each node in a tree.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToSize {
  public: std::vector<unsigned int> prel_to_size_;
};

/// Stores left-to-right postorder id of the parent node.
/**
 * Indexed in left-to-right postorder.
 * '-1' represents no parent.
 * TODO: Maybe use unsigned int and a max value for no parent. Requires changes
 *       in algorithms implementation. If done, apply in corresponding indexes.
 */
class PostLToParent {
  public: std::vector<int> postl_to_parent_;
};

/// Stores left-to-right preorder id of the parent node.
/**
 * Indexed in left-to-right preorder.
 * '-1' represents no parent.
 */
class PreLToParent {
  public: std::vector<int> prel_to_parent_;
};

/// Stores left-to-right postorder id of the leftmost leaf descendant of a node.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToLLD {
  public: std::vector<unsigned int> postl_to_lld_;
};

/// Stores left-to-right preorder id of each node.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToPreL {
  public: std::vector<unsigned int> postl_to_prel_;
};

/// Stores left-to-right postorder id of each node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToPostL {
  public: std::vector<unsigned int> prel_to_postl_;
};

/// Stores right-to-left preorder id of each node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToPreR {
  public: std::vector<unsigned int> prel_to_prer_;
};

/// Stores left-to-right preorder id of each node.
/**
 * Indexed in right-to-left preorder.
 */
class PreRToPreL {
  public: std::vector<unsigned int> prer_to_prel_;
};

/// Stores right-to-left postorder id of each node.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToPostR {
  public: std::vector<unsigned int> prel_to_postr_;
};

/// Stores left-to-right preorder id of each node.
/**
 * Indexed in right-to-left postorder.
 */
class PostRToPreL {
  public: std::vector<unsigned int> postr_to_prel_;
};

/// Stores left-to-right postorder ids of each node's children.
/**
 * Indexed in left-to-right postorder.
 */
class PostLToChildren {
  public: std::vector<std::vector<unsigned int>> postl_to_children_;
};

/// Stores left-to-right preorder ids of each node's children.
/**
 * Indexed in left-to-right preorder.
 */
class PreLToChildren {
  public: std::vector<std::vector<unsigned int>> prel_to_children_;
};

/// Stores depth of every node in the tree.
/**
 * Indexed in left-to-right postorder.
 * Depth of the root node is 0.
 */
class PostLToDepth {
  public: std::vector<unsigned int> postl_to_depth_;
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
  public: std::vector<unsigned int> postl_to_subtree_max_depth_;
};

/// Stores the left-to-right postorder id of the nearest keyroot ancestor.
/**
 * Indexed in left-to-right postorder.
 * The nearest keyroot ancestor of a keyroot node n is n.
 * Depends on: PostLToLCh, ListKR.
 */
class PostLToKRAncestor {
  public: std::vector<unsigned int> postl_to_kr_ancestor_;
};

// Stores preorder id of the first leaf node to the left/right.
/**
 * prel_to_ln_: left-to-right preorder of the first leaf to the left.
 * prer_to_ln_: right-to-left preorder of the first leaf to the right.
 * '-1' represents no such node.
 * Depends on: PreLToSize, PreRToPreL.
 */
class PreToLn {
  public: std::vector<int> prel_to_ln_;
  public: std::vector<int> prer_to_ln_;
};

/// Stores postorder ids of the keyroot nodes in the tree.
/**
 * Sorted in left-to-right postorder.
 */
class ListKR {
  public: std::vector<unsigned int> list_kr_;
};

/// Tree index for Zhangh and Shasha algorithm.
class TreeIndexZhangShasha :
  public Constants,
  public PostLToLabelId,
  public PostLToLLD,
  public ListKR
{};

/// All tree indexes. Used for correctness tests and prototyping.
class TreeIndexAll :
  public Constants,
  public PostLToLabelId,
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
  public PostLToParent,
  public PreLToParent,
  public PostLToDepth,
  public PostLToLCh,
  public PostLToSubtreeMaxDepth,
  public PostLToKRAncestor,
  public PreToLn,
  public ListKR
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
template <typename TreeIndex, typename Label>
void index_tree(TreeIndex& ti, const node::Node<Label>& n, label::LabelDictionary<Label>& ld);

/// Recursive tree traversal method that indexes a tree.
/**
 * Based on TreeIndex class, only index members of TreeIndex are filled in.
 *
 * \param ti Tree index.
 * \param n Root node of the input tree.
 * \param ld LabelDictionary to collect node labels and assign their ids.
 * \param start_preorder Variable to hold current preorder id; modified during the recursion.
 * \param start_postorder Variable to hold current postorder id; modified during the recursion.
 * \return Subtree size rooted at node n.
 */
template <typename TreeIndex, typename Label>
unsigned int index_tree_recursion(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, unsigned int& start_preorder,
    unsigned int& start_postorder, unsigned int start_depth, unsigned int& subtree_max_depth);

/// Fills in the PostLToKRAncestor index.
/**
 * See description of the class PostLToKRAncestor.
 */
void fill_kr_ancestors(std::vector<unsigned int>& kr_ancestors,
    const std::vector<int>& postl_to_lch, const std::vector<unsigned int>& list_kr);

/// Fills in the PreToLn indexes.
/**
 * See description of the classes PreToLn.
 * Depends on: 
 */
void fill_ln(std::vector<int>& prel_to_ln, std::vector<int>& prer_to_ln,
    const std::vector<unsigned int>& prel_to_size, const std::vector<unsigned int>& prer_to_prel);

// Implementation details
#include "tree_indexer_impl.h"

}
