// The MIT License (MIT)
// Copyright (c) 2019 Mateusz Pawlik.
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

template <typename TreeIndex, typename Label, typename CostModel>
void index_tree(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, const CostModel& cm) {
  
  int tree_size = n.get_tree_size();
  ti.tree_size_ = tree_size;
  
  // Resize and clear the indexes.
  // TODO: Resizing is not enough for some vectors in case the same TreeIndex
  //       object is used for multiple trees. Old incorrect values may remain
  //       in the vectors.
  if constexpr (std::is_base_of<PostLToSize, TreeIndex>::value) {
    ti.postl_to_size_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToSize, TreeIndex>::value) {
    ti.prel_to_size_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToParent, TreeIndex>::value) {
    ti.postl_to_parent_.resize(tree_size);
    std::fill(ti.postl_to_parent_.begin(), ti.postl_to_parent_.end(), -1);
  }
  if constexpr (std::is_base_of<PreLToParent, TreeIndex>::value) {
    ti.prel_to_parent_.resize(tree_size);
    std::fill(ti.prel_to_parent_.begin(), ti.prel_to_parent_.end(), -1);
  }
  if constexpr (std::is_base_of<PostLToPreL, TreeIndex>::value) {
    ti.postl_to_prel_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToPostL, TreeIndex>::value) {
    ti.prel_to_postl_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToPreR, TreeIndex>::value) {
    ti.prel_to_prer_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreRToPreL, TreeIndex>::value) {
    ti.prer_to_prel_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToPostR, TreeIndex>::value) {
    ti.prel_to_postr_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostRToPreL, TreeIndex>::value) {
    ti.postr_to_prel_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToChildren, TreeIndex>::value) {
    ti.postl_to_children_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToChildren, TreeIndex>::value) {
    ti.prel_to_children_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToLabelId, TreeIndex>::value) {
    ti.prel_to_label_id_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToLabelId, TreeIndex>::value) {
    ti.postl_to_label_id_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToType, TreeIndex>::value) {
    ti.postl_to_type_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostRToLabelId, TreeIndex>::value) {
    ti.postr_to_label_id_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToLLD, TreeIndex>::value) {
    ti.postl_to_lld_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToLLD, TreeIndex>::value) {
    ti.prel_to_lld_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToRLD, TreeIndex>::value) {
    ti.prel_to_rld_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostRToRLD, TreeIndex>::value) {
    ti.postr_to_rld_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToDepth, TreeIndex>::value) {
    ti.postl_to_depth_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToLCh, TreeIndex>::value) {
    ti.postl_to_lch_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToSubtreeMaxDepth, TreeIndex>::value) {
    ti.postl_to_subtree_max_depth_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToKRAncestor, TreeIndex>::value) {
    ti.postl_to_kr_ancestor_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreToLn, TreeIndex>::value) {
    ti.prel_to_ln_.resize(tree_size);
    ti.prer_to_ln_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PreLToTypeLeft, TreeIndex>::value) {
    ti.prel_to_type_left_.resize(tree_size);
    std::fill(ti.prel_to_type_left_.begin(), ti.prel_to_type_left_.end(), false);
  }
  if constexpr (std::is_base_of<PreLToTypeRight, TreeIndex>::value) {
    ti.prel_to_type_right_.resize(tree_size);
    std::fill(ti.prel_to_type_right_.begin(), ti.prel_to_type_right_.end(), false);
  }
  if constexpr (std::is_base_of<PreLToSpfCost, TreeIndex>::value) {
    ti.prel_to_cost_all_.resize(tree_size);
    std::fill(ti.prel_to_cost_all_.begin(), ti.prel_to_cost_all_.end(), 0);
    ti.prel_to_cost_left_.resize(tree_size);
    std::fill(ti.prel_to_cost_left_.begin(), ti.prel_to_cost_left_.end(), 0);
    ti.prel_to_cost_right_.resize(tree_size);
    std::fill(ti.prel_to_cost_right_.begin(), ti.prel_to_cost_right_.end(), 0);
  }
  if constexpr (std::is_base_of<PreLToSubtreeCost, TreeIndex>::value) {
    ti.prel_to_subtree_del_cost_.resize(tree_size);
    std::fill(ti.prel_to_subtree_del_cost_.begin(), ti.prel_to_subtree_del_cost_.end(), 0.0);
    ti.prel_to_subtree_ins_cost_.resize(tree_size);
    std::fill(ti.prel_to_subtree_ins_cost_.begin(), ti.prel_to_subtree_ins_cost_.end(), 0.0);
  }
  if constexpr (std::is_base_of<ListKR, TreeIndex>::value) {
    ti.list_kr_.clear();
  }
  if constexpr (std::is_base_of<InvertedListDepthToPostL, TreeIndex>::value) {
    ti.inverted_list_depth_to_postl_.clear();
  }
  if constexpr (std::is_base_of<InvertedListLabelIdToPostL, TreeIndex>::value) {
    ti.inverted_list_label_id_to_postl_.clear();
  }
  if constexpr (std::is_base_of<PostLToFavChild, TreeIndex>::value) {
    ti.postl_to_fav_child_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToFavChild, TreeIndex>::value) {
    ti.postl_to_left_fav_child_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToHeight, TreeIndex>::value) {
    ti.postl_to_height_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToOrderedChildSize, TreeIndex>::value) {
    ti.postl_to_ordered_child_size_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToFavorableChildOrder, TreeIndex>::value) {
    ti.postl_to_favorder_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToLeftSibling, TreeIndex>::value) {
    ti.postl_to_left_sibling_.resize(tree_size);
  }

  // Orders start with '0'. Are modified by the recursive traversal.
  int start_preorder = 0;
  int start_postorder = 0;
  int start_depth = 0;
  int start_height = 0;

  // Maximum input tree depth - the first reference passed to recursion.
  int subtree_max_depth = 0;
  // Initial height.
  int height = 0;
  index_tree_recursion(ti, n, ld, cm, start_preorder, start_postorder,
      start_depth, subtree_max_depth, start_height, height, -1, false);
  
  if constexpr (std::is_base_of<PostLToFavorableChildOrder, TreeIndex>::value) {
    // Compute favorable child order.
    int ts = tree_size - 1;
    int fid = 0;
    fav_child_processing_order(ti, ts, fid);
  }

  if constexpr (std::is_base_of<ListKR, TreeIndex>::value) {
    // Add root to kr - not added in the recursion.
    ti.list_kr_.push_back(start_postorder-1);
  }

  if constexpr (std::is_base_of<PostLToKRAncestor, TreeIndex>::value) {
    fill_kr_ancestors(ti.postl_to_kr_ancestor_, ti.postl_to_lch_, ti.list_kr_);
  }

  if constexpr (std::is_base_of<PreToLn, TreeIndex>::value) {
    fill_ln(ti.prel_to_ln_, ti.prer_to_ln_, ti.prel_to_size_, ti.prer_to_prel_);
  }

  if constexpr (std::is_base_of<PostRToRLD, TreeIndex>::value) {
    fill_rld(ti.postr_to_rld_, ti.prel_to_size_, ti.postr_to_prel_,
        ti.prel_to_postr_, ti.prel_to_children_);
  }
}

