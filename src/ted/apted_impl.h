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

template <typename Label, typename CostModel>
APTEDNodeIndexer<Label, CostModel>::APTEDNodeIndexer(const node::Node<Label>& t) : c_() {
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
  post_traversal_indexing();
}

template <typename Label, typename CostModel>
int APTEDNodeIndexer<Label, CostModel>::index_nodes(const node::Node<Label>& node, int postorder) {
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
void APTEDNodeIndexer<Label, CostModel>::post_traversal_indexing() {
  int current_leaf = -1;
  int node_for_sum = -1;
  int parent_for_sum = -1;
  for(int i = 0; i < tree_size_; ++i) {
    preL_to_ln_[i] = current_leaf;
    if(is_leaf(i)) {
      current_leaf = i;
    }

    // This block stores leftmost leaf descendants for each node
    // indexed in postorder. Used for mapping computation.
    // Added by Victor.
    int postl = i; // Assume that the for loop iterates postorder.
    int preorder = postL_to_preL_[i];
    if (preL_to_size_[preorder] == 1) {
      postL_to_lld_[postl] = postl;
    } else {
      postL_to_lld_[postl] = postL_to_lld_[preL_to_postL_[preL_to_children_[preorder][0]]];
    }
    // This block stores rightmost leaf descendants for each node
    // indexed in right-to-left postorder.
    // [TODO] Use postL_to_lld and postR_to_rld instead of APTED.getLLD
    //        and APTED.gerRLD methods, remove these method.
    //        Result: faster lookup of these values.
    int postr = i; // Assume that the for loop iterates reversed postorder.
    preorder = postR_to_preL_[postr];
    if (preL_to_size_[preorder] == 1) {
      postR_to_rld_[postr] = postr;
    } else {
      postR_to_rld_[postr] = postR_to_rld_[preL_to_postR_[preL_to_children_[preorder][preL_to_children_[preorder].size()-1]]];
    }
    // Count lchl and rchl.
    // [TODO] There are no values for parent node.
    if (preL_to_size_[i] == 1) {
      int parent = preL_to_parent_[i];
      if (parent > -1) {
        if (parent+1 == i) {
          ++lchl_;
        } else if (preL_to_preR_[parent]+1 == preL_to_preR_[i]) {
          ++rchl_;
        }
      }
    }

    // Sum up costs of deleting and inserting entire subtrees.
    // Reverse the node index. Here, we need traverse nodes bottom-up.
    node_for_sum = tree_size_ - i - 1;
    parent_for_sum = preL_to_parent_[node_for_sum];
    // Update myself.
    preL_to_sumDelCost_[node_for_sum] += c_.del(preL_to_node_[node_for_sum]);
    preL_to_sumInsCost_[node_for_sum] += c_.ins(preL_to_node_[node_for_sum]);
    if (parent_for_sum > -1) {
      // Update my parent.
      preL_to_sumDelCost_[parent_for_sum] += preL_to_sumDelCost_[node_for_sum];
      preL_to_sumInsCost_[parent_for_sum] += preL_to_sumInsCost_[node_for_sum];
    }
  }

  current_leaf = -1;
  // [TODO] Merge with the other loop. Assume different traversal.
  for(int i = 0; i < tree_size_; ++i) {
    preR_to_ln_[i] = current_leaf;
    if(is_leaf(preR_to_preL_[i])) {
      current_leaf = i;
    }
  }
}

template <typename Label, typename CostModel>
bool APTEDNodeIndexer<Label, CostModel>::is_leaf(int node) {
  return preL_to_size_[node] == 1;
}

template <typename Label, typename CostModel>
int APTEDNodeIndexer<Label, CostModel>::get_current_node() {
  return current_node_;
};

template <typename Label, typename CostModel>
void APTEDNodeIndexer<Label, CostModel>::set_current_node(int preorder) {
  current_node_ = preorder;
};

template <typename Label, typename CostModel>
int APTEDNodeIndexer<Label, CostModel>::preL_to_lld(int preL) {
  return postL_to_preL_[postL_to_lld_[preL_to_postL_[preL]]];
}

template <typename Label, typename CostModel>
int APTEDNodeIndexer<Label, CostModel>::preL_to_rld(int preL) {
  return postR_to_preL_[postR_to_rld_[preL_to_postR_[preL]]];
}

template <typename Label, typename CostModel>
const node::Node<Label>& APTEDNodeIndexer<Label, CostModel>::postL_to_node(int postL) {
  return preL_to_node_[postL_to_preL_[postL]];
}

template <typename Label, typename CostModel>
const node::Node<Label>& APTEDNodeIndexer<Label, CostModel>::postR_to_node(int postR) {
  return preL_to_node_[postR_to_preL_[postR]];
}

template <typename Label, typename CostModel>
APTED<Label, CostModel>::APTED() : c_() {}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::apted_ted(const node::Node<Label>& t1, const node::Node<Label>& t2) {
  APTEDNodeIndexer<Label, CostModel> ni_1(t1);
  APTEDNodeIndexer<Label, CostModel> ni_2(t2);
  input_size_1_ = ni_1.tree_size_;//get_size();
  input_size_2_ = ni_2.tree_size_;//get_size();
  return apted_ted(ni_1, ni_2);
}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::apted_ted(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2) {
  
  // Determine the optimal strategy for the distance computation.
  // Use the heuristic from [2, Section 5.3].
  // TODO: Implement compute_opt_strategy_postR.
  // if (ni_1.lchl_ < ni_1.rchl_) {
    delta_ = compute_opt_strategy_postL(ni_1, ni_2);
  // } else {
  //   delta_ = compute_opt_strategy_postR(ni_1, ni_2);
  // }
  // Initialise structures for distance computation.
  ted_init(ni_1, ni_2);
  // Compute the distance.
  return gted(ni_1, ni_2);
}

template <typename Label, typename CostModel>
data_structures::Matrix<double> APTED<Label, CostModel>::compute_opt_strategy_postL(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2) {
  int size1 = ni_1.tree_size_;
  int size2 = ni_2.tree_size_;
  data_structures::Matrix<double> strategy(size1, size2);
  std::vector<std::shared_ptr<std::vector<double>>> cost1_L(size1);
  std::vector<std::shared_ptr<std::vector<double>>> cost1_R(size1);
  std::vector<std::shared_ptr<std::vector<double>>> cost1_I(size1);
  std::vector<double> cost2_L(size2);
  std::vector<double> cost2_R(size2);
  std::vector<double> cost2_I(size2);
  std::vector<int> cost2_path(size2);
  std::shared_ptr<std::vector<double>> leafRow(new std::vector<double>(size2));
  int pathIDOffset = size1;
  double minCost = std::numeric_limits<double>::infinity();
  int strategyPath = -1;

  std::vector<int>& pre2size1 = ni_1.preL_to_size_;
  std::vector<int>& pre2size2 = ni_2.preL_to_size_;
  std::vector<int>& pre2descSum1 = ni_1.preL_to_desc_sum_;
  std::vector<int>& pre2descSum2 = ni_2.preL_to_desc_sum_;
  std::vector<int>& pre2krSum1 = ni_1.preL_to_kr_sum_;
  std::vector<int>& pre2krSum2 = ni_2.preL_to_kr_sum_;
  std::vector<int>& pre2revkrSum1 = ni_1.preL_to_rev_kr_sum_;
  std::vector<int>& pre2revkrSum2 = ni_2.preL_to_rev_kr_sum_;
  std::vector<int>& preL_to_preR_1 = ni_1.preL_to_preR_;
  std::vector<int>& preL_to_preR_2 = ni_2.preL_to_preR_;
  std::vector<int>& preR_to_preL_1 = ni_1.preR_to_preL_;
  std::vector<int>& preR_to_preL_2 = ni_2.preR_to_preL_;
  std::vector<int>& pre2parent1 = ni_1.preL_to_parent_;
  std::vector<int>& pre2parent2 = ni_2.preL_to_parent_;
  std::vector<bool>& nodeType_L_1 = ni_1.preL_to_node_type_L_;
  std::vector<bool>& nodeType_L_2 = ni_2.preL_to_node_type_L_;
  std::vector<bool>& nodeType_R_1 = ni_1.preL_to_node_type_R_;
  std::vector<bool>& nodeType_R_2 = ni_2.preL_to_node_type_R_;

  std::vector<int>& preL_to_postL_1 = ni_1.preL_to_postL_;
  std::vector<int>& preL_to_postL_2 = ni_2.preL_to_postL_;

  std::vector<int>& postL_to_preL_1 = ni_1.postL_to_preL_;
  std::vector<int>& postL_to_preL_2 = ni_2.postL_to_preL_;

  int size_v = -1;
  int parent_v_preL = -1;
  int parent_w_preL = -1;
  int parent_w_postL = -1;
  int size_w = -1;
  int parent_v_postL = -1;
  int leftPath_v;
  int rightPath_v;
  std::shared_ptr<std::vector<double>> cost_Lpointer_v;
  std::shared_ptr<std::vector<double>> cost_Rpointer_v;
  std::shared_ptr<std::vector<double>> cost_Ipointer_v;
  // std::vector<double>* strategypointer_v;
  std::shared_ptr<std::vector<double>> cost_Lpointer_parent_v;
  std::shared_ptr<std::vector<double>> cost_Rpointer_parent_v;
  std::shared_ptr<std::vector<double>> cost_Ipointer_parent_v;
  // std::vector<double>* strategypointer_parent_v;
  int krSum_v;
  int revkrSum_v;
  int descSum_v;
  bool is_v_leaf;

  int v_in_preL;
  int w_in_preL;

  std::stack<std::shared_ptr<std::vector<double>>> rowsToReuse_L;
  std::stack<std::shared_ptr<std::vector<double>>> rowsToReuse_R;
  std::stack<std::shared_ptr<std::vector<double>>> rowsToReuse_I;

  for(int v = 0; v < size1; ++v) {
    v_in_preL = postL_to_preL_1[v];

    is_v_leaf = ni_1.is_leaf(v_in_preL);
    parent_v_preL = pre2parent1[v_in_preL];

    if (parent_v_preL != -1) {
      parent_v_postL = preL_to_postL_1[parent_v_preL];
    }

    // strategypointer_v = strategy[v_in_preL];

    size_v = pre2size1[v_in_preL];
    leftPath_v = -(preR_to_preL_1[preL_to_preR_1[v_in_preL] + size_v - 1] + 1);// this is the left path's ID which is the leftmost leaf node: l-r_preorder(r-l_preorder(v) + |Fv| - 1)
    rightPath_v = v_in_preL + size_v - 1 + 1; // this is the right path's ID which is the rightmost leaf node: l-r_preorder(v) + |Fv| - 1
    krSum_v = pre2krSum1[v_in_preL];
    revkrSum_v = pre2revkrSum1[v_in_preL];
    descSum_v = pre2descSum1[v_in_preL];

    if(is_v_leaf) {
      cost1_L[v] = leafRow;
      cost1_R[v] = leafRow;
      cost1_I[v] = leafRow;
      for(int i = 0; i < size2; ++i) {
        // strategypointer_v[postL_to_preL_2[i]] = v_in_preL;
        strategy.at(v_in_preL, postL_to_preL_2[i]) = v_in_preL;
      }
    }

    cost_Lpointer_v = cost1_L[v];
    cost_Rpointer_v = cost1_R[v];
    cost_Ipointer_v = cost1_I[v];

    if(parent_v_preL != -1 && cost1_L[parent_v_postL] == nullptr) {
      if (rowsToReuse_L.empty()) {
        cost1_L[parent_v_postL] = std::make_shared<std::vector<double>>(size2);
        cost1_R[parent_v_postL] = std::make_shared<std::vector<double>>(size2);
        cost1_I[parent_v_postL] = std::make_shared<std::vector<double>>(size2);
      } else {
        cost1_L[parent_v_postL] = rowsToReuse_L.top();
        cost1_R[parent_v_postL] = rowsToReuse_R.top();
        cost1_I[parent_v_postL] = rowsToReuse_I.top();
        rowsToReuse_L.pop();
        rowsToReuse_R.pop();
        rowsToReuse_I.pop();
      }
    }

    if (parent_v_preL != -1) {
      cost_Lpointer_parent_v = cost1_L[parent_v_postL];
      cost_Rpointer_parent_v = cost1_R[parent_v_postL];
      cost_Ipointer_parent_v = cost1_I[parent_v_postL];
      // strategypointer_parent_v = strategy[parent_v_preL];
    }

    // Arrays.fill(cost2_L, 0L);
    std::fill(cost2_L.begin(),cost2_L.end(), 0);
    // Arrays.fill(cost2_R, 0L);
    std::fill(cost2_R.begin(),cost2_R.end(), 0);
    // Arrays.fill(cost2_I, 0L);
    std::fill(cost2_I.begin(),cost2_I.end(), 0);
    // Arrays.fill(cost2_path, 0);
    std::fill(cost2_path.begin(),cost2_path.end(), 0);

    for(int w = 0; w < size2; ++w) {
      w_in_preL = postL_to_preL_2[w];

      parent_w_preL = pre2parent2[w_in_preL];
      if (parent_w_preL != -1) {
        parent_w_postL = preL_to_postL_2[parent_w_preL];
      }

      size_w = pre2size2[w_in_preL];
      if (ni_2.is_leaf(w_in_preL)) {
        cost2_L[w] = 0;
        cost2_R[w] = 0;
        cost2_I[w] = 0;
        cost2_path[w] = w_in_preL;
      }
      minCost = std::numeric_limits<double>::infinity();
      strategyPath = -1;
      double tmpCost = std::numeric_limits<double>::infinity();

      if (size_v <= 1 || size_w <= 1) { // USE NEW SINGLE_PATH FUNCTIONS FOR SMALL SUBTREES
        minCost = std::max(size_v, size_w);
      } else {
        tmpCost = size_v * pre2krSum2[w_in_preL] + (*cost_Lpointer_v)[w];
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = leftPath_v;
        }
        tmpCost = size_v * pre2revkrSum2[w_in_preL] + (*cost_Rpointer_v)[w];
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = rightPath_v;
        }
        tmpCost = size_v * pre2descSum2[w_in_preL] + (*cost_Ipointer_v)[w];
        if (tmpCost < minCost) {
          minCost = tmpCost;
          // strategyPath = (int)strategypointer_v[w_in_preL] + 1;
          strategyPath = (int)strategy.at(v_in_preL, w_in_preL) + 1;
        }
        tmpCost = size_w * krSum_v + cost2_L[w];
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = -(preR_to_preL_2[preL_to_preR_2[w_in_preL] + size_w - 1] + pathIDOffset + 1);
        }
        tmpCost = size_w * revkrSum_v + cost2_R[w];
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = w_in_preL + size_w - 1 + pathIDOffset + 1;
        }
        tmpCost = size_w * descSum_v + cost2_I[w];
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = cost2_path[w] + pathIDOffset + 1;
        }
      }

      if (parent_v_preL != -1) {
        (*cost_Rpointer_parent_v)[w] += minCost;
        tmpCost = -minCost + (*cost1_I[v])[w];
        if (tmpCost < (*cost1_I[parent_v_postL])[w]) {
          (*cost_Ipointer_parent_v)[w] = tmpCost;
          // strategypointer_parent_v[w_in_preL] = strategypointer_v[w_in_preL];
        }
        if (nodeType_R_1[v_in_preL]) {
          (*cost_Ipointer_parent_v)[w] += (*cost_Rpointer_parent_v)[w];
          (*cost_Rpointer_parent_v)[w] += (*cost_Rpointer_v)[w] - minCost;
        }
        if (nodeType_L_1[v_in_preL]) {
          (*cost_Lpointer_parent_v)[w] += (*cost_Lpointer_v)[w];
        } else {
          (*cost_Lpointer_parent_v)[w] += minCost;
        }
      }
      if (parent_w_preL != -1) {
        cost2_R[parent_w_postL] += minCost;
        tmpCost = -minCost + cost2_I[w];
        if (tmpCost < cost2_I[parent_w_postL]) {
          cost2_I[parent_w_postL] = tmpCost;
          cost2_path[parent_w_postL] = cost2_path[w];
        }
        if (nodeType_R_2[w_in_preL]) {
          cost2_I[parent_w_postL] += cost2_R[parent_w_postL];
          cost2_R[parent_w_postL] += cost2_R[w] - minCost;
        }
        if (nodeType_L_2[w_in_preL]) {
          cost2_L[parent_w_postL] += cost2_L[w];
        } else {
          cost2_L[parent_w_postL] += minCost;
        }
      }
      // strategypointer_v[w_in_preL] = strategyPath;
      strategy.at(v_in_preL, w_in_preL) = strategyPath;
    }

    if (!ni_1.is_leaf(v_in_preL)) {
      // Arrays.fill(cost1_L[v], 0);
      std::fill(cost1_L[v]->begin(), cost1_L[v]->end(), 0);
      // Arrays.fill(cost1_R[v], 0);
      std::fill(cost1_R[v]->begin(), cost1_R[v]->end(), 0);
      // Arrays.fill(cost1_I[v], 0);
      std::fill(cost1_I[v]->begin(), cost1_I[v]->end(), 0);
      rowsToReuse_L.push(cost1_L[v]);
      rowsToReuse_R.push(cost1_R[v]);
      rowsToReuse_I.push(cost1_I[v]);
      // NOTE: The pointers under cost1_L[v] do not have to be nulled.
    }

  }
  
  std::cout << "strategy[0][0] = " << strategy.read_at(0, 0) << std::endl;
  return strategy;
};

