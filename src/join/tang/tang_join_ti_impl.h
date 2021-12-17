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

/// \file join/tang/tang_join_impl.h
///
/// \details
/// Implements the tree similarity join algorithm proposed by Tang et. al in 
/// "Scaling Similarity Joins over Tree-Structured Data".

#pragma once

template <typename Label, typename VerificationAlgorithm>
TangJoinTI<Label, VerificationAlgorithm>::TangJoinTI() {
  pre_candidates_ = 0;
  sum_subproblem_counter_ = 0;
  il_lookups_ = 0;
}

template <typename Label, typename VerificationAlgorithm>
void TangJoinTI<Label, VerificationAlgorithm>::execute_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<node::BinaryNode<Label>>& binary_trees_collection,
    std::unordered_set<std::pair<int, int>, hashintegerpair>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  // Convert trees to sets and get the result.
  convert_trees_to_binary_trees(trees_collection, binary_trees_collection);

  // Compute candidates for the join using a partition-based algorithm
  retrieve_candidates(binary_trees_collection, candidates, distance_threshold);

  // Verify all computed join candidates and return the join result
  verify_candidates(trees_collection, candidates, join_result, distance_threshold);
}


template <typename Label, typename VerificationAlgorithm>
void TangJoinTI<Label, VerificationAlgorithm>::convert_trees_to_binary_trees(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<node::BinaryNode<Label>>& binary_trees_collection) {

  // Convert trees to sets and get the result.
  binary_tree_converter::Converter<Label> btsc;
  btsc.convert(trees_collection, binary_trees_collection);
}

template <typename Label, typename VerificationAlgorithm>
void TangJoinTI<Label, VerificationAlgorithm>::retrieve_candidates(
    std::vector<node::BinaryNode<Label>>& binary_trees_collection,
    std::unordered_set<std::pair<int, int>, hashintegerpair>& candidates,
    const double distance_threshold) {

  // initialize inverted list index I of subgraphs
  // -> pointer to root of subtree?
  std::unordered_map<int, std::vector<int>> small_trees;
  // number of subgraphs, we want to achieve
  int delta = 2 * distance_threshold + 1;

  // for all trees in binary_trees_collection
  for(std::size_t curr_binary_tree_id = 0; curr_binary_tree_id < binary_trees_collection.size(); 
        ++curr_binary_tree_id) {
    int curr_tree_size = binary_trees_collection[curr_binary_tree_id].get_tree_size();

    // get all nodes of the actual 
    std::vector<node::BinaryNode<Label>*> nodes_in_postorder;
    binary_trees_collection[curr_binary_tree_id].get_node_postorder_vector(nodes_in_postorder);

    // only look at trees with size in the threshold range -> for n in max(|T_i| - tau, 1), |T_i|)
    for(int n = std::max(1, curr_tree_size - (int)distance_threshold); n <= curr_tree_size; ++n) {      
      // traverse actual tree in postorder
      for(std::size_t curr_node_postorder_id = 0; curr_node_postorder_id < nodes_in_postorder.size(); 
          ++curr_node_postorder_id) {
        // S contains trees T_j that contain the relevant subgraphs s_j  -> S = getSubgraphs(T_i, N)
        std::vector<int> S;
        get_subgraphs(nodes_in_postorder[curr_node_postorder_id], n, curr_node_postorder_id, S);

        // for each T_j (T_j .. tree that owns s) in S
        for(int T_j: S)
          // add(T_i, T_j) to candidates, join candidates is an unordered set,
          // therefore, no worries about duplicates
          candidates.emplace(curr_binary_tree_id, T_j);
      }

      // FIX: add all pairs with small trees
      for(int T_j: small_trees[n])
        candidates.emplace(curr_binary_tree_id, T_j);
    }

    // FIX: keep track of small trees, which cannot be partitioned with the given threshold
    if(curr_tree_size < delta)
      small_trees[curr_tree_size].push_back(curr_binary_tree_id);
    else {
      // g = MAXMINSIZE(T_i, 2*tau + 1)
      // maximal number of nodes within a subgraph
      int gamma = max_min_size(&binary_trees_collection[curr_binary_tree_id],
                                        curr_tree_size, delta);

      // S' = PARTITION(T_i, 2*tau + 1, g)
      // for each s in S'
        // insert s into inverted list index I_T_i
      int postorder_id = 0;
      int subgraph_id = 0;
      update_inverted_list(&binary_trees_collection[curr_binary_tree_id], delta, gamma,
                           curr_tree_size, curr_binary_tree_id, postorder_id, subgraph_id, distance_threshold);
    }
  }
}

