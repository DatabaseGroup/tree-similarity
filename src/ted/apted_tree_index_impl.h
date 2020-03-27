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

/// Contains the implementation of the APTED algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double APTEDTreeIndex<CostModel, TreeIndex>::ted(
    const TreeIndex& t1, const TreeIndex& t2) {
  // Determine the optimal strategy for the distance computation.
  // Use the heuristic from [2, Section 5.3].
  // TODO: Implement compute_opt_strategy_postR.
  // if (ni_1.lchl_ < ni_1.rchl_) {
    delta_ = compute_opt_strategy_postL(t1, t2);
  // } else {
  //   delta_ = compute_opt_strategy_postR(ni_1, ni_2);
  // }
  // Initialise structures for distance computation.
  ted_init(t1, t2);
  // Compute the distance.
  return gted(t1, 0, t2, 0);
}

template <typename CostModel, typename TreeIndex>
data_structures::Matrix<double> APTEDTreeIndex<CostModel, TreeIndex>::compute_opt_strategy_postL(const TreeIndex& t1, const TreeIndex& t2) {
  const int size1 = t1.tree_size_;
  const int size2 = t2.tree_size_;
  data_structures::Matrix<double> strategy(size1, size2);
  std::vector<std::shared_ptr<std::vector<long long int>>> cost1_L(size1);
  std::vector<std::shared_ptr<std::vector<long long int>>> cost1_R(size1);
  std::vector<std::shared_ptr<std::vector<long long int>>> cost1_I(size1);
  std::vector<long long int> cost2_L(size2);
  std::vector<long long int> cost2_R(size2);
  std::vector<long long int> cost2_I(size2);
  std::vector<double> cost2_path(size2);
  std::shared_ptr<std::vector<long long int>> leafRow(new std::vector<long long int>(size2));
  double pathIDOffset = static_cast<double>(size1);
  long long int minCost = std::numeric_limits<long long int>::max();
  double strategyPath = -1.0;

  const std::vector<int>& pre2size1 = t1.prel_to_size_;
  const std::vector<int>& pre2size2 = t2.prel_to_size_;
  const std::vector<long long int>& pre2descSum1 = t1.prel_to_cost_all_;
  const std::vector<long long int>& pre2descSum2 = t2.prel_to_cost_all_;
  const std::vector<long long int>& pre2krSum1 = t1.prel_to_cost_left_;
  const std::vector<long long int>& pre2krSum2 = t2.prel_to_cost_left_;
  const std::vector<long long int>& pre2revkrSum1 = t1.prel_to_cost_right_;
  const std::vector<long long int>& pre2revkrSum2 = t2.prel_to_cost_right_;
  const std::vector<int>& preL_to_preR_1 = t1.prel_to_prer_;
  const std::vector<int>& preL_to_preR_2 = t2.prel_to_prer_;
  const std::vector<int>& preR_to_preL_1 = t1.prer_to_prel_;
  const std::vector<int>& preR_to_preL_2 = t2.prer_to_prel_;
  const std::vector<int>& pre2parent1 = t1.prel_to_parent_;
  const std::vector<int>& pre2parent2 = t2.prel_to_parent_;
  const std::vector<bool>& nodeType_L_1 = t1.prel_to_type_left_;
  const std::vector<bool>& nodeType_L_2 = t2.prel_to_type_left_;
  const std::vector<bool>& nodeType_R_1 = t1.prel_to_type_right_;
  const std::vector<bool>& nodeType_R_2 = t2.prel_to_type_right_;

  const std::vector<int>& preL_to_postL_1 = t1.prel_to_postl_;
  const std::vector<int>& preL_to_postL_2 = t2.prel_to_postl_;

  const std::vector<int>& postL_to_preL_1 = t1.postl_to_prel_;
  const std::vector<int>& postL_to_preL_2 = t2.postl_to_prel_;

  int size_v = -1;
  int parent_v_preL = -1;
  int parent_w_preL = -1;
  int parent_w_postL = -1;
  int size_w = -1;
  int parent_v_postL = -1;
  double leftPath_v;
  double rightPath_v;
  std::shared_ptr<std::vector<long long int>> cost_Lpointer_v;
  std::shared_ptr<std::vector<long long int>> cost_Rpointer_v;
  std::shared_ptr<std::vector<long long int>> cost_Ipointer_v;
  // std::vector<double>* strategypointer_v;
  std::size_t strategypointer_v;
  std::shared_ptr<std::vector<long long int>> cost_Lpointer_parent_v;
  std::shared_ptr<std::vector<long long int>> cost_Rpointer_parent_v;
  std::shared_ptr<std::vector<long long int>> cost_Ipointer_parent_v;
  // std::vector<double>* strategypointer_parent_v;
  std::size_t strategypointer_parent_v;
  long long int krSum_v;
  long long int revkrSum_v;
  long long int descSum_v;
  bool is_v_leaf;

  int v_in_preL;
  int w_in_preL;

  std::stack<std::shared_ptr<std::vector<long long int>>> rowsToReuse_L;
  std::stack<std::shared_ptr<std::vector<long long int>>> rowsToReuse_R;
  std::stack<std::shared_ptr<std::vector<long long int>>> rowsToReuse_I;

  for(int v = 0; v < size1; ++v) {
    v_in_preL = postL_to_preL_1[v];

    is_v_leaf = t1.prel_to_size_[v_in_preL] == 1;
    parent_v_preL = pre2parent1[v_in_preL];

    if (parent_v_preL != -1) {
      parent_v_postL = preL_to_postL_1[parent_v_preL];
    }

    // strategypointer_v = strategy[v_in_preL];
    strategypointer_v = v_in_preL;

    size_v = pre2size1[v_in_preL];
    leftPath_v = static_cast<double>(-(preR_to_preL_1[preL_to_preR_1[v_in_preL] + size_v - 1] + 1));// this is the left path's ID which is the leftmost leaf node: l-r_preorder(r-l_preorder(v) + |Fv| - 1)
    rightPath_v = static_cast<double>(v_in_preL + size_v - 1 + 1); // this is the right path's ID which is the rightmost leaf node: l-r_preorder(v) + |Fv| - 1

    krSum_v = pre2krSum1[v_in_preL];
    revkrSum_v = pre2revkrSum1[v_in_preL];
    descSum_v = pre2descSum1[v_in_preL];

    if(is_v_leaf) {
      cost1_L[v] = leafRow;
      cost1_R[v] = leafRow;
      cost1_I[v] = leafRow;
      for(int i = 0; i < size2; ++i) {
        // strategypointer_v[postL_to_preL_2[i]] = v_in_preL;
        strategy.at(strategypointer_v, postL_to_preL_2[i]) = static_cast<double>(v_in_preL);
      }
    }

    cost_Lpointer_v = cost1_L[v];
    cost_Rpointer_v = cost1_R[v];
    cost_Ipointer_v = cost1_I[v];

    if(parent_v_preL != -1 && cost1_L[parent_v_postL] == nullptr) {
      if (rowsToReuse_L.empty()) {
        cost1_L[parent_v_postL] = std::make_shared<std::vector<long long int>>(size2);
        cost1_R[parent_v_postL] = std::make_shared<std::vector<long long int>>(size2);
        cost1_I[parent_v_postL] = std::make_shared<std::vector<long long int>>(size2);
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
      strategypointer_parent_v = parent_v_preL;
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
      if (t2.prel_to_size_[w_in_preL] == 1) {
        cost2_L[w] = 0;
        cost2_R[w] = 0;
        cost2_I[w] = 0;
        cost2_path[w] = w_in_preL;
      }
      minCost = std::numeric_limits<long long int>::max();
      strategyPath = -1.0;
      long long int tmpCost = std::numeric_limits<long long int>::max();

      if (size_v <= 1 || size_w <= 1) { // USE NEW SINGLE_PATH FUNCTIONS FOR SMALL SUBTREES
        minCost = std::max(size_v, size_w);
      } else {
        tmpCost = size_v * pre2krSum2[w_in_preL] + (*cost_Lpointer_v)[w];
        // std::cout << "tmpCost [1] = " << tmpCost << std::endl;
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = leftPath_v;
          // std::cout << "strategyPath [1] = " << strategyPath << std::endl;
        }
        tmpCost = size_v * pre2revkrSum2[w_in_preL] + (*cost_Rpointer_v)[w];
        // std::cout << "tmpCost [2] = " << tmpCost << std::endl;
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = rightPath_v;
          // std::cout << "strategyPath [2] = " << strategyPath << std::endl;
        }
        tmpCost = size_v * pre2descSum2[w_in_preL] + (*cost_Ipointer_v)[w];
        // std::cout << "tmpCost [3] = " << tmpCost << std::endl;
        if (tmpCost < minCost) {
          minCost = tmpCost;
          // strategyPath = (int)strategypointer_v[w_in_preL] + 1;
          strategyPath = strategy.at(strategypointer_v, w_in_preL) + 1.0;
          // std::cout << "strategyPath [3] = " << strategyPath << std::endl;
        }
        tmpCost = size_w * krSum_v + cost2_L[w];
        // std::cout << "tmpCost [4] = " << tmpCost << std::endl;
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = static_cast<double>(-(preR_to_preL_2[preL_to_preR_2[w_in_preL] + size_w - 1] + pathIDOffset + 1.0));
          // std::cout << "strategyPath [4] = " << strategyPath << std::endl;
        }
        tmpCost = size_w * revkrSum_v + cost2_R[w];
        // std::cout << "tmpCost [5] = " << tmpCost << std::endl;
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = static_cast<double>(w_in_preL + size_w - 1) + pathIDOffset + 1.0;
          // std::cout << "strategyPath [5] = " << strategyPath << std::endl;
        }
        tmpCost = size_w * descSum_v + cost2_I[w];
        // std::cout << "tmpCost [6] = " << tmpCost << std::endl;
        if (tmpCost < minCost) {
          minCost = tmpCost;
          strategyPath = cost2_path[w] + pathIDOffset + 1.0;
          // std::cout << "strategyPath [6] = " << strategyPath << std::endl;
        }
      }

      // std::cout << "minCost = " << minCost << std::endl;

      if (parent_v_preL != -1) {
        (*cost_Rpointer_parent_v)[w] += minCost;
        tmpCost = -minCost + (*cost1_I[v])[w];
        if (tmpCost < (*cost1_I[parent_v_postL])[w]) {
          (*cost_Ipointer_parent_v)[w] = tmpCost;
          // strategypointer_parent_v[w_in_preL] = strategypointer_v[w_in_preL];
          strategy.at(strategypointer_parent_v, w_in_preL) = strategy.read_at(strategypointer_v, w_in_preL);
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
      strategy.at(strategypointer_v, w_in_preL) = strategyPath;
      // std::cout << "strategy[" << strategypointer_v << "," << w_in_preL << "] = " << strategy.read_at(0, 0) << std::endl;
    }

    if (!(t1.prel_to_size_[v_in_preL] == 1)) {
      // Arrays.fill(cost1_L[v], 0);
      std::fill(cost1_L[v]->begin(), cost1_L[v]->end(), 0.0);
      // Arrays.fill(cost1_R[v], 0);
      std::fill(cost1_R[v]->begin(), cost1_R[v]->end(), 0.0);
      // Arrays.fill(cost1_I[v], 0);
      std::fill(cost1_I[v]->begin(), cost1_I[v]->end(), 0.0);
      rowsToReuse_L.push(cost1_L[v]);
      rowsToReuse_R.push(cost1_R[v]);
      rowsToReuse_I.push(cost1_I[v]);
      // NOTE: The pointers under cost1_L[v] do not have to be nulled.
    }

  }
  
  // std::cout << "strategy[0][0] = " << strategy.read_at(0, 0) << std::endl;
  return strategy;
}