template <typename Label, typename CostModel>
data_structures::Matrix<double> APTED<Label, CostModel>::compute_opt_strategy_postR(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2) {
  int size1 = ni_1.tree_size_;
  int size2 = ni_2.tree_size_;
  data_structures::Matrix<double> strategy(size1, size2);
  return strategy;
};


template <typename Label, typename CostModel>
void APTED<Label, CostModel>::ted_init(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2) {
  // Reset the subproblems counter.
  subproblem_counter_ = 0;
  // Initialize arrays.
  int max_size = std::max(input_size_1_, input_size_2_) + 1;
  // TODO: Move q initialisation to spfA.
  q_.resize(max_size);
  // TODO: Do not use fn and ft arrays [1, Section 8.4].
  fn_.resize(max_size + 1);
  ft_.resize(max_size + 1);
  // Compute subtree distances without the root nodes when one of subtrees
  // is a single node.
  int size_x = -1;
  int size_y = -1;
  // int parent_x = -1;
  // int parent_y = -1;
  // Loop over the nodes in reversed left-to-right preorder.
  for(int x = 0; x < input_size_1_; ++x) {
    size_x = ni_1.preL_to_size_[x];
    // parent_x = ni_1.preL_to_parent_[x];
    for(int y = 0; y < input_size_2_; ++y) {
      size_y = ni_2.preL_to_size_[y];
      // parent_y = ni_2.preL_to_parent_[y];
      // Set values in delta based on the sums of deletion and insertion
      // costs. Substract the costs for root nodes.
      // In this method we don't have to verify the order of the input trees
      // because it is equal to the original.
      if (size_x == 1 && size_y == 1) {
        delta_.at(x, y) = 0.0;
      } else if (size_x == 1) {
        delta_.at(x, y) = ni_2.preL_to_sumInsCost_[y] - c_.ins(ni_2.preL_to_node_[y]);
      } else if (size_y == 1) {
        delta_.at(x, y) = ni_1.preL_to_sumDelCost_[x] - c_.del(ni_1.preL_to_node_[x]);
      }
    }
  }
}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::gted(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2) {
  int currentSubtree1 = ni_1.get_current_node();
  int currentSubtree2 = ni_2.get_current_node();
  int subtreeSize1 = ni_1.preL_to_size_[currentSubtree1];
  int subtreeSize2 = ni_2.preL_to_size_[currentSubtree2];

  // Use spf1.
  if ((subtreeSize1 == 1 || subtreeSize2 == 1)) {
    return spf1(ni_1, currentSubtree1, ni_2, currentSubtree2);
  }

  int strategyPathID = (int)delta_.read_at(currentSubtree1, currentSubtree2);

  int strategyPathType = -1;
  int currentPathNode = std::abs(strategyPathID) - 1;
  int pathIDOffset = ni_1.tree_size_;

  int parent = -1;
  if(currentPathNode < pathIDOffset) {
    strategyPathType = get_strategy_path_type(strategyPathID, pathIDOffset, currentSubtree1, subtreeSize1);
    while((parent = ni_1.preL_to_parent_[currentPathNode]) >= currentSubtree1) {
      auto& ai = ni_1.preL_to_children_[parent];
      int k = ai.size();
      for(int i = 0; i < k; ++i) {
        int child = ai[i];
        if(child != currentPathNode) {
          ni_1.set_current_node(child);
          gted(ni_1, ni_2);
        }
      }
      currentPathNode = parent;
    }
    // TODO: Move this property away from node indexer and pass directly to spfs.
    ni_1.set_current_node(currentSubtree1);

    // Pass to spfs a boolean that says says if the order of input subtrees
    // has been swapped compared to the order of the initial input trees.
    // Used for accessing delta array and deciding on the edit operation
    // [1, Section 3.4].
    if (strategyPathType == 0) {
      return spfL(ni_1, ni_2, false);
    }
    if (strategyPathType == 1) {
      return spfR(ni_1, ni_2, false);
    }
    return spfA(ni_1, ni_2, std::abs(strategyPathID) - 1, strategyPathType, false);
  }

  currentPathNode -= pathIDOffset;
  strategyPathType = get_strategy_path_type(strategyPathID, pathIDOffset, currentSubtree2, subtreeSize2);
  while((parent = ni_2.preL_to_parent_[currentPathNode]) >= currentSubtree2) {
    auto& ai1 = ni_2.preL_to_children_[parent];
    int l = ai1.size();
    for(int j = 0; j < l; ++j) {
      int child = ai1[j];
      if(child != currentPathNode) {
        ni_2.set_current_node(child);
        gted(ni_1, ni_2);
      }
    }
    currentPathNode = parent;
  }
  // TODO: Move this property away from node indexer and pass directly to spfs.
  ni_2.set_current_node(currentSubtree2);

  // Pass to spfs a boolean that says says if the order of input subtrees
  // has been swapped compared to the order of the initial input trees. Used
  // for accessing delta array and deciding on the edit operation
  // [1, Section 3.4].
  if (strategyPathType == 0) {
    return spfL(ni_2, ni_1, true);
  }
  if (strategyPathType == 1) {
    return spfR(ni_2, ni_1, true);
  }
  return spfA(ni_2, ni_1, std::abs(strategyPathID) - pathIDOffset - 1, strategyPathType, true);
}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::spf1(APTEDNodeIndexer<Label, CostModel>& ni_1, int subtreeRootNode1, APTEDNodeIndexer<Label, CostModel>& ni_2, int subtreeRootNode2) {
  int subtreeSize1 = ni_1.preL_to_size_[subtreeRootNode1];
  int subtreeSize2 = ni_2.preL_to_size_[subtreeRootNode2];
  if (subtreeSize1 == 1 && subtreeSize2 == 1) {
    const node::Node<Label>& n1 = ni_1.preL_to_node_[subtreeRootNode1];
    const node::Node<Label>& n2 = ni_2.preL_to_node_[subtreeRootNode2];
    double maxCost = c_.del(n1) + c_.ins(n2);
    double renCost = c_.ren(n1, n2);
    return renCost < maxCost ? renCost : maxCost;
  }
  if (subtreeSize1 == 1) {
    const node::Node<Label>& n1 = ni_1.preL_to_node_[subtreeRootNode1];
    double cost = ni_2.preL_to_sumInsCost_[subtreeRootNode2];
    double maxCost = cost + c_.del(n1);
    double minRenMinusIns = cost;
    double nodeRenMinusIns = 0;
    for (int i = subtreeRootNode2; i < subtreeRootNode2 + subtreeSize2; i++) {
      const node::Node<Label>& n2 = ni_2.preL_to_node_[i];
      nodeRenMinusIns = c_.ren(n1, n2) - c_.ins(n2);
      if (nodeRenMinusIns < minRenMinusIns) {
        minRenMinusIns = nodeRenMinusIns;
      }
    }
    cost += minRenMinusIns;
    return cost < maxCost ? cost : maxCost;
  }
  if (subtreeSize2 == 1) {
    const node::Node<Label>& n2 = ni_2.preL_to_node_[subtreeRootNode2];
    double cost = ni_1.preL_to_sumDelCost_[subtreeRootNode1];
    double maxCost = cost + c_.ins(n2);
    double minRenMinusDel = cost;
    double nodeRenMinusDel = 0;
    for (int i = subtreeRootNode1; i < subtreeRootNode1 + subtreeSize1; i++) {
      const node::Node<Label>& n1 = ni_1.preL_to_node_[i];
      nodeRenMinusDel = c_.ren(n1, n2) - c_.del(n1);
      if (nodeRenMinusDel < minRenMinusDel) {
        minRenMinusDel = nodeRenMinusDel;
      }
    }
    cost += minRenMinusDel;
    return cost < maxCost ? cost : maxCost;
  }
  return -1;
}

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::spfA(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, int pathID, int pathType, bool treesSwapped) {
  std::vector<std::reference_wrapper<const node::Node<Label>>>& it2nodes = ni_2.preL_to_node_;
  // const node::Node<Label>& lFNode;
  std::vector<int>& it1sizes = ni_1.preL_to_size_;
  std::vector<int>& it2sizes = ni_2.preL_to_size_;
  std::vector<int>& it1parents = ni_1.preL_to_parent_;
  std::vector<int>& it2parents = ni_2.preL_to_parent_;
  std::vector<int>& it1preL_to_preR = ni_1.preL_to_preR_;
  std::vector<int>& it2preL_to_preR = ni_2.preL_to_preR_;
  std::vector<int>& it1preR_to_preL = ni_1.preR_to_preL_;
  std::vector<int>& it2preR_to_preL = ni_2.preR_to_preL_;
  int currentSubtreePreL1 = ni_1.get_current_node();
  int currentSubtreePreL2 = ni_2.get_current_node();

  // Variables to incrementally sum up the forest sizes.
  int currentForestSize1 = 0;
  int currentForestSize2 = 0;
  int tmpForestSize1 = 0;
  // Variables to incrementally sum up the forest cost.
  double currentForestCost1 = 0;
  double currentForestCost2 = 0;
  double tmpForestCost1 = 0;

  int subtreeSize2 = ni_2.preL_to_size_[currentSubtreePreL2];
  int subtreeSize1 = ni_1.preL_to_size_[currentSubtreePreL1];
  data_structures::Matrix<double> t(subtreeSize2+1, subtreeSize2+1);
  data_structures::Matrix<double> s(subtreeSize1+1, subtreeSize2+1);
  double minCost = -1;
  // sp1, sp2 and sp3 correspond to three elements of the minimum in the
  // recursive formula [1, Figure 12].
  double sp1 = 0;
  double sp2 = 0;
  double sp3 = 0;
  int startPathNode = -1;
  int endPathNode = pathID;
  int it1PreLoff = endPathNode;
  int it2PreLoff = currentSubtreePreL2;
  int it1PreRoff = it1preL_to_preR[endPathNode];
  int it2PreRoff = it2preL_to_preR[it2PreLoff];
  // variable declarations which were inside the loops
  int rFlast,lFlast,endPathNode_in_preR,startPathNode_in_preR,parent_of_endPathNode,parent_of_endPathNode_in_preR, lFfirst,rFfirst,rGlast,rGfirst,lGfirst,rG_in_preL,rGminus1_in_preL,parent_of_rG_in_preL,lGlast,lF_in_preR,lFSubtreeSize, lGminus1_in_preR,parent_of_lG,parent_of_lG_in_preR,rF_in_preL,rFSubtreeSize, rGfirst_in_preL;
  bool leftPart,rightPart,fForestIsTree,lFIsConsecutiveNodeOfCurrentPathNode,lFIsLeftSiblingOfCurrentPathNode, rFIsConsecutiveNodeOfCurrentPathNode,rFIsRightSiblingOfCurrentPathNode;
  
  std::size_t sp1spointer,sp2spointer,sp3spointer,sp3deltapointer,swritepointer,sp1tpointer,sp3tpointer;
  
  // These variables store the id of the source (which array) of looking up
  // elements of the minimum in the recursive formula [1, Figures 12,13].
  int sp1source,sp3source;
  // Loop A [1, Algorithm 3] - walk up the path.
  while (endPathNode >= currentSubtreePreL1) {
    it1PreLoff = endPathNode;
    it1PreRoff = it1preL_to_preR[endPathNode];
    rFlast = -1;
    lFlast = -1;
    endPathNode_in_preR = it1preL_to_preR[endPathNode];
    startPathNode_in_preR = startPathNode == -1 ? std::numeric_limits<int>::max() : it1preL_to_preR[startPathNode];
    parent_of_endPathNode = it1parents[endPathNode];
    parent_of_endPathNode_in_preR = parent_of_endPathNode == -1 ? std::numeric_limits<int>::max() : it1preL_to_preR[parent_of_endPathNode];
    if (startPathNode - endPathNode > 1) {
      leftPart = true;
    } else {
      leftPart = false;
    }
    if (startPathNode >= 0 && startPathNode_in_preR - endPathNode_in_preR > 1) {
      rightPart = true;
    } else {
      rightPart = false;
    }
    // Deal with nodes to the left of the path.
    if (pathType == 1 || (pathType == 2 && leftPart)) {
      if (startPathNode == -1) {
        rFfirst = endPathNode_in_preR;
        lFfirst = endPathNode;
      } else {
        rFfirst = startPathNode_in_preR;
        lFfirst = startPathNode - 1;
      }
      if (!rightPart) {
        rFlast = endPathNode_in_preR;
      }
      rGlast = it2preL_to_preR[currentSubtreePreL2];
      rGfirst = (rGlast + subtreeSize2) - 1;
      lFlast = rightPart ? endPathNode + 1 : endPathNode;
      fn_[fn_.size() - 1] = -1;
      for (int i = currentSubtreePreL2; i < currentSubtreePreL2 + subtreeSize2; ++i) {
          fn_[i] = -1;
          ft_[i] = -1;
      }
      // Store the current size and cost of forest in F.
      tmpForestSize1 = currentForestSize1;
      tmpForestCost1 = currentForestCost1;
      // Loop B [1, Algoritm 3] - for all nodes in G (right-hand input tree).
      for (int rG = rGfirst; rG >= rGlast; --rG) {
        lGfirst = it2preR_to_preL[rG];
        rG_in_preL = it2preR_to_preL[rG];
        rGminus1_in_preL = rG <= it2preL_to_preR[currentSubtreePreL2] ? std::numeric_limits<int>::max() : it2preR_to_preL[rG - 1];
        parent_of_rG_in_preL = it2parents[rG_in_preL];
        // This if statement decides on the last lG node for Loop D [1, Algorithm 3];
        if (pathType == 1){
          if (lGfirst == currentSubtreePreL2 || rGminus1_in_preL != parent_of_rG_in_preL) {
            lGlast = lGfirst;
          } else {
            lGlast = it2parents[lGfirst]+1;
          }
        } else {
          lGlast = lGfirst == currentSubtreePreL2 ? lGfirst : currentSubtreePreL2+1;
        }
        updateFnArray(ni_2.preL_to_ln_[lGfirst], lGfirst, currentSubtreePreL2);
        updateFtArray(ni_2.preL_to_ln_[lGfirst], lGfirst);
        int rF = rFfirst;
        // Reset size and cost of the forest in F.
        currentForestSize1 = tmpForestSize1;
        currentForestCost1 = tmpForestCost1;
        // Loop C [1, Algorithm 3] - for all nodes to the left of the path node.
        for (int lF = lFfirst; lF >= lFlast; --lF) {
          // This if statement fixes rF node.
          if (lF == lFlast && !rightPart) {
            rF = rFlast;
          }
          const node::Node<Label>& lFNode = ni_1.preL_to_node_[lF];
          // Increment size and cost of F forest by node lF.
          ++currentForestSize1;
          currentForestCost1 += (treesSwapped ? c_.ins(lFNode) : c_.del(lFNode)); // USE COST MODEL - sum up deletion cost of a forest.
          // Reset size and cost of forest in G to subtree G_lGfirst.
          currentForestSize2 = it2sizes[lGfirst];
          currentForestCost2 = (treesSwapped ? ni_2.preL_to_sumDelCost_[lGfirst] : ni_2.preL_to_sumInsCost_[lGfirst]); // USE COST MODEL - reset to subtree insertion cost.
          lF_in_preR = it1preL_to_preR[lF];
          fForestIsTree = lF_in_preR == rF;
          lFSubtreeSize = it1sizes[lF];
          lFIsConsecutiveNodeOfCurrentPathNode = startPathNode - lF == 1;
          lFIsLeftSiblingOfCurrentPathNode = lF + lFSubtreeSize == startPathNode;
          
          // TODO: Move these to where matrix accessed.
          sp1spointer = (lF + 1) - it1PreLoff;//s[(lF + 1) - it1PreLoff];
          sp2spointer = lF - it1PreLoff;//s[lF - it1PreLoff];
          sp3spointer = 0;//s[0];
          // TODO: Be careful with 0 in the line below.
          sp3deltapointer = treesSwapped ? 0 : lF;//treesSwapped ? null : delta[lF];
          swritepointer = lF - it1PreLoff;//s[lF - it1PreLoff];
          
          sp1source = 1; // Search sp1 value in s array by default.
          sp3source = 1; // Search second part of sp3 value in s array by default.
          if (fForestIsTree) { // F_{lF,rF} is a tree.
            if (lFSubtreeSize == 1) { // F_{lF,rF} is a single node.
              sp1source = 3;
            } else if (lFIsConsecutiveNodeOfCurrentPathNode) { // F_{lF,rF}-lF is the path node subtree.
              sp1source = 2;
            }
            sp3 = 0;
            sp3source = 2;
          } else {
            if (lFIsConsecutiveNodeOfCurrentPathNode) {
              sp1source = 2;
            }
            sp3 = currentForestCost1 - (treesSwapped ? ni_1.preL_to_sumInsCost_[lF] : ni_1.preL_to_sumDelCost_[lF]); // USE COST MODEL - Delete F_{lF,rF}-F_lF.
            if (lFIsLeftSiblingOfCurrentPathNode) {
              sp3source = 3;
            }
          }
          if (sp3source == 1) {
            // TODO: Move these to where matrix accessed.
            sp3spointer = (lF + lFSubtreeSize) - it1PreLoff;//s[(lF + lFSubtreeSize) - it1PreLoff];
          }
          // Go to first lG.
          int lG = lGfirst;
          // currentForestSize2++;
          // sp1, sp2, sp3 -- Done here for the first node in Loop D. It differs for consecutive nodes.
          // sp1 -- START
          switch(sp1source) {
            case 1: sp1 = s.read_at(sp1spointer, lG - it2PreLoff); break;
            case 2: sp1 = t.read_at(lG - it2PreLoff, rG - it2PreRoff); break;
            case 3: sp1 = currentForestCost2; break; // USE COST MODEL - Insert G_{lG,rG}.
          }
          sp1 += (treesSwapped ? c_.ins(lFNode) : c_.del(lFNode));// USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
          // sp1 -- END
          minCost = sp1; // Start with sp1 as minimal value.
          // sp2 -- START
          if (currentForestSize2 == 1) { // G_{lG,rG} is a single node.
            sp2 = currentForestCost1; // USE COST MODEL - Delete F_{lF,rF}.
          } else { // G_{lG,rG} is a tree.
            sp2 = q_[lF];
          }
          sp2 += (treesSwapped ? c_.del(it2nodes[lG]) : c_.ins(it2nodes[lG]));// USE COST MODEL - Insert lG, leftmost root node in G_{lG,rG}.
          if (sp2 < minCost) { // Check if sp2 is minimal value.
            minCost = sp2;
          }
          // sp2 -- END
          // sp3 -- START
          if (sp3 < minCost) {
            // TODO: Matrix pointer here.
            sp3 += treesSwapped ? delta_.read_at(lG, lF) : delta_.read_at(sp3deltapointer, lG);
            if (sp3 < minCost) {
              sp3 += (treesSwapped ? c_.ren(it2nodes[lG], lFNode) : c_.ren(lFNode, it2nodes[lG])); // USE COST MODEL - Rename the leftmost root nodes in F_{lF,rF} and G_{lG,rG}.
              if(sp3 < minCost) {
                minCost = sp3;
              }
            }
          }
          // sp3 -- END
          // TODO: Matrix pointer here.
          s.at(swritepointer, lG - it2PreLoff) = minCost;
          // Go to next lG.
          lG = ft_[lG];
          ++subproblem_counter_;
          // Loop D [1, Algorithm 3] - for all nodes to the left of rG.
          while (lG >= lGlast) {
            // Increment size and cost of G forest by node lG.
            ++currentForestSize2;
            currentForestCost2 += (treesSwapped ? c_.del(it2nodes[lG]) : c_.ins(it2nodes[lG]));
            switch(sp1source) {
              // TODO: Matrix pointer here.
              case 1: sp1 = s.read_at(sp1spointer, lG - it2PreLoff) + (treesSwapped ? c_.ins(lFNode) : c_.del(lFNode)); break; // USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
              case 2: sp1 = t.read_at(lG - it2PreLoff, rG - it2PreRoff) + (treesSwapped ? c_.ins(lFNode) : c_.del(lFNode)); break; // USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
              case 3: sp1 = currentForestCost2 + (treesSwapped ? c_.ins(lFNode) : c_.del(lFNode)); break; // USE COST MODEL - Insert G_{lG,rG} and elete lF, leftmost root node in F_{lF,rF}.
            }
            // TODO: Matrix pointer here.
            sp2 = s.read_at(sp2spointer, fn_[lG] - it2PreLoff) + (treesSwapped ? c_.del(it2nodes[lG]) : c_.ins(it2nodes[lG])); // USE COST MODEL - Insert lG, leftmost root node in G_{lG,rG}.
            minCost = sp1;
            if(sp2 < minCost) {
              minCost = sp2;
            }
            // TODO: Matrix pointer here.
            sp3 = treesSwapped ? delta_.read_at(lG, lF) : delta_.read_at(sp3deltapointer, lG);
            if (sp3 < minCost) {
              switch(sp3source) {
                // TODO: Matrix pointer here.
                  case 1: sp3 += s.read_at(sp3spointer, fn_[(lG + it2sizes[lG]) - 1] - it2PreLoff); break;
                  case 2: sp3 += currentForestCost2 - (treesSwapped ? ni_2.preL_to_sumDelCost_[lG] : ni_2.preL_to_sumInsCost_[lG]); break; // USE COST MODEL - Insert G_{lG,rG}-G_lG.
                  case 3: sp3 += t.read_at(fn_[(lG + it2sizes[lG]) - 1] - it2PreLoff, rG - it2PreRoff); break;
              }
              if (sp3 < minCost) {
                sp3 += (treesSwapped ? c_.ren(it2nodes[lG], lFNode) : c_.ren(lFNode, it2nodes[lG])); // USE COST MODEL - Rename the leftmost root nodes in F_{lF,rF} and G_{lG,rG}.
                if (sp3 < minCost) {
                  minCost = sp3;
                }
              }
            }
            // TODO: Matrix pointer here.
            s.at(swritepointer, lG - it2PreLoff) = minCost;
            lG = ft_[lG];
            ++subproblem_counter_;
          }
        }
        if (rGminus1_in_preL == parent_of_rG_in_preL) {
          if (!rightPart) {
            if (leftPart) {
              if (treesSwapped) {
                delta_.at(parent_of_rG_in_preL, endPathNode) = s.read_at((lFlast + 1) - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              } else {
                delta_.at(endPathNode, parent_of_rG_in_preL) = s.read_at((lFlast + 1) - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              }
            }
            if (endPathNode > 0 && endPathNode == parent_of_endPathNode + 1 && endPathNode_in_preR == parent_of_endPathNode_in_preR + 1) {
              if (treesSwapped) {
                delta_.at(parent_of_rG_in_preL, parent_of_endPathNode) = s.read_at(lFlast - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              } else {
                delta_.at(parent_of_endPathNode, parent_of_rG_in_preL) = s.read_at(lFlast - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              }
            }
          }
          for (int lF = lFfirst; lF >= lFlast; --lF) {
            q_[lF] = s.read_at(lF - it1PreLoff, (parent_of_rG_in_preL + 1) - it2PreLoff);
          }
        }
        // TODO: first pointers can be precomputed
        for (int lG = lGfirst; lG >= lGlast; lG = ft_[lG]) {
          t.at(lG - it2PreLoff, rG - it2PreRoff) = s.read_at(lFlast - it1PreLoff, lG - it2PreLoff);
        }
      }
    }
    // Deal with nodes to the right of the path.
    if (pathType == 0 || (pathType == 2 && rightPart) || (pathType == 2 && !leftPart && !rightPart)) {
      if (startPathNode == -1) {
        lFfirst = endPathNode;
        rFfirst = it1preL_to_preR[endPathNode];
      } else {
        rFfirst = it1preL_to_preR[startPathNode] - 1;
        lFfirst = endPathNode + 1;
      }
      lFlast = endPathNode;
      lGlast = currentSubtreePreL2;
      lGfirst = (lGlast + subtreeSize2) - 1;
      rFlast = it1preL_to_preR[endPathNode];
      fn_[fn_.size() - 1] = -1;
      for (int i = currentSubtreePreL2; i < currentSubtreePreL2 + subtreeSize2; i++){
        fn_[i] = -1;
        ft_[i] = -1;
      }
      // Store size and cost of the current forest in F.
      tmpForestSize1 = currentForestSize1;
      tmpForestCost1 = currentForestCost1;
      // Loop B' [1, Algorithm 3] - for all nodes in G.
      for (int lG = lGfirst; lG >= lGlast; lG--) {
        rGfirst = it2preL_to_preR[lG];
        updateFnArray(ni_2.preR_to_ln_[rGfirst], rGfirst, it2preL_to_preR[currentSubtreePreL2]);
        updateFtArray(ni_2.preR_to_ln_[rGfirst], rGfirst);
        int lF = lFfirst;
        lGminus1_in_preR = lG <= currentSubtreePreL2 ? std::numeric_limits<int>::max() : it2preL_to_preR[lG - 1];
        parent_of_lG = it2parents[lG];
        parent_of_lG_in_preR = parent_of_lG == -1 ? -1 : it2preL_to_preR[parent_of_lG];
        // Reset size and cost of forest if F.
        currentForestSize1 = tmpForestSize1;
        currentForestCost1 = tmpForestCost1;
        if (pathType == 0) {
          if (lG == currentSubtreePreL2) {
            rGlast = rGfirst;
          } else if (ni_2.preL_to_children_[parent_of_lG][0] != lG) {
            rGlast = rGfirst;
          } else {
            rGlast = it2preL_to_preR[parent_of_lG]+1;
          }
        } else {
          rGlast = rGfirst == it2preL_to_preR[currentSubtreePreL2] ? rGfirst : it2preL_to_preR[currentSubtreePreL2];
        }
        // Loop C' [1, Algorithm 3] - for all nodes to the right of the path node.
        for (int rF = rFfirst; rF >= rFlast; --rF) {
          if (rF == rFlast) {
            lF = lFlast;
          }
          rF_in_preL = it1preR_to_preL[rF];
          // Increment size and cost of F forest by node rF.
          ++currentForestSize1;
          currentForestCost1 += (treesSwapped ? c_.ins(ni_1.preL_to_node_[rF_in_preL]) : c_.del(ni_1.preL_to_node_[rF_in_preL])); // USE COST MODEL - sum up deletion cost of a forest.
          // Reset size and cost of G forest to G_lG.
          currentForestSize2 = it2sizes[lG];
          currentForestCost2 = (treesSwapped ? ni_2.preL_to_sumDelCost_[lG] : ni_2.preL_to_sumInsCost_[lG]); // USE COST MODEL - reset to subtree insertion cost.
          rFSubtreeSize = it1sizes[rF_in_preL];
          if (startPathNode > 0) {
            rFIsConsecutiveNodeOfCurrentPathNode = startPathNode_in_preR - rF == 1;
            rFIsRightSiblingOfCurrentPathNode = rF + rFSubtreeSize == startPathNode_in_preR;
          } else {
            rFIsConsecutiveNodeOfCurrentPathNode = false;
            rFIsRightSiblingOfCurrentPathNode = false;
          }
          fForestIsTree = rF_in_preL == lF;
          const node::Node<Label>& rFNode = ni_1.preL_to_node_[rF_in_preL];
          sp1spointer = (rF + 1) - it1PreRoff;//s[(rF + 1) - it1PreRoff];
          sp2spointer = rF - it1PreRoff;//s[rF - it1PreRoff];
          sp3spointer = 0;//s[0];
          // TODO: Be careful with 0 in the line below.
          sp3deltapointer = treesSwapped ? 0 : rF_in_preL;//treesSwapped ? null : delta[rF_in_preL];
          swritepointer = rF - it1PreRoff;//s[rF - it1PreRoff];
          sp1tpointer = lG - it2PreLoff;//t[lG - it2PreLoff];
          sp3tpointer = lG - it2PreLoff;//t[lG - it2PreLoff];
          sp1source = 1;
          sp3source = 1;
          if (fForestIsTree) {
            if (rFSubtreeSize == 1) {
              sp1source = 3;
            } else if (rFIsConsecutiveNodeOfCurrentPathNode) {
              sp1source = 2;
            }
            sp3 = 0;
            sp3source = 2;
          } else {
            if (rFIsConsecutiveNodeOfCurrentPathNode) {
              sp1source = 2;
            }
            sp3 = currentForestCost1 - (treesSwapped ? ni_1.preL_to_sumInsCost_[rF_in_preL] : ni_1.preL_to_sumDelCost_[rF_in_preL]); // USE COST MODEL - Delete F_{lF,rF}-F_rF.
            if (rFIsRightSiblingOfCurrentPathNode) {
              sp3source = 3;
            }
          }
          if (sp3source == 1) {
            sp3spointer = (rF + rFSubtreeSize) - it1PreRoff;//s[(rF + rFSubtreeSize) - it1PreRoff];
          }
          if (currentForestSize2 == 1) {
            sp2 = currentForestCost1;// USE COST MODEL - Delete F_{lF,rF}.
          } else {
            sp2 = q_[rF];
          }
          int rG = rGfirst;
          rGfirst_in_preL = it2preR_to_preL[rGfirst];
          ++currentForestSize2;
          switch (sp1source) {
            case 1: sp1 = s.read_at(sp1spointer, rG - it2PreRoff); break;
            case 2: sp1 = t.read_at(sp1tpointer, rG - it2PreRoff); break;
            case 3: sp1 = currentForestCost2; break; // USE COST MODEL - Insert G_{lG,rG}.
          }
          sp1 += (treesSwapped ? c_.ins(rFNode) : c_.del(rFNode)); // USE COST MODEL - Delete rF.
          minCost = sp1;
          sp2 += (treesSwapped ? c_.del(it2nodes[rGfirst_in_preL]) : c_.ins(it2nodes[rGfirst_in_preL])); // USE COST MODEL - Insert rG.
          if (sp2 < minCost) {
            minCost = sp2;
          }
          if (sp3 < minCost) {
            sp3 += treesSwapped ? delta_.read_at(rGfirst_in_preL, rF_in_preL) : delta_.read_at(sp3deltapointer, rGfirst_in_preL);
            if (sp3 < minCost) {
              sp3 += (treesSwapped ? c_.ren(it2nodes[rGfirst_in_preL], rFNode) : c_.ren(rFNode, it2nodes[rGfirst_in_preL]));
              if (sp3 < minCost) {
                minCost = sp3;
              }
            }
          }
          s.at(swritepointer, rG - it2PreRoff) = minCost;
          rG = ft_[rG];
          ++subproblem_counter_;
          // Loop D' [1, Algorithm 3] - for all nodes to the right of lG;
          while (rG >= rGlast) {
            rG_in_preL = it2preR_to_preL[rG];
            // Increment size and cost of G forest by node rG.
            ++currentForestSize2;
            currentForestCost2 += (treesSwapped ? c_.del(it2nodes[rG_in_preL]) : c_.ins(it2nodes[rG_in_preL]));
            switch (sp1source) {
              case 1: sp1 = s.read_at(sp1spointer, rG - it2PreRoff) + (treesSwapped ? c_.ins(rFNode) : c_.del(rFNode)); break; // USE COST MODEL - Delete rF.
              case 2: sp1 = t.read_at(sp1tpointer, rG - it2PreRoff) + (treesSwapped ? c_.ins(rFNode) : c_.del(rFNode)); break; // USE COST MODEL - Delete rF.
              case 3: sp1 = currentForestCost2 + (treesSwapped ? c_.ins(rFNode) : c_.del(rFNode)); break; // USE COST MODEL - Insert G_{lG,rG} and delete rF.
            }
            sp2 = s.read_at(sp2spointer, fn_[rG] - it2PreRoff) + (treesSwapped ? c_.del(it2nodes[rG_in_preL]) : c_.ins(it2nodes[rG_in_preL])); // USE COST MODEL - Insert rG.
            minCost = sp1;
            if (sp2 < minCost) {
              minCost = sp2;
            }
            sp3 = treesSwapped ? delta_.read_at(rG_in_preL, rF_in_preL) : delta_.read_at(sp3deltapointer, rG_in_preL);
            if (sp3 < minCost) {
              switch (sp3source) {
                case 1: sp3 += s.read_at(sp3spointer, fn_[(rG + it2sizes[rG_in_preL]) - 1] - it2PreRoff); break;
                case 2: sp3 += currentForestCost2 - (treesSwapped ? ni_2.preL_to_sumDelCost_[rG_in_preL] : ni_2.preL_to_sumInsCost_[rG_in_preL]); break; // USE COST MODEL - Insert G_{lG,rG}-G_rG.
                case 3: sp3 += t.read_at(sp3tpointer, fn_[(rG + it2sizes[rG_in_preL]) - 1] - it2PreRoff); break;
              }
              if (sp3 < minCost) {
                sp3 += (treesSwapped ? c_.ren(it2nodes[rG_in_preL], rFNode) : c_.ren(rFNode, it2nodes[rG_in_preL])); // USE COST MODEL - Rename rF to rG.
                if (sp3 < minCost) {
                  minCost = sp3;
                }
              }
            }
            s.at(swritepointer, rG - it2PreRoff) = minCost;
            rG = ft_[rG];
            ++subproblem_counter_;
          }
        }
        if (lG > currentSubtreePreL2 && lG - 1 == parent_of_lG) {
          if (rightPart) {
            if (treesSwapped) {
              delta_.at(parent_of_lG, endPathNode) = s.read_at((rFlast + 1) - it1PreRoff, (lGminus1_in_preR + 1) - it2PreRoff);
            } else {
              delta_.at(endPathNode, parent_of_lG) = s.read_at((rFlast + 1) - it1PreRoff, (lGminus1_in_preR + 1) - it2PreRoff);
            }
          }
          if (endPathNode > 0 && endPathNode == parent_of_endPathNode + 1 && endPathNode_in_preR == parent_of_endPathNode_in_preR + 1) {
            if (treesSwapped) {
              delta_.at(parent_of_lG, parent_of_endPathNode) = s.read_at(rFlast - it1PreRoff, (lGminus1_in_preR + 1) - it2PreRoff);
            } else {
              delta_.at(parent_of_endPathNode, parent_of_lG) = s.read_at(rFlast - it1PreRoff, (lGminus1_in_preR + 1) - it2PreRoff);
            }
          }
          for (int rF = rFfirst; rF >= rFlast; --rF) {
            q_[rF] = s.read_at(rF - it1PreRoff, (parent_of_lG_in_preR + 1) - it2PreRoff);
          }
        }
        // TODO: first pointers can be precomputed
        for (int rG = rGfirst; rG >= rGlast; rG = ft_[rG]) {
          t.at(lG - it2PreLoff, rG - it2PreRoff) = s.read_at(rFlast - it1PreRoff, rG - it2PreRoff);
        }
      }
    }
    // Walk up the path by one node.
    startPathNode = endPathNode;
    endPathNode = it1parents[endPathNode];
  }
  return minCost;
};

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::spfL(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, bool treesSwapped) {
  // Initialise the array to store the keyroot nodes in the right-hand input
  // subtree.
  std::vector<int> keyRoots(ni_2.preL_to_size_[ni_2.get_current_node()]);
  // Arrays.fill(keyRoots, -1);
  std::fill(keyRoots.begin(), keyRoots.end(), -1);
  // Get the leftmost leaf node of the right-hand input subtree.
  int pathID = ni_2.preL_to_lld(ni_2.get_current_node());
  // Calculate the keyroot nodes in the right-hand input subtree.
  // firstKeyRoot is the index in keyRoots of the first keyroot node that
  // we have to process. We need this index because keyRoots array is larger
  // than the number of keyroot nodes.
  int firstKeyRoot = computeKeyRoots(ni_2, ni_2.get_current_node(), pathID, keyRoots, 0);
  // Initialise an array to store intermediate distances for subforest pairs.
  data_structures::Matrix<double> forestdist(ni_1.preL_to_size_[ni_1.get_current_node()]+1, ni_2.preL_to_size_[ni_2.get_current_node()]+1);
  // Compute the distances between pairs of keyroot nodes. In the left-hand
  // input subtree only the root is the keyroot. Thus, we compute the distance
  // between the left-hand input subtree and all keyroot nodes in the
  // right-hand input subtree.
  for (int i = firstKeyRoot-1; i >= 0; --i) {
    treeEditDist(ni_1, ni_2, ni_1.get_current_node(), keyRoots[i], forestdist, treesSwapped);
  }
  // Return the distance between the input subtrees.
  return forestdist.read_at(ni_1.preL_to_size_[ni_1.get_current_node()], ni_2.preL_to_size_[ni_2.get_current_node()]);
};

template <typename Label, typename CostModel>
int APTED<Label, CostModel>::computeKeyRoots(APTEDNodeIndexer<Label, CostModel>& ni_2, int subtreeRootNode, int pathID, std::vector<int>& keyRoots, int index) {
  // The subtreeRootNode is a keyroot node. Add it to keyRoots.
  keyRoots[index] = subtreeRootNode;
  // Increment the index to know where to store the next keyroot node.
  ++index;
  // Walk up the left path starting with the leftmost leaf of subtreeRootNode,
  // until the child of subtreeRootNode.
  int pathNode = pathID;
  while (pathNode > subtreeRootNode) {
    int parent = ni_2.preL_to_parent_[pathNode];
    // For each sibling to the right of pathNode, execute this method recursively.
    // Each right sibling of pathNode is a keyroot node.
    for (int child : ni_2.preL_to_children_[parent]) {
      // Execute computeKeyRoots recursively for the new subtree rooted at child and child's leftmost leaf node.
      if (child != pathNode) index = computeKeyRoots(ni_2, child, ni_2.preL_to_lld(child), keyRoots, index);
    }
    // Walk up.
    pathNode = parent;
  }
  return index;
};

template <typename Label, typename CostModel>
void APTED<Label, CostModel>::treeEditDist(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, int it1subtree, int it2subtree, data_structures::Matrix<double>& forestdist, bool treesSwapped) {
  // Translate input subtree root nodes to left-to-right postorder.
  int i = ni_1.preL_to_postL_[it1subtree];
  int j = ni_2.preL_to_postL_[it2subtree];
  // We need to offset the node ids for accessing forestdist array which has
  // indices from 0 to subtree size. However, the subtree node indices do not
  // necessarily start with 0.
  // Whenever the original left-to-right postorder id has to be accessed, use
  // i+ioff and j+joff.
  int ioff = ni_1.postL_to_lld_[i] - 1;
  int joff = ni_2.postL_to_lld_[j] - 1;
  // Variables holding costs of each minimum element.
  float da = 0;
  float db = 0;
  float dc = 0;
  // Initialize forestdist array with deletion and insertion costs of each
  // relevant subforest.
  forestdist.at(0, 0) = 0;
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    forestdist.at(i1, 0) = forestdist.read_at(i1 - 1, 0) + (treesSwapped ? c_.ins(ni_1.postL_to_node(i1 + ioff)) : c_.del(ni_1.postL_to_node(i1 + ioff))); // USE COST MODEL - delete i1.
  }
  for (int j1 = 1; j1 <= j - joff; ++j1) {
    forestdist.at(0, j1) = forestdist.read_at(0, j1 - 1) + (treesSwapped ? c_.del(ni_2.postL_to_node(j1 + joff)) : c_.ins(ni_2.postL_to_node(j1 + joff))); // USE COST MODEL - insert j1.
  }
  // Fill in the remaining costs.
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    for (int j1 = 1; j1 <= j - joff; ++j1) {
      // Increment the number of subproblems.
      ++subproblem_counter_;
      // Calculate partial distance values for this subproblem.
      double u = (treesSwapped ? c_.ren(ni_2.postL_to_node(j1 + joff), ni_1.postL_to_node(i1 + ioff)) : c_.ren(ni_1.postL_to_node(i1 + ioff), ni_2.postL_to_node(j1 + joff))); // USE COST MODEL - rename i1 to j1.
      da = forestdist.read_at(i1 - 1, j1) + (treesSwapped ? c_.ins(ni_1.postL_to_node(i1 + ioff)) : c_.del(ni_1.postL_to_node(i1 + ioff))); // USE COST MODEL - delete i1.
      db = forestdist.read_at(i1, j1 - 1) + (treesSwapped ? c_.del(ni_2.postL_to_node(j1 + joff)) : c_.ins(ni_2.postL_to_node(j1 + joff))); // USE COST MODEL - insert j1.
      // If current subforests are subtrees.
      if (ni_1.postL_to_lld_[i1 + ioff] == ni_1.postL_to_lld_[i] && ni_2.postL_to_lld_[j1 + joff] == ni_2.postL_to_lld_[j]) {
        dc = forestdist.read_at(i1 - 1, j1 - 1) + u;
        // Store the relevant distance value in delta array.
        if (treesSwapped) {
          delta_.at(ni_2.postL_to_preL_[j1 + joff], ni_1.postL_to_preL_[i1 + ioff]) = forestdist.read_at(i1 - 1, j1 - 1);
        } else {
          delta_.at(ni_1.postL_to_preL_[i1 + ioff], ni_2.postL_to_preL_[j1 + joff]) = forestdist.read_at(i1 - 1, j1 - 1);
        }
      } else {
        dc = forestdist.read_at(ni_1.postL_to_lld_[i1 + ioff] - 1 - ioff, ni_2.postL_to_lld_[j1 + joff] - 1 - joff) +
          (treesSwapped ? delta_.read_at(ni_2.postL_to_preL_[j1 + joff], ni_1.postL_to_preL_[i1 + ioff]) : delta_.read_at(ni_1.postL_to_preL_[i1 + ioff], ni_2.postL_to_preL_[j1 + joff])) + u;
      }
      // Calculate final minimum.
      forestdist.at(i1, j1) = da >= db ? db >= dc ? dc : db : da >= dc ? dc : da;
    }
  }
};

template <typename Label, typename CostModel>
double APTED<Label, CostModel>::spfR(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, bool treesSwapped) {
  // Initialise the array to store the keyroot nodes in the right-hand input
  // subtree.
  std::vector<int> revKeyRoots(ni_2.preL_to_size_[ni_2.get_current_node()]);
  // Arrays.fill(revKeyRoots, -1);
  std::fill(revKeyRoots.begin(), revKeyRoots.end(), -1);
  // Get the rightmost leaf node of the right-hand input subtree.
  int pathID = ni_2.preL_to_rld(ni_2.get_current_node());
  // Calculate the keyroot nodes in the right-hand input subtree.
  // firstKeyRoot is the index in keyRoots of the first keyroot node that
  // we have to process. We need this index because keyRoots array is larger
  // than the number of keyroot nodes.
  int firstKeyRoot = computeRevKeyRoots(ni_2, ni_2.get_current_node(), pathID, revKeyRoots, 0);
  // Initialise an array to store intermediate distances for subforest pairs.
  data_structures::Matrix<double> forestdist(ni_1.preL_to_size_[ni_1.get_current_node()]+1, ni_2.preL_to_size_[ni_2.get_current_node()]+1);
  // Compute the distances between pairs of keyroot nodes. In the left-hand
  // input subtree only the root is the keyroot. Thus, we compute the distance
  // between the left-hand input subtree and all keyroot nodes in the
  // right-hand input subtree.
  for (int i = firstKeyRoot-1; i >= 0; --i) {
    revTreeEditDist(ni_1, ni_2, ni_1.get_current_node(), revKeyRoots[i], forestdist, treesSwapped);
  }
  // Return the distance between the input subtrees.
  return forestdist.read_at(ni_1.preL_to_size_[ni_1.get_current_node()], ni_2.preL_to_size_[ni_2.get_current_node()]);
};

template <typename Label, typename CostModel>
int APTED<Label, CostModel>::computeRevKeyRoots(APTEDNodeIndexer<Label, CostModel>& ni_2, int subtreeRootNode, int pathID, std::vector<int>& revKeyRoots, int index) {
  // The subtreeRootNode is a keyroot node. Add it to keyRoots.
  revKeyRoots[index] = subtreeRootNode;
  // Increment the index to know where to store the next keyroot node.
  ++index;
  // Walk up the right path starting with the rightmost leaf of
  // subtreeRootNode, until the child of subtreeRootNode.
  int pathNode = pathID;
  while (pathNode > subtreeRootNode) {
    int parent = ni_2.preL_to_parent_[pathNode];
    // For each sibling to the left of pathNode, execute this method recursively.
    // Each left sibling of pathNode is a keyroot node.
    for (int child : ni_2.preL_to_children_[parent]) {
      // Execute computeRevKeyRoots recursively for the new subtree rooted at child and child's rightmost leaf node.
      if (child != pathNode) index = computeRevKeyRoots(ni_2, child, ni_2.preL_to_rld(child), revKeyRoots, index);
    }
    // Walk up.
    pathNode = parent;
  }
  return index;
};

template <typename Label, typename CostModel>
void APTED<Label, CostModel>::revTreeEditDist(APTEDNodeIndexer<Label, CostModel>& ni_1, APTEDNodeIndexer<Label, CostModel>& ni_2, int it1subtree, int it2subtree, data_structures::Matrix<double>& forestdist, bool treesSwapped) {
  // Translate input subtree root nodes to right-to-left postorder.
  int i = ni_1.preL_to_postR_[it1subtree];
  int j = ni_2.preL_to_postR_[it2subtree];
  // We need to offset the node ids for accessing forestdist array which has
  // indices from 0 to subtree size. However, the subtree node indices do not
  // necessarily start with 0.
  // Whenever the original right-to-left postorder id has to be accessed, use
  // i+ioff and j+joff.
  int ioff = ni_1.postR_to_rld_[i] - 1;
  int joff = ni_2.postR_to_rld_[j] - 1;
  // Variables holding costs of each minimum element.
  float da = 0;
  float db = 0;
  float dc = 0;
  // Initialize forestdist array with deletion and insertion costs of each
  // relevant subforest.
  forestdist.at(0,0) = 0;
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    forestdist.at(i1, 0) = forestdist.read_at(i1 - 1, 0) + (treesSwapped ? c_.ins(ni_1.postR_to_node(i1 + ioff)) : c_.del(ni_1.postR_to_node(i1 + ioff))); // USE COST MODEL - delete i1.
  }
  for (int j1 = 1; j1 <= j - joff; ++j1) {
    forestdist.at(0, j1) = forestdist.read_at(0, j1 - 1) + (treesSwapped ? c_.del(ni_2.postR_to_node(j1 + joff)) : c_.ins(ni_2.postR_to_node(j1 + joff))); // USE COST MODEL - insert j1.
  }
  // Fill in the remaining costs.
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    for (int j1 = 1; j1 <= j - joff; ++j1) {
      // Increment the number of subproblems.
      ++subproblem_counter_;
      // Calculate partial distance values for this subproblem.
      float u = (treesSwapped ? c_.ren(ni_2.postR_to_node(j1 + joff), ni_1.postR_to_node(i1 + ioff)) : c_.ren(ni_1.postR_to_node(i1 + ioff), ni_2.postR_to_node(j1 + joff))); // USE COST MODEL - rename i1 to j1.
      da = forestdist.read_at(i1 - 1, j1) + (treesSwapped ? c_.ins(ni_1.postR_to_node(i1 + ioff)) : c_.del(ni_1.postR_to_node(i1 + ioff))); // USE COST MODEL - delete i1.
      db = forestdist.read_at(i1, j1 - 1) + (treesSwapped ? c_.del(ni_2.postR_to_node(j1 + joff)) : c_.ins(ni_2.postR_to_node(j1 + joff))); // USE COST MODEL - insert j1.
      // If current subforests are subtrees.
      if (ni_1.postR_to_rld_[i1 + ioff] == ni_1.postR_to_rld_[i] && ni_2.postR_to_rld_[j1 + joff] == ni_2.postR_to_rld_[j]) {
        dc = forestdist.read_at(i1 - 1, j1 - 1) + u;
        // Store the relevant distance value in delta array.
        if (treesSwapped) {
          delta_.at(ni_2.postR_to_preL_[j1+joff], ni_1.postR_to_preL_[i1+ioff]) = forestdist.read_at(i1 - 1, j1 - 1);
        } else {
          delta_.at(ni_1.postR_to_preL_[i1+ioff], ni_2.postR_to_preL_[j1+joff]) = forestdist.read_at(i1 - 1, j1 - 1);
        }
      } else {
        dc = forestdist.read_at(ni_1.postR_to_rld_[i1 + ioff] - 1 - ioff, ni_2.postR_to_rld_[j1 + joff] - 1 - joff) +
          (treesSwapped ? delta_.read_at(ni_2.postR_to_preL_[j1 + joff], ni_1.postR_to_preL_[i1 + ioff]) : delta_.read_at(ni_1.postR_to_preL_[i1 + ioff], ni_2.postR_to_preL_[j1 + joff])) + u;
      }
      // Calculate final minimum.
      forestdist.at(i1, j1) = da >= db ? db >= dc ? dc : db : da >= dc ? dc : da;
    }
  }
};