template <typename Label, typename VerificationAlgorithm>
void TangJoinTI<Label, VerificationAlgorithm>::get_subgraphs(
    node::BinaryNode<Label>* curr_node, int tree_size, 
    int curr_node_postorder_id, std::vector<int>& S) {
  
  // get string that concatenates the labels of the current node and its children
  // take a look at all combinations of empty children (Paper Section 3.4 label indexing)
  if(curr_node->get_left_child() != nullptr && curr_node->get_right_child() != nullptr) {
    std::string l_ll_lr = curr_node->get_top_twig_labels();
    for(auto& set_in_il: inverted_list_[tree_size][curr_node_postorder_id][l_ll_lr]) {
      ++il_lookups_;
      if(check_subgraphs(curr_node, set_in_il.second)) // are the subgraphs equivalent
        S.push_back(set_in_il.first);
    }
  }
  if(curr_node->get_left_child() != nullptr) {
    std::string l_ll_e = curr_node->label().to_string() + curr_node->get_left_child()->label().to_string() + empty_string_;
    for(auto& set_in_il: inverted_list_[tree_size][curr_node_postorder_id][l_ll_e]) {
      ++il_lookups_;
      if(check_subgraphs(curr_node, set_in_il.second)) // are the subgraphs equivalent
        S.push_back(set_in_il.first);
    }
  }
  if(curr_node->get_right_child() != nullptr) {
    std::string l_e_lr = curr_node->label().to_string() + empty_string_ + curr_node->get_right_child()->label().to_string();
    for(auto& set_in_il: inverted_list_[tree_size][curr_node_postorder_id][l_e_lr]) {
      ++il_lookups_;
      if(check_subgraphs(curr_node, set_in_il.second)) // are the subgraphs equivalent
        S.push_back(set_in_il.first);
    }
  }
  std::string l_e_e = curr_node->label().to_string() + empty_string_ + empty_string_;
    for(auto& set_in_il: inverted_list_[tree_size][curr_node_postorder_id][l_e_e]) {
      ++il_lookups_;
      if(check_subgraphs(curr_node, set_in_il.second)) // are the subgraphs equivalent
        S.push_back(set_in_il.first);
    }
}


template <typename Label, typename VerificationAlgorithm>
bool TangJoinTI<Label, VerificationAlgorithm>::check_subgraphs(
    node::BinaryNode<Label>* left_tree_node, node::BinaryNode<Label>* right_tree_node) {
  // nodes have the same label
  if(left_tree_node->label() == right_tree_node->label()) {
    bool subgraph_identical = true;
    // check recursively for all descendants
    if(right_tree_node->has_left_child() &&
        right_tree_node->get_left_child()->get_subgraph_size() != right_tree_node->get_left_child()->get_detached()) {
      if(left_tree_node->has_left_child())
        subgraph_identical &= check_subgraphs(left_tree_node->get_left_child(), right_tree_node->get_left_child());
      else
        return false;
    }
    if(subgraph_identical && right_tree_node->has_right_child() &&
        right_tree_node->get_right_child()->get_subgraph_size() != right_tree_node->get_right_child()->get_detached()) {
      if(left_tree_node->has_right_child())
        subgraph_identical &= check_subgraphs(left_tree_node->get_right_child(), right_tree_node->get_right_child());
      else
        return false;
    }

    return subgraph_identical;
  }

  // labels don't match -> subgraphs aren't identical
  return false;
}

template <typename Label, typename VerificationAlgorithm>
int TangJoinTI<Label, VerificationAlgorithm>::max_min_size(
    node::BinaryNode<Label>* curr_node, int tree_size, int delta) {
  
  int gamma_max = std::floor(tree_size / delta);
  int gamma_min = std::floor((tree_size + delta - 1) / (2 * delta - 1));
  int c = gamma_max - gamma_min + 1;

  while(c > 1) {
    int gamma_mid = gamma_min + std::floor(c/2);

    if(partitionable(curr_node, delta, gamma_mid)) {
      gamma_min = gamma_mid;
      c = c - std::floor(c/2);
    } else {
      c = std::floor(c/2);
    }
  }

  return gamma_min;
}

template <typename Label, typename VerificationAlgorithm>
bool TangJoinTI<Label, VerificationAlgorithm>::partitionable(
    node::BinaryNode<Label>* curr_node, int delta, int gamma) {
  int nr_of_subgraphs_found = 0;

  return recursive_partitionable(curr_node, delta, gamma, nr_of_subgraphs_found);
}

template <typename Label, typename VerificationAlgorithm>
bool TangJoinTI<Label, VerificationAlgorithm>::recursive_partitionable(
    node::BinaryNode<Label>* curr_node, int delta, int gamma, 
    int& nr_of_subgraphs_found) {
  
  curr_node->set_subgraph_size(1);
  curr_node->set_detached(0);

  node::BinaryNode<Label>* left_child = curr_node->get_left_child();
  node::BinaryNode<Label>* right_child = curr_node->get_right_child();
  if(left_child != nullptr) {
    if(recursive_partitionable(left_child, delta, gamma, nr_of_subgraphs_found))
      return true;
    curr_node->set_subgraph_size(curr_node->get_subgraph_size() + left_child->get_subgraph_size());
    curr_node->set_detached(curr_node->get_detached() + left_child->get_detached());
  }
  if(right_child != nullptr) {
    if(recursive_partitionable(right_child, delta, gamma, nr_of_subgraphs_found))
      return true;
    curr_node->set_subgraph_size(curr_node->get_subgraph_size() + right_child->get_subgraph_size());
    curr_node->set_detached(curr_node->get_detached() + right_child->get_detached());
  }
  if(curr_node->get_subgraph_size() - curr_node->get_detached() >= gamma) {
    ++nr_of_subgraphs_found;  // identified gamma subtree
    curr_node->set_detached(curr_node->get_subgraph_size());
    if(nr_of_subgraphs_found >= delta)
      return true;  // found (delta, gamma)-partitioning
  }

  return false;
}