template <typename CostModel, typename TreeIndex>
data_structures::Matrix<double> APTEDTreeIndex<CostModel, TreeIndex>::compute_opt_strategy_postR(const TreeIndex& t1, const TreeIndex& t2) {
  const int size1 = t1.tree_size_;
  const int size2 = t2.tree_size_;
  data_structures::Matrix<double> strategy(size1, size2);
  return strategy;
}

template <typename CostModel, typename TreeIndex>
void APTEDTreeIndex<CostModel, TreeIndex>::ted_init(const TreeIndex& t1, const TreeIndex& t2) {
  // Reset the subproblems counter.
  subproblem_counter_ = 0;
  // Initialize arrays.
  int max_size = std::max(t1.tree_size_, t2.tree_size_) + 1;
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
  for(int x = 0; x < t1.tree_size_; ++x) {
    size_x = t1.prel_to_size_[x];
    // parent_x = ni_1.preL_to_parent_[x];
    for(int y = 0; y < t2.tree_size_; ++y) {
      size_y = t2.prel_to_size_[y];
      // parent_y = ni_2.preL_to_parent_[y];
      // Set values in delta based on the sums of deletion and insertion
      // costs. Substract the costs for root nodes.
      // In this method we don't have to verify the order of the input trees
      // because it is equal to the original.
      if (size_x == 1 && size_y == 1) {
        delta_.at(x, y) = 0.0;
      } else if (size_x == 1) {
        delta_.at(x, y) = t2.prel_to_subtree_ins_cost_[y] - c_.ins(t2.prel_to_label_id_[y]);
      } else if (size_y == 1) {
        delta_.at(x, y) = t1.prel_to_subtree_del_cost_[x] - c_.del(t1.prel_to_label_id_[x]);
      }
    }
  }
}

