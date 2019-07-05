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

#pragma once

template <typename TreeIndex, typename Label>
void index_tree(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld) {
  
  unsigned int tree_size = n.get_tree_size();
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
  if constexpr (std::is_base_of<PostLToLabelId, TreeIndex>::value) {
    ti.postl_to_label_id_.resize(tree_size);
  }
  if constexpr (std::is_base_of<PostLToLLD, TreeIndex>::value) {
    ti.postl_to_lld_.resize(tree_size);
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
  if constexpr (std::is_base_of<ListKR, TreeIndex>::value) {
    ti.list_kr_.clear();
  }
  
  // Orders start with '0'. Are modified by the recursive traversal.
  unsigned int start_preorder = 0;
  unsigned int start_postorder = 0;
  unsigned int start_depth = 0;
  // Maximum input tree depth - the first reference passed to recursion.
  unsigned int subtree_max_depth = 0;
  index_tree_recursion(ti, n, ld, start_preorder, start_postorder, start_depth, subtree_max_depth);
  
  if constexpr (std::is_base_of<ListKR, TreeIndex>::value) {
    // Add root to kr - not added in the recursion.
    ti.list_kr_.push_back(start_postorder-1);
  }
};

template <typename TreeIndex, typename Label>
unsigned int index_tree_recursion(TreeIndex& ti, const node::Node<Label>& n,
    label::LabelDictionary<Label>& ld, unsigned int& start_preorder,
    unsigned int& start_postorder, unsigned int start_depth, unsigned int& subtree_max_depth) {
  
  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  unsigned int desc_sum = 0;

  // Here, start_preorder holds this node's preorder id.
  
  unsigned int preorder_temp = start_preorder;
  unsigned int preorder_r = 0;
  unsigned int postorder_r = 0;

  // Increment start_preorder to hold the correct id of the consecutive node
  // in preorder.
  ++start_preorder;
  
  // To store postorder ids of this node's children.
  std::vector<unsigned int> children_postorders;
  // To store preorder ids of this node's children.
  std::vector<unsigned int> children_preorders;

  // This node subtree's max depth.
  unsigned int this_subtree_max_depth = 0;
  
  // Treat the first child separately (non-key-root, updates parent's lld).
  int first_child_postorder = -1;
  // Recursions to childen nodes.
  auto children_start_it = std::begin(n.get_children());
  auto children_end_it = std::end(n.get_children());
  while (children_start_it != children_end_it) {
    // Add the preoder of the current child to children_preorders.
    children_preorders.push_back(start_preorder);
    
    desc_sum += index_tree_recursion(ti, *children_start_it, ld, start_preorder,
        start_postorder, start_depth + 1, this_subtree_max_depth);
    
    // Add the postoder of the current child to children_postorders.
    children_postorders.push_back(start_postorder-1);
    
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
    
    // Continue to consecutive children.
    ++children_start_it;
  }

  // Here, start_postorder holds this node's postorder id.
  
  // Calculate right-to-left preorder.
  preorder_r = ti.tree_size_ - 1 - start_postorder;
  
  // Calculate right-to-left postorder.
  postorder_r = ti.tree_size_ - 1 - preorder_temp;
  
  // PostLToSize index
  if constexpr (std::is_base_of<PostLToSize, TreeIndex>::value) {
    if (n.is_leaf()) {
      // Leaf has size 1.
      ti.postl_to_size_[start_postorder] = 1;
    } else {
      // Inner node has size desc_sum+1.
      ti.postl_to_size_[start_postorder] = desc_sum + 1;
    }
  }
  
  // PreLToSize index
  if constexpr (std::is_base_of<PreLToSize, TreeIndex>::value) {
    if (n.is_leaf()) {
      // Leaf has size 1.
      ti.prel_to_size_[preorder_temp] = 1;
    } else {
      // Inner node has size desc_sum+1.
      ti.prel_to_size_[preorder_temp] = desc_sum + 1;
    }
  }
  
  // PostLToParent index
  if constexpr (std::is_base_of<PostLToParent, TreeIndex>::value) {
    for (const auto& child_id : children_postorders) {
      ti.postl_to_parent_[child_id] = start_postorder;
    }
  }
  
  // PreLToParent index
  if constexpr (std::is_base_of<PreLToParent, TreeIndex>::value) {
    for (const auto& child_id : children_preorders) {
      ti.prel_to_parent_[child_id] = preorder_temp;
    }
  }
  
  // PostLToPreL index
  if constexpr (std::is_base_of<PostLToPreL, TreeIndex>::value) {
    ti.postl_to_prel_[start_postorder] = preorder_temp;
  }
  
  // PreLToPostL index
  if constexpr (std::is_base_of<PreLToPostL, TreeIndex>::value) {
    ti.prel_to_postl_[preorder_temp] = start_postorder;
  }
  
  // PreLToPreR index
  if constexpr (std::is_base_of<PreLToPreR, TreeIndex>::value) {
    ti.prel_to_prer_[preorder_temp] = preorder_r;
  }
  
  // PreRToPreL index
  if constexpr (std::is_base_of<PreRToPreL, TreeIndex>::value) {
    ti.prer_to_prel_[preorder_r] = preorder_temp;
  }
  
  // PreLToPostR index
  if constexpr (std::is_base_of<PreLToPostR, TreeIndex>::value) {
    ti.prel_to_postr_[preorder_temp] = postorder_r;
  }
  
  // PostRToPreL index
  if constexpr (std::is_base_of<PostRToPreL, TreeIndex>::value) {
    ti.postr_to_prel_[postorder_r] = preorder_temp;
  }
  
  // PostLToChildren index
  if constexpr (std::is_base_of<PostLToChildren, TreeIndex>::value) {
    ti.postl_to_children_[start_postorder] = children_postorders;
  }
  
  // PreLToChildren index
  if constexpr (std::is_base_of<PreLToChildren, TreeIndex>::value) {
    ti.prel_to_children_[preorder_temp] = children_preorders;
  }
  
  // PostLToLabelId index
  if constexpr (std::is_base_of<PostLToLabelId, TreeIndex>::value) {
    ti.postl_to_label_id_[start_postorder] = ld.insert(n.label());
  }
  
  // PostLToDepth index
  if constexpr (std::is_base_of<PostLToDepth, TreeIndex>::value) {
    ti.postl_to_depth_[start_postorder] = start_depth;
  }
  
  // PostLToLCh index
  if constexpr (std::is_base_of<PostLToLCh, TreeIndex>::value) {
    ti.postl_to_lch_[start_postorder] = first_child_postorder;
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

  // Increment start_postorder for the consecutive node in postorder to have the
  // correct id.
  ++start_postorder;

  // Return the number of nodes in the subtree rooted at this node.
  return desc_sum + 1;
  
};