template <typename Label, typename VerificationAlgorithm>
bool TangJoinTI<Label, VerificationAlgorithm>::update_inverted_list(
    node::BinaryNode<Label>* curr_node, int delta, int gamma, 
    int tree_size, int curr_tree_id, int& postorder_id,
    int& subgraph_id, const double distance_threshold) {
  
  curr_node->set_subgraph_size(1);
  curr_node->set_detached(0);

  node::BinaryNode<Label>* left_child = curr_node->get_left_child();
  node::BinaryNode<Label>* right_child = curr_node->get_right_child();
  if(left_child != nullptr) {
    if(update_inverted_list(left_child, delta, gamma, tree_size, curr_tree_id, postorder_id, subgraph_id, distance_threshold))
      return true;
    curr_node->set_subgraph_size(curr_node->get_subgraph_size() + left_child->get_subgraph_size());
    curr_node->set_detached(curr_node->get_detached() + left_child->get_detached());
  }
  if(right_child != nullptr) {
    if(update_inverted_list(right_child, delta, gamma, tree_size, curr_tree_id, postorder_id, subgraph_id, distance_threshold))
      return true;
    curr_node->set_subgraph_size(curr_node->get_subgraph_size() + right_child->get_subgraph_size());
    curr_node->set_detached(curr_node->get_detached() + right_child->get_detached());
  }

  curr_node->set_postorder_id(postorder_id++);

  if(curr_node->get_subgraph_size() - curr_node->get_detached() >= gamma) {
    ++subgraph_id;
    curr_node->set_detached(curr_node->get_subgraph_size());

    // get top twig label of the current subgraph
    std::string twig_labels = curr_node->label().to_string();
    if(left_child != nullptr) twig_labels += left_child->label().to_string();
    else twig_labels += empty_string_;
    if(right_child != nullptr) twig_labels += right_child->label().to_string();
    else twig_labels += empty_string_;

    int lambda_prim = std::max(0, (int)(distance_threshold - std::floor(subgraph_id/2)));
    int min_index = std::max(0, (int)(curr_node->get_postorder_id() - lambda_prim));
    int max_index = curr_node->get_postorder_id() + lambda_prim;

    // insert the subgraph at all relevant postorder positions with its top twig labels
    for(int current_postorder_idx = min_index; current_postorder_idx <= max_index; ++current_postorder_idx)
      inverted_list_[tree_size][current_postorder_idx][twig_labels].emplace_back(curr_tree_id, curr_node);

    if(subgraph_id >= delta)
      return true;  // found (delta, gamma)-partitioning
  }

  return false;
}

template <typename Label, typename VerificationAlgorithm>
void TangJoinTI<Label, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::unordered_set<std::pair<int, int>, hashintegerpair>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

  label::LabelDictionary<Label> ld;
  typename VerificationAlgorithm::AlgsCostModel cm(ld);

  VerificationAlgorithm ted_algorithm(cm);
  typename VerificationAlgorithm::AlgsTreeIndex ti_1;
  typename VerificationAlgorithm::AlgsTreeIndex ti_2;

  // Verify each pair in the candidate set
  for(const auto& pair: candidates) {
    node::index_tree(ti_1, trees_collection[pair.first], ld, cm);
    node::index_tree(ti_2, trees_collection[pair.second], ld, cm);
    double ted_value = ted_algorithm.ted_k(ti_1, ti_2, distance_threshold);

    if(ted_value <= distance_threshold)
      join_result.emplace_back(pair.first, pair.second, ted_value);
    
    // Sum up all number of subproblems
    sum_subproblem_counter_ += ted_algorithm.get_subproblem_count();
  }
}

template <typename Label, typename VerificationAlgorithm>
long long int
    TangJoinTI<Label, VerificationAlgorithm>::get_number_of_pre_candidates() const {
  return pre_candidates_;
}

template <typename Label, typename VerificationAlgorithm>
long long int TangJoinTI<Label, VerificationAlgorithm>::get_subproblem_count() const {
  return sum_subproblem_counter_;
}

template <typename Label, typename VerificationAlgorithm>
long long int
    TangJoinTI<Label, VerificationAlgorithm>::get_number_of_il_lookups() const {
  return il_lookups_;
}