template <typename CostModel, typename TreeIndex>
double APTEDTreeIndex<CostModel, TreeIndex>::gted(const TreeIndex& t1,
    int t1_current_subtree, const TreeIndex& t2,
    int t2_current_subtree) {
  const int currentSubtree1 = t1_current_subtree;
  const int currentSubtree2 = t2_current_subtree;
  const int subtreeSize1 = t1.prel_to_size_[currentSubtree1];
  const int subtreeSize2 = t2.prel_to_size_[currentSubtree2];

  // std::cout << "gted(" << currentSubtree1 << "," << currentSubtree2 << ")" << std::endl;

  double result = 0;

  // Use spf1.
  if ((subtreeSize1 == 1 || subtreeSize2 == 1)) {
    result = spf1(t1, currentSubtree1, t2, currentSubtree2);
    // std::cerr << "spf1(" << currentSubtree1 << "," << currentSubtree2 << ") = " << result << std::endl;
    return result;
  }

  int strategyPathID = static_cast<int>(delta_.read_at(currentSubtree1, currentSubtree2));

  int strategyPathType = -1;
  int currentPathNode = std::abs(strategyPathID) - 1;
  int pathIDOffset = t1.tree_size_;

  int parent = -1;
  if(currentPathNode < pathIDOffset) {
    strategyPathType = get_strategy_path_type(strategyPathID, pathIDOffset, currentSubtree1, subtreeSize1);
    parent = t1.prel_to_parent_[currentPathNode];
    while(parent >= currentSubtree1) {
      auto& ai = t1.prel_to_children_[parent];
      int k = ai.size();
      for(int i = 0; i < k; ++i) {
        int child = ai[i];
        if(child != currentPathNode) {
          // t1.set_current_node(child);
          gted(t1, child, t2, t2_current_subtree);
        }
      }
      currentPathNode = parent;
      parent = t1.prel_to_parent_[currentPathNode];
    }
    // TODO: Move this property away from node indexer and pass directly to spfs.
    // t1.set_current_node(currentSubtree1);

    // Pass to spfs a boolean that says says if the order of input subtrees
    // has been swapped compared to the order of the initial input trees.
    // Used for accessing delta array and deciding on the edit operation
    // [1, Section 3.4].
    if (strategyPathType == 0) {
      result = spfL(t1, t1_current_subtree, t2, t2_current_subtree, false);
      // std::cerr << "spfL(" << t1_current_subtree << "," << t2_current_subtree << ") = " << result << std::endl;
      return result;
    }
    if (strategyPathType == 1) {
      result = spfR(t1, t1_current_subtree, t2, t2_current_subtree, false);
      // std::cerr << "spfR(" << t1_current_subtree << "," << t2_current_subtree << ") = " << result << std::endl;
      return result;
    }
    result = spfA(t1, t1_current_subtree, t2, t2_current_subtree,
        std::abs(strategyPathID) - 1, strategyPathType, false);
    // std::cerr << "spfA(" << t1_current_subtree << "," << t2_current_subtree << ") = " << result << std::endl;
    return result;
  }

  currentPathNode -= pathIDOffset;
  strategyPathType = get_strategy_path_type(strategyPathID, pathIDOffset, currentSubtree2, subtreeSize2);
  parent = t2.prel_to_parent_[currentPathNode];
  while(parent >= currentSubtree2) {
    // std::cerr << "parent = " << parent << std::endl;
    // std::cerr << "# parent's children = " << t2.prel_to_children_[parent].size() << std::endl;
    auto& ai1 = t2.prel_to_children_[parent];
    int l = ai1.size();
    for(int j = 0; j < l; ++j) {
      int child = ai1[j];
      if(child != currentPathNode) {
        // t2.set_current_node(child);
        gted(t1, t1_current_subtree, t2, child);
      }
    }
    currentPathNode = parent;
    parent = t2.prel_to_parent_[currentPathNode];
  }
  // TODO: Move this property away from node indexer and pass directly to spfs.
  // t2.set_current_node(currentSubtree2);

  // Pass to spfs a boolean that says says if the order of input subtrees
  // has been swapped compared to the order of the initial input trees. Used
  // for accessing delta array and deciding on the edit operation
  // [1, Section 3.4].
  if (strategyPathType == 0) {
    result = spfL(t2, t2_current_subtree, t1, t1_current_subtree, true);
    // std::cerr << "spfL(" << t1_current_subtree << "," << t2_current_subtree << ") = " << result << std::endl;
    return result;
  }
  if (strategyPathType == 1) {
    result = spfR(t2, t2_current_subtree, t1, t1_current_subtree, true);
    // std::cerr << "spfR(" << t1_current_subtree << "," << t2_current_subtree << ") = " << result << std::endl;
    return result;
  }
  result = spfA(t2, t2_current_subtree, t1, t1_current_subtree,
      std::abs(strategyPathID) - pathIDOffset - 1, strategyPathType, true);
  // std::cerr << "spfA(" << t1_current_subtree << "," << t2_current_subtree << ") = " << result << std::endl;
  return result;
}

template <typename CostModel, typename TreeIndex>
double APTEDTreeIndex<CostModel, TreeIndex>::spf1(const TreeIndex& t1,
    int subtreeRootNode1, const TreeIndex& t2, int subtreeRootNode2) {
  int subtreeSize1 = t1.prel_to_size_[subtreeRootNode1];
  int subtreeSize2 = t2.prel_to_size_[subtreeRootNode2];
  if (subtreeSize1 == 1 && subtreeSize2 == 1) {
    int label1 = t1.prel_to_label_id_[subtreeRootNode1];
    int label2 = t2.prel_to_label_id_[subtreeRootNode2];
    double maxCost = c_.del(label1) + c_.ins(label2);
    double renCost = c_.ren(label1, label2);
    // std::cout << "spf1 = " << (renCost < maxCost ? renCost : maxCost) << std::endl;
    return renCost < maxCost ? renCost : maxCost;
  }
  if (subtreeSize1 == 1) {
    int label1 = t1.prel_to_label_id_[subtreeRootNode1];
    double cost = t2.prel_to_subtree_ins_cost_[subtreeRootNode2];
    double maxCost = cost + c_.del(label1);
    double minRenMinusIns = cost;
    double nodeRenMinusIns = 0;
    for (int i = subtreeRootNode2; i < subtreeRootNode2 + subtreeSize2; ++i) {
      int label2 = t2.prel_to_label_id_[i];
      nodeRenMinusIns = c_.ren(label1, label2) - c_.ins(label2);
      if (nodeRenMinusIns < minRenMinusIns) {
        minRenMinusIns = nodeRenMinusIns;
      }
    }
    cost += minRenMinusIns;
    // std::cout << "spf1 = " << (cost < maxCost ? cost : maxCost) << std::endl;
    return cost < maxCost ? cost : maxCost;
  }
  if (subtreeSize2 == 1) {
    int label2 = t2.prel_to_label_id_[subtreeRootNode2];
    double cost = t1.prel_to_subtree_del_cost_[subtreeRootNode1];
    double maxCost = cost + c_.ins(label2);
    double minRenMinusDel = cost;
    double nodeRenMinusDel = 0;
    for (int i = subtreeRootNode1; i < subtreeRootNode1 + subtreeSize1; ++i) {
      int label1 = t1.prel_to_label_id_[i];
      nodeRenMinusDel = c_.ren(label1, label2) - c_.del(label1);
      if (nodeRenMinusDel < minRenMinusDel) {
        minRenMinusDel = nodeRenMinusDel;
      }
    }
    cost += minRenMinusDel;
    // std::cout << "spf1 = " << (cost < maxCost ? cost : maxCost) << std::endl;
    return cost < maxCost ? cost : maxCost;
  }
  // std::cout << "spf1 = -1" << std::endl;
  return -1;
}