template <typename TreeIndex, typename Label, typename CostModel>
int index_tree_recursion(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, const CostModel& cm,
    int& start_preorder, int& start_postorder,
    int start_depth, int& subtree_max_depth,
    int start_height, int& height,
    int parent_preorder, bool is_rightmost_child) {
  
  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  int desc_sum = 0;

  // Stores the current node's label id.
  int label_id = ld.insert(n.label());

  // Stores the current node's type.
  int type = n.label().get_type();

  // Here, start_preorder holds this node's preorder id.
  
  int preorder_r = 0;
  int postorder_r = 0;

  // This node's preorder id.
  int this_nodes_preorder = start_preorder;

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  ++start_preorder;
  
  // To store postorder ids of this node's children.
  std::vector<int> children_postorders;
  // To store preorder ids of this node's children.
  std::vector<int> children_preorders;
  // Store the height of all children.
  std::vector<int> children_sorted_subtree_size;

  // InvertedListDepthToPostL index
  if constexpr (std::is_base_of<InvertedListDepthToPostL, TreeIndex>::value) {
    // Add a vector for depth=start_depth in depth inverted list.
    // NOTE: `inverted_list_depth_to_postl_.size()`` returns an `unsigned int`.
    //       If `inverted_list_depth_to_postl_.size = 0`, then
    //       substracting 1 causes incorrect validation of the condition.
    if (static_cast<int>(ti.inverted_list_depth_to_postl_.size()) < (start_depth + 1)) {
      ti.inverted_list_depth_to_postl_.push_back(std::vector<int>());
    }
  }
  
  // This node's subtree max depth.
  int this_subtree_max_depth = 0;
  // This node's height.
  int this_height = 0;

  // To store if the current child in the loop is rightmost.
  bool is_current_child_rightmost = false;

  // Treat the first child separately (non-key-root, updates parent's lld).
  int first_child_postorder = -1;
  // Number of nodes of children subtrees.
  int subtree_size_child = -1;
  // Number of nodes of the child with the largest subtree.
  int largest_subtree_size_child = -1;
  // Postorder number of the favorable child.
  int favorable_child = -1;
  // Postorder number of the left sibling of the favorable child.
  int left_fav_child = -1;
  // Postorder number of the previous child.
  int previous_child_po = -1;
  // Count number of children.
  int num_children = 0;
  // Recursions to childen nodes.
  auto children_start_it = std::begin(n.get_children());
  auto children_end_it = std::end(n.get_children());

  while (children_start_it != children_end_it) {
    // Add the preoder of the current child to children_preorders.
    children_preorders.push_back(start_preorder);

    // Check if this child is rightmost.
    if(std::next(children_start_it) == children_end_it) {
      is_current_child_rightmost = true;
    }

    // PreLToParent index
    if constexpr (std::is_base_of<PreLToParent, TreeIndex>::value) {
      ti.prel_to_parent_[start_preorder] = this_nodes_preorder;
    }
    
    subtree_size_child = index_tree_recursion(ti, *children_start_it, ld, cm,
        start_preorder, start_postorder, start_depth + 1,
        this_subtree_max_depth, start_height, height,
        this_nodes_preorder, is_current_child_rightmost);
    desc_sum += subtree_size_child;
    
    // Add the postorder of the current child to children_postorders.
    children_postorders.push_back(start_postorder-1);
    
    // PostLToFavChild index
    if constexpr (std::is_base_of<PostLToFavChild, TreeIndex>::value) {
      if (subtree_size_child > largest_subtree_size_child) {
        largest_subtree_size_child = subtree_size_child;
        left_fav_child = previous_child_po;
        favorable_child = start_postorder-1;
      }
    }

    // PostLToHeight index
    if constexpr (std::is_base_of<PostLToHeight, TreeIndex>::value) {
      if (height > this_height) {
        this_height = height;
      }
    }

    // PostLToLeftSibling index
    if constexpr (std::is_base_of<PostLToLeftSibling, TreeIndex>::value) {
      ti.postl_to_left_sibling_[start_postorder-1] = previous_child_po;
    }
    
    if constexpr (std::is_base_of<PostLToHeight, TreeIndex>::value) {
      children_sorted_subtree_size.push_back(subtree_size_child);
    }

    // Treat the first child separately.
    if (children_start_it == n.get_children().begin()) {
      // Here, start_postorder-1 is the postorder of the current child.
      // Set this node's lld to its first child's lld.
      first_child_postorder = start_postorder-1;
    } else {
      if constexpr (std::is_base_of<ListKR, TreeIndex>::value) {
        // Add current child to kr.
        ti.list_kr_.push_back(start_postorder-1);
      }
    }
    
    ++num_children;

    previous_child_po = start_postorder-1;
    
    // Continue to consecutive children.
    ++children_start_it;
  }

  // Here, start_postorder holds this node's postorder id.
  
  // Calculate right-to-left preorder.
  preorder_r = ti.tree_size_ - 1 - start_postorder;
  
  // Calculate right-to-left postorder.
  postorder_r = ti.tree_size_ - 1 - this_nodes_preorder;
  
  // PostLToFavChild index
  if constexpr (std::is_base_of<PostLToFavChild, TreeIndex>::value) {
    ti.postl_to_fav_child_[start_postorder] = favorable_child;
    ti.postl_to_left_fav_child_[start_postorder] = left_fav_child;
  }

  // PostLToOrderedChildSize
  if constexpr (std::is_base_of<PostLToOrderedChildSize, TreeIndex>::value) {
    // Sort all children by subtree size.
    std::sort (children_sorted_subtree_size.begin(), children_sorted_subtree_size.end());

    // Removing subtree costs sorted ascending.
    std::vector<int> children_subtree_deletion_costs;
    // Iterate over all children and sum up the costs.
    int current_child = 0;
    for (std::vector<int>::iterator it=children_sorted_subtree_size.begin(); it!=children_sorted_subtree_size.end(); ++it) {
      current_child = children_subtree_deletion_costs.size();
      if (current_child == 0) {
        children_subtree_deletion_costs.push_back(*it);
      } else {
        children_subtree_deletion_costs.push_back(children_subtree_deletion_costs[current_child-1] + *it);
      }
    }
    ti.postl_to_ordered_child_size_[start_postorder] = children_subtree_deletion_costs;
  }

  // PostLToHeight index
  if constexpr (std::is_base_of<PostLToHeight, TreeIndex>::value) {
    height = this_height + 1;
    ti.postl_to_height_[start_postorder] = height;
  }

  // PostLToSize index
  if constexpr (std::is_base_of<PostLToSize, TreeIndex>::value) {
    ti.postl_to_size_[start_postorder] = desc_sum + 1;
  }
  
  // PreLToSize index
  if constexpr (std::is_base_of<PreLToSize, TreeIndex>::value) {
    ti.prel_to_size_[this_nodes_preorder] = desc_sum + 1;
  }
  
  // PostLToParent index
  if constexpr (std::is_base_of<PostLToParent, TreeIndex>::value) {
    for (const auto& child_id : children_postorders) {
      ti.postl_to_parent_[child_id] = start_postorder;
    }
  }
  
  // PostLToPreL index
  if constexpr (std::is_base_of<PostLToPreL, TreeIndex>::value) {
    ti.postl_to_prel_[start_postorder] = this_nodes_preorder;
  }
  
  // PreLToPostL index
  if constexpr (std::is_base_of<PreLToPostL, TreeIndex>::value) {
    ti.prel_to_postl_[this_nodes_preorder] = start_postorder;
  }
  
  // PreLToPreR index
  if constexpr (std::is_base_of<PreLToPreR, TreeIndex>::value) {
    ti.prel_to_prer_[this_nodes_preorder] = preorder_r;
  }
  
  // PreRToPreL index
  if constexpr (std::is_base_of<PreRToPreL, TreeIndex>::value) {
    ti.prer_to_prel_[preorder_r] = this_nodes_preorder;
  }
  
  // PreLToPostR index
  if constexpr (std::is_base_of<PreLToPostR, TreeIndex>::value) {
    ti.prel_to_postr_[this_nodes_preorder] = postorder_r;
  }
  
  // PostRToPreL index
  if constexpr (std::is_base_of<PostRToPreL, TreeIndex>::value) {
    ti.postr_to_prel_[postorder_r] = this_nodes_preorder;
  }
  
  // PostLToChildren index
  if constexpr (std::is_base_of<PostLToChildren, TreeIndex>::value) {
    ti.postl_to_children_[start_postorder] = children_postorders;
  }
  
  // PreLToChildren index
  if constexpr (std::is_base_of<PreLToChildren, TreeIndex>::value) {
    ti.prel_to_children_[this_nodes_preorder] = children_preorders;
  }
  
  // PreLToLabelId index
  if constexpr (std::is_base_of<PreLToLabelId, TreeIndex>::value) {
    ti.prel_to_label_id_[this_nodes_preorder] = label_id;
  }
  
  // PostLToLabelId index
  if constexpr (std::is_base_of<PostLToLabelId, TreeIndex>::value) {
    ti.postl_to_label_id_[start_postorder] = label_id;
  }
  
  // PostLToType index
  if constexpr (std::is_base_of<PostLToType, TreeIndex>::value) {
    ti.postl_to_type_[start_postorder] = type;
  }
  
  // InvertedListLabelIdToPostL index
  if constexpr (std::is_base_of<InvertedListLabelIdToPostL, TreeIndex>::value) {
    ti.inverted_list_label_id_to_postl_[label_id].push_back(start_postorder);
  }
  
  // PostRToLabelId index
  if constexpr (std::is_base_of<PostRToLabelId, TreeIndex>::value) {
    ti.postr_to_label_id_[postorder_r] = label_id;
  }
  
  // PostLToDepth index
  if constexpr (std::is_base_of<PostLToDepth, TreeIndex>::value) {
    ti.postl_to_depth_[start_postorder] = start_depth;
  }
  
  // InvertedListDepthToPostL index
  if constexpr (std::is_base_of<InvertedListDepthToPostL, TreeIndex>::value) {
    ti.inverted_list_depth_to_postl_.at(start_depth).push_back(start_postorder);
  }

  // PostLToSubtreeMaxDepth index
  if constexpr (std::is_base_of<PostLToSubtreeMaxDepth, TreeIndex>::value) {
    if (n.is_leaf()) {
      // If this node has no children, set the max depth to this node's depth.
      this_subtree_max_depth = start_depth;
    }
    ti.postl_to_subtree_max_depth_[start_postorder] = this_subtree_max_depth;
    // Update parent subtree's max depth.
    subtree_max_depth = std::max(subtree_max_depth, this_subtree_max_depth);
  }
  
  // PostLToHeight index
  if constexpr (std::is_base_of<PostLToHeight, TreeIndex>::value) {
    if (n.is_leaf()) {
      // If this node has no children, set the max depth to this node's depth.
      this_height = start_height;
    }
    ti.postl_to_height_[start_postorder] = this_height;
    // Update parent subtree's max depth.
    height = std::max(height, this_height);
  }
  
  // PostLToLCh index
  if constexpr (std::is_base_of<PostLToLCh, TreeIndex>::value) {
    ti.postl_to_lch_[start_postorder] = first_child_postorder;
  }
  
  // PostLToLLD index
  if constexpr (std::is_base_of<PostLToLLD, TreeIndex>::value) {
    if (n.is_leaf()) {
      // Set lld of this node to this node's postorer.
      ti.postl_to_lld_[start_postorder] = start_postorder;
    } else {
      // This node's lld must be pushed after its childrens llds.
      // lld is indexed starting with 0, thus first_child_postorder-1.
      ti.postl_to_lld_[start_postorder] = ti.postl_to_lld_[first_child_postorder];
    }
  }

  // PreLToLLD index
  if constexpr (std::is_base_of<PreLToLLD, TreeIndex>::value) {
    if (n.is_leaf()) {
      // Set lld of this node to this node's preorder.
      ti.prel_to_lld_[this_nodes_preorder] = this_nodes_preorder;
    } else {
      // Push this node's lld to its first child (this_nodes_preorder + 1).
      ti.prel_to_lld_[this_nodes_preorder] = ti.prel_to_lld_[this_nodes_preorder + 1];
    }
  }

  // PreLToRLD index
  if constexpr (std::is_base_of<PreLToRLD, TreeIndex>::value) {
    // RLD of a node n is: pre(n)+size(n)-1; size(n)=desc_sum+1
    ti.prel_to_rld_[this_nodes_preorder] = this_nodes_preorder + desc_sum;
  }

  // PreLToTypeLeft index
  if constexpr (std::is_base_of<PreLToTypeLeft, TreeIndex>::value) {
    if (num_children > 0) {
      ti.prel_to_type_left_[children_preorders[0]] = true;
    }
  }

  // PreLToTypeRight index
  if constexpr (std::is_base_of<PreLToTypeRight, TreeIndex>::value) {
    if (num_children > 0) {
      ti.prel_to_type_right_[children_preorders[num_children-1]] = true;
    }
  }

  // PreLToSpfCost indexes
  // TODO: Indexes store long long int, but desc_sum is an
  //       int. Verify if there is no weird conversion.
  if constexpr (std::is_base_of<PreLToSpfCost, TreeIndex>::value) {
    // Add this node's subtree size to this node's sum.
    ti.prel_to_cost_all_[this_nodes_preorder] += desc_sum + 1;
    // If this node has a parent.
    if (parent_preorder != -1) {
      // Update the parent with this node's sum.
      ti.prel_to_cost_all_[parent_preorder] += ti.prel_to_cost_all_[this_nodes_preorder];
      ti.prel_to_cost_left_[parent_preorder] += ti.prel_to_cost_left_[this_nodes_preorder];
      ti.prel_to_cost_right_[parent_preorder] += ti.prel_to_cost_right_[this_nodes_preorder];
      // Check if this node is leftmost child of its parent.
      if (this_nodes_preorder != parent_preorder + 1) {
        ti.prel_to_cost_left_[parent_preorder] += desc_sum + 1;
      }
      // Check if this node is rightmost child of its parent.
      if (!is_rightmost_child) {
        ti.prel_to_cost_right_[parent_preorder] += desc_sum + 1;
      }
    }
    // Calculate the cost and store.
    ti.prel_to_cost_all_[this_nodes_preorder] =
        (desc_sum + 1) * (desc_sum + 1 + 3) / 2 - ti.prel_to_cost_all_[this_nodes_preorder];
    // Add this node's subtree size to this node's sum.
    ti.prel_to_cost_left_[this_nodes_preorder] += desc_sum + 1;
    ti.prel_to_cost_right_[this_nodes_preorder] += desc_sum + 1;
  }

  // PreLToSubtreeCost indexes
  if constexpr (std::is_base_of<PreLToSubtreeCost, TreeIndex>::value) {
    // Add the cost of this node's subtree.
    ti.prel_to_subtree_del_cost_[this_nodes_preorder] += cm.del(label_id);
    ti.prel_to_subtree_ins_cost_[this_nodes_preorder] += cm.ins(label_id);
    // If this node has a parent.
    if (parent_preorder != -1) {
      // Update the cost of the parent node subtree.
      ti.prel_to_subtree_del_cost_[parent_preorder] += ti.prel_to_subtree_del_cost_[this_nodes_preorder];
      ti.prel_to_subtree_ins_cost_[parent_preorder] += ti.prel_to_subtree_ins_cost_[this_nodes_preorder];
    }
  }

  // Increment start_postorder for the consecutive node in postorder to have the
  // correct id.
  ++start_postorder;

  // Return the number of nodes in the subtree rooted at this node.
  return desc_sum + 1;
  
}

