// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik.
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

/// \file ted/apted_impl.h
///
/// \details
/// Contains the implementation of the APTED algorithm.

#ifndef TREE_SIMILARITY_TED_APTED_IMPL_H
#define TREE_SIMILARITY_TED_APTED_IMPL_H

template <typename Label>
APTEDNodeIndexer<Label>::APTEDNodeIndexer(const node::Node<Label>& t) {
  // Initialise variables.
  current_node_ = 0;
  size_tmp_ = 0;
  desc_sizes_tmp_ = 0;
  kr_sizes_sum_tmp_ = 0;
  rev_kr_sizes_sum_tmp_ = 0;
  preorder_tmp_ = 0;
  tree_size_ = t.get_tree_size();

  // Initialise indices with the lengths equal to the tree size.
  preL_to_node_.reserve(tree_size_);
  preL_to_size_.resize(tree_size_);
  preL_to_parent_.resize(tree_size_);
  preL_to_children_.resize(tree_size_);
  postL_to_lld_.resize(tree_size_);
  postR_to_rld_.resize(tree_size_);
  preL_to_ln_.resize(tree_size_);
  preR_to_ln_.resize(tree_size_);
  preL_to_node_type_L_.resize(tree_size_);
  preL_to_node_type_R_.resize(tree_size_);
  preL_to_preR_.resize(tree_size_);
  preR_to_preL_.resize(tree_size_);
  preL_to_postL_.resize(tree_size_);
  postL_to_preL_.resize(tree_size_);
  preL_to_postR_.resize(tree_size_);
  postR_to_preL_.resize(tree_size_);
  preL_to_kr_sum_.resize(tree_size_);
  preL_to_rev_kr_sum_.resize(tree_size_);
  preL_to_desc_sum_.resize(tree_size_);
  preL_to_sumDelCost_.resize(tree_size_);
  preL_to_sumInsCost_.resize(tree_size_);

  preL_to_parent_[0] = -1; // The root has no parent.

  // Index the nodes.
  index_nodes(t, -1);
  // postTraversalIndexing();
}

template <typename Label>
int APTEDNodeIndexer<Label>::index_nodes(const node::Node<Label>& node, int postorder) {
    // Initialise variables.
    int current_size = 0;
    int children_count = 0;
    int desc_sizes = 0;
    int kr_sizes_sum = 0;
    int rev_kr_sizes_sum = 0;
    int preorder = preorder_tmp_;
    int preorder_r = 0;
    int current_preorder = -1;
    // Initialise empty array to store children of this node.
    std::vector<int> children_preorders;

    // Store the preorder id of the current node to use it after the recursion.
    ++preorder_tmp_;

    // Loop over children of a node.
    const std::vector<node::Node<Label>>& children = node.get_children();
    for (auto children_it = children.begin(); children_it != children.end(); ++children_it) {
      ++children_count;
      current_preorder = preorder_tmp_;
      preL_to_parent_[current_preorder] = preorder;

      // Execute method recursively for next child.
      postorder = index_nodes(*children_it, postorder);

      children_preorders.push_back(current_preorder);

      current_size += 1 + size_tmp_;
      desc_sizes += desc_sizes_tmp_;
      if(children_count > 1) {
          kr_sizes_sum += kr_sizes_sum_tmp_ + size_tmp_ + 1;
      } else {
          kr_sizes_sum += kr_sizes_sum_tmp_;
          preL_to_node_type_L_[current_preorder] = true;
      }
      if(std::next(children_it) != children.end()) {
          rev_kr_sizes_sum += rev_kr_sizes_sum_tmp_ + size_tmp_ + 1;
      } else {
          rev_kr_sizes_sum += rev_kr_sizes_sum_tmp_;
          preL_to_node_type_R_[current_preorder] = true;
      }
    }

    ++postorder;

    int current_desc_sizes = desc_sizes + current_size + 1;
    preL_to_desc_sum_[preorder] = ((current_size + 1) * (current_size + 1 + 3)) / 2 - current_desc_sizes;
    preL_to_kr_sum_[preorder] = kr_sizes_sum + current_size + 1;
    preL_to_rev_kr_sum_[preorder] = rev_kr_sizes_sum + current_size + 1;

    // Store pointer to a node object corresponding to preorder.
    preL_to_node_[preorder] = std::ref(node);

    preL_to_size_[preorder] = current_size + 1;
    preorder_r = tree_size_ - 1 - postorder;
    preL_to_preR_[preorder] = preorder_r;
    preR_to_preL_[preorder_r] = preorder;

    preL_to_children_[preorder] = std::vector<int>(children_preorders);

    desc_sizes_tmp_ = current_desc_sizes;
    size_tmp_ = current_size;
    kr_sizes_sum_tmp_ = kr_sizes_sum;
    rev_kr_sizes_sum_tmp_ = rev_kr_sizes_sum;

    postL_to_preL_[postorder] = preorder;
    preL_to_postL_[preorder] = postorder;
    preL_to_postR_[preorder] = tree_size_ - 1 - preorder;
    postR_to_preL_[tree_size_ - 1 - preorder] = preorder;

    return postorder;
}

template <typename Label, typename CostModel>
APTED<Label, CostModel>::APTED() : c_() {}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::apted_ted(const node::Node<Label>& t1, const node::Node<Label>& t2) {
  APTEDNodeIndexer<Label> ni_1(t1);
  APTEDNodeIndexer<Label> ni_2(t2);
  return apted_ted(ni_1, ni_2);
}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::apted_ted(const APTEDNodeIndexer<Label>& ni_1, const APTEDNodeIndexer<Label>& ni_2) {
  return 111.111;
}

// template <typename Label, typename CostModel>
// const TestItems& APTED<Label, CostModel>::get_test_items(const node::Node<Label>& t1) const {
//
// }

#endif // TREE_SIMILARITY_TED_APTED_IMPL_H