template <typename CostModel, typename TreeIndex>
double APTEDTreeIndex<CostModel, TreeIndex>::spfA(const TreeIndex& t1,
    int t1_current_subtree, const TreeIndex& t2,
    int t2_current_subtree, int pathID, int pathType,
    bool treesSwapped) {
  const std::vector<int>& it2labels = t2.prel_to_label_id_;
  // const node::Node<Label>& lFNode;
  const std::vector<int>& it1sizes = t1.prel_to_size_;
  const std::vector<int>& it2sizes = t2.prel_to_size_;
  const std::vector<int>& it1parents = t1.prel_to_parent_;
  const std::vector<int>& it2parents = t2.prel_to_parent_;
  const std::vector<int>& it1preL_to_preR = t1.prel_to_prer_;
  const std::vector<int>& it2preL_to_preR = t2.prel_to_prer_;
  const std::vector<int>& it1preR_to_preL = t1.prer_to_prel_;
  const std::vector<int>& it2preR_to_preL = t2.prer_to_prel_;
  const int currentSubtreePreL1 = t1_current_subtree;//t1.get_current_node();
  const int currentSubtreePreL2 = t2_current_subtree;//t2.get_current_node();

  // Variables to incrementally sum up the forest sizes.
  int currentForestSize1 = 0;
  int currentForestSize2 = 0;
  int tmpForestSize1 = 0;
  // Variables to incrementally sum up the forest cost.
  double currentForestCost1 = 0;
  double currentForestCost2 = 0;
  double tmpForestCost1 = 0;

  const int subtreeSize2 = t2.prel_to_size_[currentSubtreePreL2];
  const int subtreeSize1 = t1.prel_to_size_[currentSubtreePreL1];
  data_structures::Matrix<double> t(subtreeSize2+1, subtreeSize2+1);
  data_structures::Matrix<double> s(subtreeSize1+1, subtreeSize2+1);
  
  // std::vector<double> q_(std::max(input_size_1_, input_size_2_) + 1);
  
  double minCost = -1;
  // sp1, sp2 and sp3 correspond to three elements of the minimum in the
  // recursive formula [1, Figure 12].
  double sp1 = 0;
  double sp2 = 0;
  double sp3 = 0;
  int startPathNode = -1; // dummy node on the bottom of the path
  int endPathNode = pathID;
  int it1PreLoff = endPathNode;
  int it2PreLoff = currentSubtreePreL2;
  int it1PreRoff = it1preL_to_preR[endPathNode];
  int it2PreRoff = it2preL_to_preR[it2PreLoff];
  // variable declarations which were inside the loops
  bool leftPart,rightPart,fForestIsTree,lFIsConsecutiveNodeOfCurrentPathNode,
    lFIsLeftSiblingOfCurrentPathNode, rFIsConsecutiveNodeOfCurrentPathNode,
    rFIsRightSiblingOfCurrentPathNode;
  
  int parent_of_endPathNode, startPathNode_in_preR, rGminus1_in_preL,
    rGlast, lGfirst, lGminus1_in_preR, parent_of_rG_in_preL, lFSubtreeSize,
    lFlast, lGlast, rGfirst, lF_in_preR, rFSubtreeSize, parent_of_lG,
    parent_of_lG_in_preR, rFlast, endPathNode_in_preR,
    parent_of_endPathNode_in_preR, lFfirst, rFfirst, rG_in_preL, rF_in_preL,
    rGfirst_in_preL;

  std::size_t sp1spointer,sp2spointer,sp3spointer,sp3deltapointer,swritepointer,sp1tpointer,sp3tpointer;
  
  // These variables store the id of the source (which array) of looking up
  // elements of the minimum in the recursive formula [1, Figures 12,13].
  int sp1source,sp3source;
  // Loop A [1, Algorithm 3] - walk up the path.
  while (endPathNode >= currentSubtreePreL1) {
    it1PreLoff = endPathNode;
    it1PreRoff = it1preL_to_preR[endPathNode];
    rFlast = -1; // is only reset here
    lFlast = -1; // is only reset here
    endPathNode_in_preR = it1preL_to_preR[endPathNode];
    startPathNode_in_preR = startPathNode == -1 ? std::numeric_limits<int>::max() : it1preL_to_preR[startPathNode];
    parent_of_endPathNode = it1parents[endPathNode];
    parent_of_endPathNode_in_preR = parent_of_endPathNode == -1 ? std::numeric_limits<int>::max() : it1preL_to_preR[parent_of_endPathNode];
    // TODO: Overflow problem again if startPathNode = max.
    // TODO: Double check what to do if the endPathNode is a leaf.
    if (startPathNode - endPathNode > 1) {
      leftPart = true;
    } else {
      leftPart = false;
    }
    if (startPathNode != -1 && startPathNode_in_preR - endPathNode_in_preR > 1) {
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
        rGminus1_in_preL = rG <= it2preL_to_preR[currentSubtreePreL2] ?
          std::numeric_limits<int>::max() : it2preR_to_preL[rG - 1];
        parent_of_rG_in_preL = it2parents[rG_in_preL];
        // This if statement decides on the last lG node for Loop D [1, Algorithm 3];
        if (pathType == 1){ // Right path.
          // `rGminus1_in_preL == parent_of_rG_in_preL` checks if rG is the rightmost child of its parent
          if (lGfirst == currentSubtreePreL2 || rGminus1_in_preL != parent_of_rG_in_preL) {
            lGlast = lGfirst;
          } else {
            lGlast = it2parents[lGfirst]+1;
          }
        } else {
          lGlast = lGfirst == currentSubtreePreL2 ? lGfirst : currentSubtreePreL2+1;
        }
        updateFnArray(t2.prel_to_ln_[lGfirst], lGfirst, currentSubtreePreL2);
        updateFtArray(t2.prel_to_ln_[lGfirst], lGfirst);
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
          const int lFNode_label = t1.prel_to_label_id_[lF];
          // Increment size and cost of F forest by node lF.
          ++currentForestSize1;
          currentForestCost1 += (treesSwapped ? c_.ins(lFNode_label) : c_.del(lFNode_label)); // TODO: USE COST MODEL - sum up deletion cost of a forest.
          // Reset size and cost of forest in G to subtree G_lGfirst.
          currentForestSize2 = it2sizes[lGfirst];
          currentForestCost2 = (treesSwapped ? t2.prel_to_subtree_del_cost_[lGfirst] : t2.prel_to_subtree_ins_cost_[lGfirst]); // TODO: USE COST MODEL - reset to subtree insertion cost.
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
            sp3 = currentForestCost1 - (treesSwapped ? t1.prel_to_subtree_ins_cost_[lF] : t1.prel_to_subtree_del_cost_[lF]); // TODO: USE COST MODEL - Delete F_{lF,rF}-F_lF.
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
            case 3: sp1 = currentForestCost2; break; // TODO: USE COST MODEL - Insert G_{lG,rG}.
          }
          sp1 += (treesSwapped ? c_.ins(lFNode_label) : c_.del(lFNode_label));// TODO: USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
          // sp1 -- END
          minCost = sp1; // Start with sp1 as minimal value.
          // sp2 -- START
          if (currentForestSize2 == 1) { // G_{lG,rG} is a single node.
            sp2 = currentForestCost1; // TODO: USE COST MODEL - Delete F_{lF,rF}.
          } else { // G_{lG,rG} is a tree.
            sp2 = q_[lF];
          }
          sp2 += (treesSwapped ? c_.del(it2labels[lG]) : c_.ins(it2labels[lG]));// TODO: USE COST MODEL - Insert lG, leftmost root node in G_{lG,rG}.
          if (sp2 < minCost) { // Check if sp2 is minimal value.
            minCost = sp2;
          }
          // sp2 -- END
          // sp3 -- START
          if (sp3 < minCost) {
            // TODO: Matrix pointer here.
            sp3 += treesSwapped ? delta_.read_at(lG, lF) : delta_.read_at(sp3deltapointer, lG);
            if (sp3 < minCost) {
              sp3 += (treesSwapped ? c_.ren(it2labels[lG], lFNode_label) : c_.ren(lFNode_label, it2labels[lG])); // TODO: USE COST MODEL - Rename the leftmost root nodes in F_{lF,rF} and G_{lG,rG}.
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
            currentForestCost2 += (treesSwapped ? c_.del(it2labels[lG]) : c_.ins(it2labels[lG]));
            switch(sp1source) {
              // TODO: Matrix pointer here.
              case 1: sp1 = s.read_at(sp1spointer, lG - it2PreLoff) + (treesSwapped ? c_.ins(lFNode_label) : c_.del(lFNode_label)); break; // TODO: USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
              case 2: sp1 = t.read_at(lG - it2PreLoff, rG - it2PreRoff) + (treesSwapped ? c_.ins(lFNode_label) : c_.del(lFNode_label)); break; // TODO: USE COST MODEL - Delete lF, leftmost root node in F_{lF,rF}.
              case 3: sp1 = currentForestCost2 + (treesSwapped ? c_.ins(lFNode_label) : c_.del(lFNode_label)); break; // TODO: USE COST MODEL - Insert G_{lG,rG} and elete lF, leftmost root node in F_{lF,rF}.
            }
            // TODO: Matrix pointer here.
            sp2 = s.read_at(sp2spointer, fn_[lG] - it2PreLoff) + (treesSwapped ? c_.del(it2labels[lG]) : c_.ins(it2labels[lG])); // TODO: USE COST MODEL - Insert lG, leftmost root node in G_{lG,rG}.
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
                  case 2: sp3 += currentForestCost2 - (treesSwapped ? t2.prel_to_subtree_del_cost_[lG] : t2.prel_to_subtree_ins_cost_[lG]); break; // TODO: USE COST MODEL - Insert G_{lG,rG}-G_lG.
                  case 3: sp3 += t.read_at(fn_[(lG + it2sizes[lG]) - 1] - it2PreLoff, rG - it2PreRoff); break;
              }
              if (sp3 < minCost) {
                sp3 += (treesSwapped ? c_.ren(it2labels[lG], lFNode_label) : c_.ren(lFNode_label, it2labels[lG])); // TODO: USE COST MODEL - Rename the leftmost root nodes in F_{lF,rF} and G_{lG,rG}.
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
        // `rGminus1_in_preL == parent_of_rG_in_preL` checks if rG is the rightmost child of its parent
        // 
        // NOTE: rG not only has to be rightmost child of its parent, but also
        //       the parent has to be within the current subtree - it is dealt
        //       by setting `rGminus1_in_preL = max`, and then
        //       `rGminus1_in_preL != parent_of_rG_in_preL`.
        if (rGminus1_in_preL == parent_of_rG_in_preL) {
          if (!rightPart) {
            if (leftPart) {
              if (treesSwapped) {
                // TODO: What is rGminus1_in_preL + 1?
                delta_.at(parent_of_rG_in_preL, endPathNode) =
                    s.read_at((lFlast + 1) - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              } else {
                delta_.at(endPathNode, parent_of_rG_in_preL) =
                    s.read_at((lFlast + 1) - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              }
            }
            if (endPathNode > 0 && endPathNode == parent_of_endPathNode + 1 &&
                endPathNode_in_preR == parent_of_endPathNode_in_preR + 1) {
              if (treesSwapped) {
                delta_.at(parent_of_rG_in_preL, parent_of_endPathNode) =
                    s.read_at(lFlast - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              } else {
                delta_.at(parent_of_endPathNode, parent_of_rG_in_preL) =
                    s.read_at(lFlast - it1PreLoff, (rGminus1_in_preL + 1) - it2PreLoff);
              }
            }
          }
          // TODO: What is (parent_of_rG_in_preL + 1) - it2PreLoff?
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
      for (int i = currentSubtreePreL2; i < currentSubtreePreL2 + subtreeSize2; ++i){
        fn_[i] = -1;
        ft_[i] = -1;
      }
      // Store size and cost of the current forest in F.
      tmpForestSize1 = currentForestSize1;
      tmpForestCost1 = currentForestCost1;
      // Loop B' [1, Algorithm 3] - for all nodes in G.
      for (int lG = lGfirst; lG >= lGlast; --lG) {
        rGfirst = it2preL_to_preR[lG];
        updateFnArray(t2.prer_to_ln_[rGfirst], rGfirst, it2preL_to_preR[currentSubtreePreL2]);
        updateFtArray(t2.prer_to_ln_[rGfirst], rGfirst);
        int lF = lFfirst;
        lGminus1_in_preR = lG <= currentSubtreePreL2 ?
          std::numeric_limits<int>::max() : it2preL_to_preR[lG - 1];
        parent_of_lG = it2parents[lG];
        parent_of_lG_in_preR = parent_of_lG == -1 ? -1 : it2preL_to_preR[parent_of_lG];
        // Reset size and cost of forest if F.
        currentForestSize1 = tmpForestSize1;
        currentForestCost1 = tmpForestCost1;
        if (pathType == 0) {
          if (lG == currentSubtreePreL2) {
            rGlast = rGfirst;
          } else if (t2.prel_to_children_[parent_of_lG][0] != lG) {
            rGlast = rGfirst;
          } else {
            rGlast = it2preL_to_preR[parent_of_lG]+1;
          }
        } else {
          rGlast = rGfirst == it2preL_to_preR[currentSubtreePreL2] ?
              rGfirst : it2preL_to_preR[currentSubtreePreL2];
        }
        // Loop C' [1, Algorithm 3] - for all nodes to the right of the path node.
        for (int rF = rFfirst; rF >= rFlast; --rF) {
          if (rF == rFlast) {
            lF = lFlast;
          }
          rF_in_preL = it1preR_to_preL[rF];
          // Increment size and cost of F forest by node rF.
          ++currentForestSize1;
          currentForestCost1 += (treesSwapped ? c_.ins(t1.prel_to_label_id_[rF_in_preL]) : c_.del(t1.prel_to_label_id_[rF_in_preL])); // TODO: USE COST MODEL - sum up deletion cost of a forest.
          // Reset size and cost of G forest to G_lG.
          currentForestSize2 = it2sizes[lG];
          currentForestCost2 = (treesSwapped ? t2.prel_to_subtree_del_cost_[lG] : t2.prel_to_subtree_ins_cost_[lG]); // TODO: USE COST MODEL - reset to subtree insertion cost.
          rFSubtreeSize = it1sizes[rF_in_preL];
          if (startPathNode > 0) {
            rFIsConsecutiveNodeOfCurrentPathNode = startPathNode_in_preR - rF == 1;
            rFIsRightSiblingOfCurrentPathNode = rF + rFSubtreeSize == startPathNode_in_preR;
          } else {
            rFIsConsecutiveNodeOfCurrentPathNode = false;
            rFIsRightSiblingOfCurrentPathNode = false;
          }
          fForestIsTree = rF_in_preL == lF;
          const int rFNode_label = t1.prel_to_label_id_[rF_in_preL];
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
            sp3 = currentForestCost1 - (treesSwapped ? t1.prel_to_subtree_ins_cost_[rF_in_preL] : t1.prel_to_subtree_del_cost_[rF_in_preL]); // TODO: USE COST MODEL - Delete F_{lF,rF}-F_rF.
            if (rFIsRightSiblingOfCurrentPathNode) {
              sp3source = 3;
            }
          }
          if (sp3source == 1) {
            sp3spointer = (rF + rFSubtreeSize) - it1PreRoff;//s[(rF + rFSubtreeSize) - it1PreRoff];
          }
          if (currentForestSize2 == 1) {
            sp2 = currentForestCost1;// TODO: USE COST MODEL - Delete F_{lF,rF}.
          } else {
            sp2 = q_[rF];
          }
          int rG = rGfirst;
          rGfirst_in_preL = it2preR_to_preL[rGfirst];
          ++currentForestSize2;
          switch (sp1source) {
            case 1: sp1 = s.read_at(sp1spointer, rG - it2PreRoff); break;
            case 2: sp1 = t.read_at(sp1tpointer, rG - it2PreRoff); break;
            case 3: sp1 = currentForestCost2; break; // TODO: USE COST MODEL - Insert G_{lG,rG}.
          }
          sp1 += (treesSwapped ? c_.ins(rFNode_label) : c_.del(rFNode_label)); // TODO: USE COST MODEL - Delete rF.
          minCost = sp1;
          sp2 += (treesSwapped ? c_.del(it2labels[rGfirst_in_preL]) : c_.ins(it2labels[rGfirst_in_preL])); // TODO: USE COST MODEL - Insert rG.
          if (sp2 < minCost) {
            minCost = sp2;
          }
          if (sp3 < minCost) {
            sp3 += treesSwapped ? delta_.read_at(rGfirst_in_preL, rF_in_preL) : delta_.read_at(sp3deltapointer, rGfirst_in_preL);
            if (sp3 < minCost) {
              sp3 += (treesSwapped ? c_.ren(it2labels[rGfirst_in_preL], rFNode_label) : c_.ren(rFNode_label, it2labels[rGfirst_in_preL]));
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
            currentForestCost2 += (treesSwapped ? c_.del(it2labels[rG_in_preL]) : c_.ins(it2labels[rG_in_preL]));
            switch (sp1source) {
              case 1: sp1 = s.read_at(sp1spointer, rG - it2PreRoff) + (treesSwapped ? c_.ins(rFNode_label) : c_.del(rFNode_label)); break; // TODO: USE COST MODEL - Delete rF.
              case 2: sp1 = t.read_at(sp1tpointer, rG - it2PreRoff) + (treesSwapped ? c_.ins(rFNode_label) : c_.del(rFNode_label)); break; // TODO: USE COST MODEL - Delete rF.
              case 3: sp1 = currentForestCost2 + (treesSwapped ? c_.ins(rFNode_label) : c_.del(rFNode_label)); break; // TODO: USE COST MODEL - Insert G_{lG,rG} and delete rF.
            }
            sp2 = s.read_at(sp2spointer, fn_[rG] - it2PreRoff) + (treesSwapped ? c_.del(it2labels[rG_in_preL]) : c_.ins(it2labels[rG_in_preL])); // TODO: USE COST MODEL - Insert rG.
            minCost = sp1;
            if (sp2 < minCost) {
              minCost = sp2;
            }
            sp3 = treesSwapped ? delta_.read_at(rG_in_preL, rF_in_preL) : delta_.read_at(sp3deltapointer, rG_in_preL);
            if (sp3 < minCost) {
              switch (sp3source) {
                case 1: sp3 += s.read_at(sp3spointer, fn_[(rG + it2sizes[rG_in_preL]) - 1] - it2PreRoff); break;
                case 2: sp3 += currentForestCost2 - (treesSwapped ? t2.prel_to_subtree_del_cost_[rG_in_preL] : t2.prel_to_subtree_ins_cost_[rG_in_preL]); break; // TODO: USE COST MODEL - Insert G_{lG,rG}-G_rG.
                case 3: sp3 += t.read_at(sp3tpointer, fn_[(rG + it2sizes[rG_in_preL]) - 1] - it2PreRoff); break;
              }
              if (sp3 < minCost) {
                sp3 += (treesSwapped ? c_.ren(it2labels[rG_in_preL], rFNode_label) : c_.ren(rFNode_label, it2labels[rG_in_preL])); // TODO: USE COST MODEL - Rename rF to rG.
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
  // std::cout << "spfA = " << minCost << std::endl;
  return minCost;
}

template <typename CostModel, typename TreeIndex>
double APTEDTreeIndex<CostModel, TreeIndex>::spfL(const TreeIndex& t1,
    int t1_current_subtree, const TreeIndex& t2,
    int t2_current_subtree, bool treesSwapped) {
  // Initialise the array to store the keyroot nodes in the right-hand input
  // subtree.
  std::vector<int> keyRoots(t2.prel_to_size_[t2_current_subtree]);
  // Arrays.fill(keyRoots, -1);
  std::fill(keyRoots.begin(), keyRoots.end(), -1);
  // Get the leftmost leaf node of the right-hand input subtree.
  int pathID = t2.prel_to_lld_[t2_current_subtree];
  // Calculate the keyroot nodes in the right-hand input subtree.
  // firstKeyRoot is the index in keyRoots of the first keyroot node that
  // we have to process. We need this index because keyRoots array is larger
  // than the number of keyroot nodes.
  int firstKeyRoot = computeKeyRoots(t2, t2_current_subtree, pathID, keyRoots, 0);
  // Initialise an array to store intermediate distances for subforest pairs.
  data_structures::Matrix<double> forestdist(t1.prel_to_size_[t1_current_subtree]+1, t2.prel_to_size_[t2_current_subtree]+1);
  // Compute the distances between pairs of keyroot nodes. In the left-hand
  // input subtree only the root is the keyroot. Thus, we compute the distance
  // between the left-hand input subtree and all keyroot nodes in the
  // right-hand input subtree.
  for (int i = firstKeyRoot-1; i >= 0; --i) {
    treeEditDist(t1, t2, t1_current_subtree, keyRoots[i], forestdist, treesSwapped);
  }
  // Return the distance between the input subtrees.
  // std::cout << "spfL = " << forestdist.read_at(ni_1.preL_to_size_[ni_1.get_current_node()], ni_2.preL_to_size_[ni_2.get_current_node()]) << std::endl;
  return forestdist.read_at(t1.prel_to_size_[t1_current_subtree], t2.prel_to_size_[t2_current_subtree]);
}

template <typename CostModel, typename TreeIndex>
int APTEDTreeIndex<CostModel, TreeIndex>::computeKeyRoots(const TreeIndex& t2,
    int subtreeRootNode, int pathID, std::vector<int>& keyRoots, int index) {
  // The subtreeRootNode is a keyroot node. Add it to keyRoots.
  keyRoots[index] = subtreeRootNode;
  // Increment the index to know where to store the next keyroot node.
  ++index;
  // Walk up the left path starting with the leftmost leaf of subtreeRootNode,
  // until the child of subtreeRootNode.
  int pathNode = pathID;
  while (pathNode > subtreeRootNode) {
    int parent = t2.prel_to_parent_[pathNode];
    // For each sibling to the right of pathNode, execute this method recursively.
    // Each right sibling of pathNode is a keyroot node.
    for (int child : t2.prel_to_children_[parent]) {
      // Execute computeKeyRoots recursively for the new subtree rooted at child and child's leftmost leaf node.
      if (child != pathNode) index = computeKeyRoots(t2, child, t2.prel_to_lld_[child], keyRoots, index);
    }
    // Walk up.
    pathNode = parent;
  }
  return index;
}

template <typename CostModel, typename TreeIndex>
void APTEDTreeIndex<CostModel, TreeIndex>::treeEditDist(const TreeIndex& t1,
    const TreeIndex& t2, int it1subtree, int it2subtree,
    data_structures::Matrix<double>& forestdist, bool treesSwapped) {
  // Translate input subtree root nodes to left-to-right postorder.
  int i = t1.prel_to_postl_[it1subtree];
  int j = t2.prel_to_postl_[it2subtree];
  // We need to offset the node ids for accessing forestdist array which has
  // indices from 0 to subtree size. However, the subtree node indices do not
  // necessarily start with 0.
  // Whenever the original left-to-right postorder id has to be accessed, use
  // i+ioff and j+joff.
  int ioff = t1.postl_to_lld_[i] - 1;
  int joff = t2.postl_to_lld_[j] - 1;
  // Variables holding costs of each minimum element.
  float da = 0;
  float db = 0;
  float dc = 0;
  // Initialize forestdist array with deletion and insertion costs of each
  // relevant subforest.
  forestdist.at(0, 0) = 0;
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    forestdist.at(i1, 0) = forestdist.read_at(i1 - 1, 0) + (treesSwapped ? c_.ins(t1.postl_to_label_id_[i1 + ioff]) : c_.del(t1.postl_to_label_id_[i1 + ioff])); // TODO: USE COST MODEL - delete i1.
  }
  for (int j1 = 1; j1 <= j - joff; ++j1) {
    forestdist.at(0, j1) = forestdist.read_at(0, j1 - 1) + (treesSwapped ? c_.del(t2.postl_to_label_id_[j1 + joff]) : c_.ins(t2.postl_to_label_id_[j1 + joff])); // TODO: USE COST MODEL - insert j1.
  }
  // Fill in the remaining costs.
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    for (int j1 = 1; j1 <= j - joff; ++j1) {
      // Increment the number of subproblems.
      ++subproblem_counter_;
      // Calculate partial distance values for this subproblem.
      double u = (treesSwapped ? c_.ren(t2.postl_to_label_id_[j1 + joff], t1.postl_to_label_id_[i1 + ioff]) : c_.ren(t1.postl_to_label_id_[i1 + ioff], t2.postl_to_label_id_[j1 + joff])); // TODO: USE COST MODEL - rename i1 to j1.
      da = forestdist.read_at(i1 - 1, j1) + (treesSwapped ? c_.ins(t1.postl_to_label_id_[i1 + ioff]) : c_.del(t1.postl_to_label_id_[i1 + ioff])); // TODO: USE COST MODEL - delete i1.
      db = forestdist.read_at(i1, j1 - 1) + (treesSwapped ? c_.del(t2.postl_to_label_id_[j1 + joff]) : c_.ins(t2.postl_to_label_id_[j1 + joff])); // TODO: USE COST MODEL - insert j1.
      // If current subforests are subtrees.
      if (t1.postl_to_lld_[i1 + ioff] == t1.postl_to_lld_[i] && t2.postl_to_lld_[j1 + joff] == t2.postl_to_lld_[j]) {
        dc = forestdist.read_at(i1 - 1, j1 - 1) + u;
        // Store the relevant distance value in delta array.
        if (treesSwapped) {
          delta_.at(t2.postl_to_prel_[j1 + joff], t1.postl_to_prel_[i1 + ioff]) = forestdist.read_at(i1 - 1, j1 - 1);
        } else {
          delta_.at(t1.postl_to_prel_[i1 + ioff], t2.postl_to_prel_[j1 + joff]) = forestdist.read_at(i1 - 1, j1 - 1);
        }
      } else {
        dc = forestdist.read_at(t1.postl_to_lld_[i1 + ioff] - 1 - ioff, t2.postl_to_lld_[j1 + joff] - 1 - joff) +
          (treesSwapped ? delta_.read_at(t2.postl_to_prel_[j1 + joff], t1.postl_to_prel_[i1 + ioff]) : delta_.read_at(t1.postl_to_prel_[i1 + ioff], t2.postl_to_prel_[j1 + joff])) + u;
      }
      // Calculate final minimum.
      forestdist.at(i1, j1) = da >= db ? db >= dc ? dc : db : da >= dc ? dc : da;
    }
  }
}

template <typename CostModel, typename TreeIndex>
double APTEDTreeIndex<CostModel, TreeIndex>::spfR(const TreeIndex& t1,
    int t1_current_subtree, const TreeIndex& t2,
    int t2_current_subtree, bool treesSwapped) {
  // Initialise the array to store the keyroot nodes in the right-hand input
  // subtree.
  std::vector<int> revKeyRoots(t2.prel_to_size_[t2_current_subtree]);
  // Arrays.fill(revKeyRoots, -1);
  std::fill(revKeyRoots.begin(), revKeyRoots.end(), -1);
  // Get the rightmost leaf node of the right-hand input subtree.
  int pathID = t2.prel_to_rld_[t2_current_subtree];
  // Calculate the keyroot nodes in the right-hand input subtree.
  // firstKeyRoot is the index in keyRoots of the first keyroot node that
  // we have to process. We need this index because keyRoots array is larger
  // than the number of keyroot nodes.
  int firstKeyRoot = computeRevKeyRoots(t2, t2_current_subtree, pathID, revKeyRoots, 0);
  // Initialise an array to store intermediate distances for subforest pairs.
  data_structures::Matrix<double> forestdist(t1.prel_to_size_[t1_current_subtree]+1, t2.prel_to_size_[t2_current_subtree]+1);
  // Compute the distances between pairs of keyroot nodes. In the left-hand
  // input subtree only the root is the keyroot. Thus, we compute the distance
  // between the left-hand input subtree and all keyroot nodes in the
  // right-hand input subtree.
  for (int i = firstKeyRoot-1; i >= 0; --i) {
    revTreeEditDist(t1, t2, t1_current_subtree, revKeyRoots[i], forestdist, treesSwapped);
  }
  // Return the distance between the input subtrees.
  // std::cout << "spfR = " << forestdist.read_at(ni_1.preL_to_size_[ni_1.get_current_node()], ni_2.preL_to_size_[ni_2.get_current_node()]) << std::endl;
  return forestdist.read_at(t1.prel_to_size_[t1_current_subtree], t2.prel_to_size_[t2_current_subtree]);
}

template <typename CostModel, typename TreeIndex>
int APTEDTreeIndex<CostModel, TreeIndex>::computeRevKeyRoots(
    const TreeIndex& t2, int subtreeRootNode, int pathID,
    std::vector<int>& revKeyRoots, int index) {
  // The subtreeRootNode is a keyroot node. Add it to keyRoots.
  revKeyRoots[index] = subtreeRootNode;
  // Increment the index to know where to store the next keyroot node.
  ++index;
  // Walk up the right path starting with the rightmost leaf of
  // subtreeRootNode, until the child of subtreeRootNode.
  int pathNode = pathID;
  while (pathNode > subtreeRootNode) {
    int parent = t2.prel_to_parent_[pathNode];
    // For each sibling to the left of pathNode, execute this method recursively.
    // Each left sibling of pathNode is a keyroot node.
    for (int child : t2.prel_to_children_[parent]) {
      // Execute computeRevKeyRoots recursively for the new subtree rooted at child and child's rightmost leaf node.
      if (child != pathNode) {
        index = computeRevKeyRoots(t2, child, t2.prel_to_rld_[child], revKeyRoots, index);
      }
    }
    // Walk up.
    pathNode = parent;
  }
  return index;
}

template <typename CostModel, typename TreeIndex>
void APTEDTreeIndex<CostModel, TreeIndex>::revTreeEditDist(const TreeIndex& t1,
    const TreeIndex& t2, int it1subtree, int it2subtree,
    data_structures::Matrix<double>& forestdist, bool treesSwapped) {
  // Translate input subtree root nodes to right-to-left postorder.
  int i = t1.prel_to_postr_[it1subtree];
  int j = t2.prel_to_postr_[it2subtree];
  // We need to offset the node ids for accessing forestdist array which has
  // indices from 0 to subtree size. However, the subtree node indices do not
  // necessarily start with 0.
  // Whenever the original right-to-left postorder id has to be accessed, use
  // i+ioff and j+joff.
  int ioff = t1.postr_to_rld_[i] - 1;
  int joff = t2.postr_to_rld_[j] - 1;
  // Variables holding costs of each minimum element.
  float da = 0;
  float db = 0;
  float dc = 0;
  // Initialize forestdist array with deletion and insertion costs of each
  // relevant subforest.
  forestdist.at(0,0) = 0;
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    forestdist.at(i1, 0) = forestdist.read_at(i1 - 1, 0) + (treesSwapped ? c_.ins(t1.postr_to_label_id_[i1 + ioff]) : c_.del(t1.postr_to_label_id_[i1 + ioff])); // TODO: USE COST MODEL - delete i1.
  }
  for (int j1 = 1; j1 <= j - joff; ++j1) {
    forestdist.at(0, j1) = forestdist.read_at(0, j1 - 1) + (treesSwapped ? c_.del(t2.postr_to_label_id_[j1 + joff]) : c_.ins(t2.postr_to_label_id_[j1 + joff])); // TODO: USE COST MODEL - insert j1.
  }
  // Fill in the remaining costs.
  for (int i1 = 1; i1 <= i - ioff; ++i1) {
    for (int j1 = 1; j1 <= j - joff; ++j1) {
      // Increment the number of subproblems.
      ++subproblem_counter_;
      // Calculate partial distance values for this subproblem.
      float u = (treesSwapped ? c_.ren(t2.postr_to_label_id_[j1 + joff], t1.postr_to_label_id_[i1 + ioff]) : c_.ren(t1.postr_to_label_id_[i1 + ioff], t2.postr_to_label_id_[j1 + joff])); // TODO: USE COST MODEL - rename i1 to j1.
      da = forestdist.read_at(i1 - 1, j1) + (treesSwapped ? c_.ins(t1.postr_to_label_id_[i1 + ioff]) : c_.del(t1.postr_to_label_id_[i1 + ioff])); // TODO: USE COST MODEL - delete i1.
      db = forestdist.read_at(i1, j1 - 1) + (treesSwapped ? c_.del(t2.postr_to_label_id_[j1 + joff]) : c_.ins(t2.postr_to_label_id_[j1 + joff])); // TODO: USE COST MODEL - insert j1.
      // If current subforests are subtrees.
      if (t1.postr_to_rld_[i1 + ioff] == t1.postr_to_rld_[i] && t2.postr_to_rld_[j1 + joff] == t2.postr_to_rld_[j]) {
        dc = forestdist.read_at(i1 - 1, j1 - 1) + u;
        // Store the relevant distance value in delta array.
        if (treesSwapped) {
          delta_.at(t2.postr_to_prel_[j1+joff], t1.postr_to_prel_[i1+ioff]) = forestdist.read_at(i1 - 1, j1 - 1);
        } else {
          delta_.at(t1.postr_to_prel_[i1+ioff], t2.postr_to_prel_[j1+joff]) = forestdist.read_at(i1 - 1, j1 - 1);
        }
      } else {
        dc = forestdist.read_at(t1.postr_to_rld_[i1 + ioff] - 1 - ioff, t2.postr_to_rld_[j1 + joff] - 1 - joff) +
          (treesSwapped ? delta_.read_at(t2.postr_to_prel_[j1 + joff], t1.postr_to_prel_[i1 + ioff]) : delta_.read_at(t1.postr_to_prel_[i1 + ioff], t2.postr_to_prel_[j1 + joff])) + u;
      }
      // Calculate final minimum.
      forestdist.at(i1, j1) = da >= db ? db >= dc ? dc : db : da >= dc ? dc : da;
      // std::cerr << "forestdist.at(i1, j1) = " << forestdist.at(i1, j1) << std::endl;
    }
  }
}


template <typename CostModel, typename TreeIndex>
int APTEDTreeIndex<CostModel, TreeIndex>::get_strategy_path_type(
    int pathIDWithPathIDOffset, int pathIDOffset, int currentRootNodePreL,
    int currentSubtreeSize) {
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
}

template <typename CostModel, typename TreeIndex>
void APTEDTreeIndex<CostModel, TreeIndex>::updateFnArray(int lnForNode,
    int node, int currentSubtreePreL) {
  if (lnForNode >= currentSubtreePreL) {
    fn_[node] = fn_[lnForNode];
    fn_[lnForNode] = node;
  } else {
    fn_[node] = fn_[fn_.size() - 1];
    fn_[fn_.size() - 1] = node;
  }
}

template <typename CostModel, typename TreeIndex>
void APTEDTreeIndex<CostModel, TreeIndex>::updateFtArray(int lnForNode,
    int node) {
  ft_[node] = lnForNode;
  if(fn_[node] > -1) {
    ft_[fn_[node]] = node;
  }
}
