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

/// Contains the implementation of the Touzet KR Set algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double TouzetKRSetTreeIndex<CostModel, TreeIndex>::ted_k(const TreeIndex& t1,
    const TreeIndex& t2, const int k) {
  
  const int t1_size = t1.tree_size_;
  const int t2_size = t2.tree_size_;
  
  init_matrices(t1_size, k);
  
  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  // If the pair of root nodes is not in k-strip (input tree size difference is
  // greater than k), return infinity.
  if (std::abs(t1_size - t2_size) > k) {
    return std::numeric_limits<double>::infinity();
  }
  
  // A map to store pairs of met keyroot nodes and an index to kr_vector where
  // the corresponding pair of nodes is stored.
  std::unordered_map<unsigned long long int, unsigned int> kr_pair_to_index;
  
  // NOTE: To make hashing more efficient, we pack a node pair into a single
  //       integer value.
  unsigned long long int key;
  
  /// Constant used in touzet_ted_kr_set for shifting bits in packing two
  /// values in a single long long (we need 32 bits for each value to pack in
  /// order to accomodate input tree sizes beyond 60K nodes).
  const short kBitsToShift = 32;
  
  // Vector to collect pairs of nodes for executing forest distance.
  std::vector<std::pair<int, int>> kr_vector;
    
  // Nested loop over all node pairs in k-strip : |x-y|<=k. This loop iterates
  // over all node pairs from k-strip, and verifies their k-relevancy.
  // The loop iterates in decreasing postorder ids.
  //
  // NOTE: Finding the first k-relevant node pair in this loop desn't ensure
  //       finding top relevant nodes on the left paths of their root nodes.
  //       y-nodes above the top_y last found have to be verified.
  //
  // For each k-relevant node pair in the k-strip, look up the corresponding
  // keyroot pair.
  // If we haven't seen the keroot pair before:
  //   store the (x,y) pair at the end of the kr_vector;
  //   insert the keyroot pair to the map with the kr_vector index as a value
  //     (to remember where is the current (top_x,top_y) pair of this keroot
  //     pair).
  // If we've seen the keroot pair before:
  //   look up their (top_x,top_y) pair in kr_vector;
  //   update the top_y to the max of the current y and the top_y stored in
  //     kr_vector.
  unsigned int kr_pair_index = 0;
  for (int x = t1_size - 1; x >= 0; --x) {
    int x_keyroot = t1.postl_to_kr_ancestor_[x];
    for (int y = std::min(x + k, t2_size-1); y >= std::max(0, x - k); --y) {
      if (k_relevant(t1, t2, x, y, k)) {
        // The casting is needed for 32 bits shift.
        key = ((unsigned long long int)x_keyroot << kBitsToShift) | (unsigned long long int)t2.postl_to_kr_ancestor_[y];
        // Try to insert {key, kr_pair_index} pair into kr_pair_to_index.
        auto kr_pair_search = kr_pair_to_index.insert({key, kr_pair_index});
        if (kr_pair_search.second) { // We meet this kr pair for the first time.
          // kr_pair_index already inserted to kr_pair_to_index.
          kr_vector.push_back({x, y});
          ++kr_pair_index; // Increment the index for the consecutive kr pairs.
        } else { // We've met this kr pair before.
          // Look up the (top_x,top_y) pair for the kr_pair.
          auto& kr_pair = kr_vector[kr_pair_search.first->second];
          // Update top_y.
          if (y > kr_pair.second) {
            kr_pair.second = y;
          }
        }
      }
    }
  }
  
  // Iterate the vector of node pairs backwards and execute forest distance.
  // This ensures correct order of keyroot nodes.
  typename std::vector<std::pair<int, int>>::reverse_iterator rit = kr_vector.rbegin();
  for (; rit != kr_vector.rend(); ++rit) {
    int x_l = rit->first;
    int y_l = rit->second;
    int e_max = e_budget(t1, t2, x_l, y_l, k);
    // // Get max e over node pairs on left paths.
    // // TODO: Whether to do it or not has to be evaluated. I don't remember
    // //       but I think it is kind of an optimization if e_max can be
    // //       smaller than k.
    // int e_max = k;
    // if (compute_e_max) {
    //   e_max = 0;
    //   int top_x = x_l;
    //   while (top_x > -1) {
    //     int top_y = y_l;
    //     while (top_y > -1) {
    //       e_max = std::max(e_max, e_budget(top_x, top_y, k));
    //       top_y = t2_lch_[top_y];
    //     }
    //     top_x = t1_lch_[top_x];
    //   }
    // }
    // Compute td(x, y) with e errors - the value of e(x, y, k).
    td_.at(x_l, y_l) = tree_dist(t1, t2, x_l, y_l, k, e_max);
  }
  
  return td_.read_at(t1_size-1, t2_size-1);
}