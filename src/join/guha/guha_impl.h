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
void Guha<Label, CostModel, VerificationAlgorithm>::execute_rsb_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {
  
  std::vector<unsigned int> reference_set = get_reference_set(
      trees_collection, distance_threshold
  );
  
  std::vector<std::vector<double>> ted_vectors(trees_collection.size(), std::vector<double>(reference_set.size()));
  
  // Compute the vectors.
  compute_vectors(trees_collection, reference_set, ted_vectors);
  
  // Retrieves candidates from the candidate index.
  retrieve_metric_candidates(trees_collection, candidates, join_result, distance_threshold, ted_vectors);
  retrieve_sc_candidates(trees_collection, candidates, join_result, distance_threshold);

  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold);
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::execute_rsc_join(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {
  
  std::vector<unsigned int> reference_set = get_reference_set(
      trees_collection, distance_threshold
  );
  
  std::vector<std::vector<double>> lb_vectors(trees_collection.size(), std::vector<double>(reference_set.size()));
  std::vector<std::vector<double>> ub_vectors(trees_collection.size(), std::vector<double>(reference_set.size()));
  
  // Compute the vectors.
  compute_vectors(trees_collection, reference_set, lb_vectors, ub_vectors);
  
  // Retrieves candidates from the candidate index.
  retrieve_metric_candidates(trees_collection, candidates, join_result, distance_threshold, lb_vectors, ub_vectors);
  retrieve_sc_candidates(trees_collection, candidates, join_result, distance_threshold);

  // Verify all computed join candidates and return the join result.
  verify_candidates(trees_collection, candidates, join_result, distance_threshold);
};

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
void Guha<Label, CostModel, VerificationAlgorithm>::compute_vectors(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<unsigned int>& reference_set,
    std::vector<std::vector<double>>& lb_vectors,
    std::vector<std::vector<double>>& ub_vectors) {
  // For each tree in the collection compute the distance to every element
  // in the reference set. Store the values in a seperate vector for each tree.
  ted_lb::StringEditDistanceLB<Label, CostModel> sed_lb;
  ted_ub::ConstrainedUB<Label, CostModel> cted_ub;
  unsigned int data_tree_id = 0;
  unsigned int rs_tree_id = 0;
  for (auto t : trees_collection) {
    rs_tree_id = 0;
    for (auto t_id_rs : reference_set) {
      lb_vectors[data_tree_id][rs_tree_id] = sed_lb.sed_lb_ted(t, trees_collection[t_id_rs]);
      ub_vectors[data_tree_id][rs_tree_id] = cted_ub.cted_ub_ted(t, trees_collection[t_id_rs]);
      ++rs_tree_id;
    }
    ++data_tree_id;
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::retrieve_metric_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold,
    std::vector<std::vector<double>>& ted_vectors) {
  
  // For each ted vector pair, verify the triangle unequality lower bound
  // condition. If the condition is satisfied, add the pair to candidate set.
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
        candidates.push_back({v1_id, v2_id});
      }
    }
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::retrieve_metric_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold,
    std::vector<std::vector<double>>& lb_vectors,
    std::vector<std::vector<double>>& ub_vectors) {
  
  // For each ted vector pair, verify the triangle unequality lower bound
  // condition. If the condition is satisfied, add the pair to candidate set.
  double pair_l_t = 0;
  double pair_u_t = 0;
  for (unsigned int v1_id = 0; v1_id < lb_vectors.size(); ++v1_id) {
    for (unsigned int v2_id = v1_id+1; v2_id < lb_vectors.size(); ++v2_id) {
      // std::cout << v1_id << "," << v2_id << std::endl;
      pair_l_t = l_t(lb_vectors[v1_id], ub_vectors[v1_id], lb_vectors[v2_id], ub_vectors[v2_id]);
      if (pair_l_t <= distance_threshold) {
        ++l_t_candidates_;
        pair_u_t = u_t(ub_vectors[v1_id], ub_vectors[v2_id]);
        if (pair_u_t <= distance_threshold) {
          ++u_t_result_pairs_;
          join_result.emplace_back(v1_id, v2_id, pair_u_t);
          continue;
        }
        candidates.push_back({v1_id, v2_id});
      }
    }
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::retrieve_sc_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {
  
  // For each ted vector pair, verify the triangle unequality lower bound
  // condition. If the condition is satisfied, add the pair to candidate set.
  ted_lb::StringEditDistanceLB<Label, CostModel> sed_lb;
  ted_ub::ConstrainedUB<Label, CostModel> cted_ub;
  double pair_l_t = 0;
  double pair_u_t = 0;
  auto it = candidates.begin();
  while(it != candidates.end()) {
    pair_l_t = sed_lb.sed_lb_ted(trees_collection[it->first], trees_collection[it->second]);
    if (pair_l_t <= distance_threshold) {
      ++sed_candidates_;
      pair_u_t = cted_ub.cted_ub_ted(trees_collection[it->first], trees_collection[it->second]);
      if (pair_u_t <= distance_threshold) {
        ++cted_result_pairs_;
        join_result.emplace_back(it->first, it->second, pair_u_t);
        // Take the last candidate and substitute with thte current.
        // (remove the current candidate form candidates)
        *it = candidates.back();
        candidates.pop_back();
      } else {
        ++it;
      }
    } else {
      // Take the last candidate and substitute with thte current.
      // (remove the current candidate form candidates)
      *it = candidates.back();
      candidates.pop_back();
    }
  }
};

template <typename Label, typename CostModel, typename VerificationAlgorithm>
void Guha<Label, CostModel, VerificationAlgorithm>::verify_candidates(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<unsigned int, unsigned int>>& candidates,
    std::vector<join::JoinResultElement>& join_result,
    const double distance_threshold) {

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
std::vector<unsigned int> Guha<Label, CostModel, VerificationAlgorithm>::get_reference_set(
  std::vector<node::Node<Label>>& trees_collection,
  const double distance_threshold) {
    // Get sample size.
    // According to Guha, at least O(sqrt(|T|)*log(|T|)), where |T|=trees_collection.size().
    // We use 1*sqrt(|T|)*log(|T|).
    unsigned int collection_size = trees_collection.size();
    unsigned int sample_size = static_cast <unsigned int> (std::ceil(std::sqrt(collection_size) * std::log10(collection_size)));
    
    // std::cout << "sample_size = " << sample_size << std::endl;
    
    // Random generator.
    std::mt19937 rd(1); // Fix the seed.
    std::uniform_int_distribution<unsigned int> dist;
    
    // Draw the sample.
    // std::cout << "-- Draw the sample." << std::endl;
    std::vector<unsigned int> sample;
    dist = std::uniform_int_distribution<unsigned int>(0, collection_size-1);
    for (unsigned int i = 0; i < sample_size; ++i) {
      sample.push_back(dist(rd));
    }
    
    // std::cout << "sample.size() = " << sample.size() << std::endl;
    
    // Cluster the sample.
    // std::cout << "-- Cluster the sample." << std::endl;
    ted::APTED<Label, CostModel> ted_algorithm;
    std::vector<std::vector<unsigned int>> clusters;
    std::vector<unsigned int> remaining_sample(sample);
    std::vector<unsigned int> temp_remaining_sample;
    unsigned int sample_tree_id = 0;
    while (remaining_sample.size() > 0) {
      // std::cout << "remaining_sample.size() = " << remaining_sample.size() << std::endl;
      temp_remaining_sample.clear();
      dist = std::uniform_int_distribution<unsigned int>(0, remaining_sample.size()-1);
      sample_tree_id = remaining_sample[dist(rd)];
      // std::cout << "sample_tree_id = " << sample_tree_id << std::endl;
      std::vector<unsigned int> new_cluster;
      auto& sample_tree = trees_collection[sample_tree_id];
      for (auto tree_id : remaining_sample) {
        // Skip computing self distance between sample tree and itself.
        // Add the tree to the cluster.
        if (tree_id == sample_tree_id) {
          new_cluster.push_back(tree_id);
        } else {
          if (ted_algorithm.apted_ted(trees_collection[tree_id], sample_tree) <= std::ceil(distance_threshold / 2.0)) {
            new_cluster.push_back(tree_id);
          } else {
            temp_remaining_sample.push_back(tree_id);
          }
        }
      }
      // Push only clusters of size greater than one.
      if (new_cluster.size() > 1) {
        clusters.push_back(new_cluster);
      }
      remaining_sample = temp_remaining_sample;
    }
    // std::cout << "clusters.size() = " << clusters.size() << std::endl;
    
    // Sort the clusters by size.
    // std::cout << "-- Sort the clusters by size." << std::endl;
    std::sort(clusters.begin(), clusters.end(), [](const std::vector<unsigned int>& a, const std::vector<unsigned int>& b){ return a.size() > b.size(); });
    
    // Calculate k, but only if there are more than two clusters.
    // Otherwise k=2 due to k >= i >= 2.
    // std::cout << "-- Calculate k." << std::endl;
    unsigned int k = 2;
    if (clusters.size() > 2) {
      unsigned int i = 0;
      unsigned int cluster_size_sum = 0;
      double i_fraction = 0.0; // Right side of the formula.
      double fi = 1.0; // Denominator in the formula.
      cluster_size_sum += clusters[i].size();
      double fii = std::pow(1.0 - (double(cluster_size_sum) / double(sample_size)), 2); // Numerator in the formula.
      // std::cout << "cluster_size_sum = " << cluster_size_sum << std::endl;
      // std::cout << "fi = " << fi << std::endl;
      // std::cout << "fii = " << fii << std::endl;
      while ( (fii / fi) > i_fraction ) {
        ++i;
        // If there are no more clusters, break
        if (i > clusters.size() - 1) {
          break;
        }
        i_fraction = double(i) / (double(i) + 1.0);
        fi = std::pow(1.0 - (double(cluster_size_sum) / double(sample_size)), 2);
        cluster_size_sum += clusters[i].size();
        fii = std::pow(1.0 - (double(cluster_size_sum) / double(sample_size)), 2);
        // std::cout << "i_fraction = " << i_fraction << std::endl;
        // std::cout << "cluster_size_sum = " << cluster_size_sum << std::endl;
        // std::cout << "fi = " << fi << std::endl;
        // std::cout << "fii = " << fii << std::endl;
      }
      k = i;
    }
    // std::cout << "k = " << k << std::endl;
    
    // Choose k reference points, one from each k largest clusters.
    // std::cout << "-- Choose k reference points." << std::endl;
    std::vector<unsigned int> reference_set;
    for (unsigned int ci = 0; ci < k; ++ci) {
      dist = std::uniform_int_distribution<unsigned int>(0, clusters[ci].size()-1);
      reference_set.push_back(clusters[ci][dist(rd)]);
    }
    // std::string s("{");
    // for (auto e : reference_set) {
    //   s += std::to_string(e) + ",";
    // }
    // s.pop_back();
    // s += "}";
    // std::cout << "reference_set = " << s << std::endl;
    return reference_set;
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
double Guha<Label, CostModel, VerificationAlgorithm>::l_t(
  std::vector<double>& lb_v_i, std::vector<double>& ub_v_i,
  std::vector<double>& lb_v_j, std::vector<double>& ub_v_j) {
  double global_maximum = 0;
  double current_diff = 0;
  for (unsigned int l = 0; l < lb_v_i.size(); ++l) {
    if (lb_v_j[l] > ub_v_i[l]) {
      current_diff = lb_v_j[l] - ub_v_i[l];
    } else if (lb_v_i[l] > ub_v_j[l]) {
      current_diff = lb_v_i[l] - ub_v_j[l];
    } else {
      current_diff = 0.0;
    }
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