template <typename Label, typename CostModel>
int APTED<Label, CostModel>::get_strategy_path_type(int pathIDWithPathIDOffset, int pathIDOffset, int currentRootNodePreL, int currentSubtreeSize) {
  // if (Integer.signum(pathIDWithPathIDOffset) == -1) {
  if (pathIDWithPathIDOffset < 0) {
    return 0;
  }
  int pathID = std::abs(pathIDWithPathIDOffset) - 1;
  if (pathID >= pathIDOffset) {
    pathID = pathID - pathIDOffset;
  }
  if (pathID == (currentRootNodePreL + currentSubtreeSize) - 1) {
    return 1;
  }
  return 2;
};

template <typename Label, typename CostModel>
void APTED<Label, CostModel>::updateFnArray(int lnForNode, int node, int currentSubtreePreL) {
  if (lnForNode >= currentSubtreePreL) {
    fn_[node] = fn_[lnForNode];
    fn_[lnForNode] = node;
  } else {
    fn_[node] = fn_[fn_.size() - 1];
    fn_[fn_.size() - 1] = node;
  }
};

template <typename Label, typename CostModel>
void APTED<Label, CostModel>::updateFtArray(int lnForNode, int node) {
  ft_[node] = lnForNode;
  if(fn_[node] > -1) {
    ft_[fn_[node]] = node;
  }
};

// template <typename Label, typename CostModel>
// const TestItems& APTED<Label, CostModel>::get_test_items(const node::Node<Label>& t1) const {
//
// }

#endif // TREE_SIMILARITY_TED_APTED_IMPL_H
