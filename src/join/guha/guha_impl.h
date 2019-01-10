// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Thomas Huetter
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

/// \file join/tjoin/guha_impl.h
///
/// \details
/// Implements partially the join algorithm by Guha [1].

#ifndef TREE_SIMILARITY_JOIN_GUHA_GUHA_IMPL_H
#define TREE_SIMILARITY_JOIN_GUHA_GUHA_IMPL_H

template <typename Label, typename CostModel, typename VerificationAlgorithm>
Guha<Label, CostModel, VerificationAlgorithm>::Guha() {
  l_t_candidates_ = 0;
  sed_candidates_ = 0;
  u_t_result_pairs_ = 0;
  cted_result_pairs_ = 0;
}

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::execute_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold,
    unsigned int reference_set_size) {

  std::vector<unsigned int> reference_set = get_random_reference_set(
      trees_collection, reference_set_size
  );
  
  std::vector<std::vector<double>> ted_vectors(trees_collection.size(), std::vector<double>(reference_set.size()));
  
  // Compute the vectors.
  compute_vectors(trees_collection, reference_set, ted_vectors);
  
  // Retrieves candidates from the candidate index.
  retrieve_candidates(trees_collection, candidates, join_result, distance_threshold, reference_set, ted_vectors);

  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold, ted_vectors);
}


template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::compute_vectors(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<unsigned int>& reference_set,
    std::vector<std::vector<double>>& ted_vectors) {
  // For each tree in the collection compute the distance to every element
  // in the reference set. Store the values in a seperate vector for each tree.
  ted::APTED<Label, CostModel> ted_algorithm;
  unsigned int data_tree_id = 0;
  unsigned int rs_tree_id = 0;
  for (auto t : trees_collection) {
    rs_tree_id = 0;
    for (auto t_id_rs : reference_set) {
      ted_vectors[data_tree_id][rs_tree_id] = ted_algorithm.apted_ted(t, trees_collection[t_id_rs]);
      ++rs_tree_id;
    }
    ++data_tree_id;
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::retrieve_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold,
    std::vector<unsigned int>& reference_set,
    std::vector<std::vector<double>>& ted_vectors) {
  
  // For each ted vector pair, verify the triangle unequality lower bound
  // condition. If the condition is satisfied, add the pair to candidate set.
  ted_lb::StringEditDistanceLB<Label, CostModel> sed_lb;
  ted_ub::ConstrainedUB<Label, CostModel> cted_ub;
  double pair_l_t = 0;
  double pair_u_t = 0;
  for (unsigned int v1_id = 0; v1_id < ted_vectors.size(); ++v1_id) {
    for (unsigned int v2_id = v1_id+1; v2_id < ted_vectors.size(); ++v2_id) {
      // std::cout << v1_id << "," << v2_id << std::endl;
      pair_l_t = l_t(ted_vectors[v1_id], ted_vectors[v2_id]);
      if (pair_l_t <= distance_threshold) {
        ++l_t_candidates_;
        pair_u_t = u_t(ted_vectors[v1_id], ted_vectors[v2_id]);
        if (pair_u_t <= distance_threshold) {
          ++u_t_result_pairs_;
          join_result.emplace_back(v1_id, v2_id, pair_u_t);
          continue;
        }
        pair_l_t = sed_lb.sed_lb_ted(trees_collection[v1_id], trees_collection[v2_id]);
        if (pair_l_t <= distance_threshold) {
          ++sed_candidates_;
          pair_u_t = cted_ub.cted_ub_ted(trees_collection[v1_id], trees_collection[v2_id]);
          if (pair_u_t <= distance_threshold) {
            ++cted_result_pairs_;
            join_result.emplace_back(v1_id, v2_id, pair_u_t);
            continue;
          }
          candidates.push_back({v1_id, v2_id});
        }
      }
    }
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold,
    std::vector<std::vector<double>>& ted_vectors) {

  VerificationAlgorithm ted_algorithm;

  // Verify each pair in the candidate set
  for(const auto& pair: candidates) {
    // Verify with TED.
    double ted_value = ted_algorithm.verify(trees_collection[pair.first],
                                            trees_collection[pair.second],
                                            distance_threshold);
    if(ted_value <= distance_threshold) {
      join_result.emplace_back(pair.first, pair.second, ted_value);
    }
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
std::vector<unsigned int> Guha<Label, CostModel, VerificationAlgorithm>::get_random_reference_set(
  std::vector<node::Node<Label>>& trees_collection,
  unsigned int k) {
    std::vector<unsigned int> reference_set;
    std::mt19937 rd;
    std::uniform_int_distribution<unsigned int> nodes_dist(0, trees_collection.size()-1);
    for (unsigned int i = 0; i < k; ++i) {
      reference_set.push_back(nodes_dist(rd));
    }
    return reference_set;
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
double Guha<Label, CostModel, VerificationAlgorithm>::u_t(std::vector<double>& v_1, std::vector<double>& v_2) {
  double global_minimum = std::numeric_limits<double>::infinity();
  double current_sum = 0;
  for (unsigned int i = 0; i < v_1.size(); ++i) {
    current_sum = v_1[i] + v_2[i];
    global_minimum = std::min(global_minimum, current_sum);
  }
  return global_minimum;
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
double Guha<Label, CostModel, VerificationAlgorithm>::l_t(std::vector<double>& v_1, std::vector<double>& v_2) {
  double global_maximum = 0;
  double current_diff = 0;
  for (unsigned int i = 0; i < v_1.size(); ++i) {
    current_diff = std::abs(v_1[i] - v_2[i]);
    global_maximum = std::max(global_maximum, current_diff);
  }
  return global_maximum;
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
const unsigned long long int Guha<Label, CostModel, VerificationAlgorithm>::get_l_t_candidates() const {
  return l_t_candidates_;
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
const unsigned long long int Guha<Label, CostModel, VerificationAlgorithm>::get_sed_candidates() const {
  return sed_candidates_;
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
const unsigned long long int Guha<Label, CostModel, VerificationAlgorithm>::get_u_t_result_pairs() const {
  return u_t_result_pairs_;
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
const unsigned long long int Guha<Label, CostModel, VerificationAlgorithm>::get_cted_result_pairs() const {
  return cted_result_pairs_;
};
#endif // TREE_SIMILARITY_JOIN_GUHA_GUHA_IMPL_H
