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
Guha<Label, CostModel, VerificationAlgorithm>::Guha() {}

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
  
  // std::string s("{");
  // for (auto e : reference_set) {
  //   s += std::to_string(e) + ",";
  // }
  // s.pop_back();
  // s += "}";
  // std::cout << s << std::endl;

  std::vector<std::vector<double>> ted_vectors(trees_collection.size(), std::vector<double>(reference_set.size()));
  
  // Retrieves candidates from the candidate index.
  retrieve_candidates(trees_collection, candidates, distance_threshold, reference_set, ted_vectors);

  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold, ted_vectors);
}

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::retrieve_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    const double distance_threshold,
    std::vector<unsigned int>& reference_set,
    std::vector<std::vector<double>>& ted_vectors) {

  // TODO
  
  // For each tree in the collection compute the distance to every element
  // in the reference set. Store the values in a seperate vector for each tree.
  ted::ZhangShasha<Label, CostModel> zs_ted;
  
  unsigned int data_tree_id = 0;
  unsigned int rs_tree_id = 0;
  for (auto t : trees_collection) {
    rs_tree_id = 0;
    for (auto t_id_rs : reference_set) {
      ted_vectors[data_tree_id][rs_tree_id] = zs_ted.zhang_shasha_ted(t, trees_collection[t_id_rs]);
      ++rs_tree_id;
    }
    ++data_tree_id;
  }
  
  // std::string s("{");
  // for (auto ve : ted_vectors) {
  //   s += "{";
  //   for (auto e : ve) {
  //     s += std::to_string(e) + ",";
  //   }
  //   s.pop_back();
  //   s += "}\n";
  // }
  // s += "}";
  // std::cout << s;
  // 
  // std::cout << "\n\n";
  // 
  // std::cout << "ted_vectors.size() = " << std::to_string(ted_vectors.size()) << std::endl;
  // std::cout << "reference_set.size() = " << std::to_string(reference_set.size()) << std::endl;
  // 
  // std::cout << "\n\n";
  
  // For each ted vector pair, verify the triangle unequality lower bound
  // condition. If the condition is satisfied, add the pair to candidate set.
  double pair_l_t = 0;
  for (unsigned int v1_id = 0; v1_id < ted_vectors.size(); ++v1_id) {
    for (unsigned int v2_id = v1_id+1; v2_id < ted_vectors.size(); ++v2_id) {
      // std::cout << v1_id << "," << v2_id << std::endl;
      pair_l_t = l_t(ted_vectors[v1_id], ted_vectors[v2_id]);
      // std::cout << "l_t = " << std::to_string(pair_l_t) << std::endl;
      if (pair_l_t <= distance_threshold) {
        candidates.push_back({v1_id, v2_id});
      }
    }
  }
  
  // s = "{";
  // for (auto e : candidates) {
  //   s += "(" + std::to_string(e.first) + "," + std::to_string(e.second) + "),";
  // }
  // s.pop_back();
  // s += "}";
  // std::cout << s << std::endl;
}

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold,
    std::vector<std::vector<double>>& ted_vectors) {

  VerificationAlgorithm ted_algorithm;

  double pair_u_t = 0;
  // Verify each pair in the candidate set
  for(const auto& pair: candidates) {
    // Verify the triangle unequality upper bound condition.
    // If the condition is satisfied, add the pair to result set.
    // std::cout << pair.first << "," << pair.second << std::endl;
    pair_u_t = u_t(ted_vectors[pair.first], ted_vectors[pair.second]);
    // std::cout << "u_t = " << std::to_string(pair_u_t) << std::endl;
    if (pair_u_t <= distance_threshold) {
      join_result.emplace_back(pair.first, pair.second, pair_u_t);
    } else {
      // Verify with TED.
      double ted_value = ted_algorithm.verify(trees_collection[pair.first],
                                              trees_collection[pair.second],
                                              distance_threshold);
      if(ted_value <= distance_threshold) {
        join_result.emplace_back(pair.first, pair.second, ted_value);
      }
    }
  }
}

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

#endif // TREE_SIMILARITY_JOIN_GUHA_GUHA_IMPL_H