void fill_kr_ancestors(std::vector<int>& postl_to_kr_ancestor,
    const std::vector<int>& postl_to_lch, const std::vector<int>& list_kr) {
  for (auto i : list_kr) {
    int l = i;
    while (l >= 0) {
      postl_to_kr_ancestor[l] = i;
      l = postl_to_lch[l];
    }
  }
}

void fill_ln(std::vector<int>& prel_to_ln,
    std::vector<int>& prer_to_ln,
    const std::vector<int>& prel_to_size,
    const std::vector<int>& prer_to_prel) {
  int current_leaf_prel = -1;
  int current_leaf_prer = -1;
  for(int i = 0; i < prel_to_size[0]; ++i) {
    prel_to_ln[i] = current_leaf_prel;
    if(prel_to_size[i] == 1) {
      current_leaf_prel = i;
    }
    prer_to_ln[i] = current_leaf_prer;
    if(prel_to_size[prer_to_prel[i]] == 1) {
      current_leaf_prer = i;
    }
  }
}

void fill_rld(std::vector<int>& postr_to_rld,
    const std::vector<int>& prel_to_size,
    const std::vector<int>& postr_to_prel,
    const std::vector<int>& prel_to_postr,
    const std::vector<std::vector<int>>& prel_to_children) {
  // The loop iterates over right-to-left postorder.
  int preorder = 0;
  for(int i = 0; i < prel_to_size[0]; ++i) {
    preorder = postr_to_prel[i];
    if (prel_to_size[preorder] == 1) {
      postr_to_rld[i] = i;
    } else {
      postr_to_rld[i] = postr_to_rld[prel_to_postr[prel_to_children[preorder][prel_to_children[preorder].size()-1]]];
    }
  }
}

template <typename TreeIndex>
void fav_child_processing_order(TreeIndex& ti, int& postorder, int& favorder) {
  // Perform recursively for children.
  if (ti.postl_to_children_[postorder].size() != 0) {
    // First process the favorable child.
    fav_child_processing_order(ti, ti.postl_to_fav_child_[postorder], favorder);
    // Next, process all other children.
    for (unsigned int t = 0; t < ti.postl_to_children_[postorder].size(); ++t) {
      // Favorable child was processed before.
      if (ti.postl_to_children_[postorder][t] == ti.postl_to_fav_child_[postorder]) {
        continue;
      }
      // Process other children in left to right order.
      fav_child_processing_order(ti, ti.postl_to_children_[postorder][t], favorder);
    }
  }
  // Assign favorder to postorder.
  ti.postl_to_favorder_[favorder++] = postorder;
}