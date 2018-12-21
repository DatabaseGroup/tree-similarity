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

/// \file join/guha/guha.h
///
/// \details
/// Implements partially the join algorithm by Guha [1].
///
/// [1] Guha et al. Integrating XML data sources using approximate joins. 
///     ACM TODS 2006.

#ifndef TREE_SIMILARITY_JOIN_GUHA_GUHA_H
#define TREE_SIMILARITY_JOIN_GUHA_GUHA_H

#include <vector>
#include <random>
#include "join_result_element.h"
#include "node.h"
#include "zhang_shasha.h"

namespace join {

template <typename Label, typename CostModel, typename VerificationAlgorithm>
class Guha {
// Member functions.
public:
  /// Constructor.
  Guha();
  /// TODO
  void execute_join(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      std::vector<join::JoinResultElement>& join_result,
      const double distance_threshold,
      unsigned int reference_set_size
  );
  /// TODO
  void retrieve_candidates(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      const double distance_threshold,
      std::vector<unsigned int>& reference_set,
      std::vector<std::vector<double>>& ted_vectors
  );
  /// TODO
  void verify_candidates(
      std::vector<node::Node<Label>>& trees_collection,
      std::vector<std::pair<unsigned int, unsigned int>>& candidates,
      std::vector<join::JoinResultElement>& join_result, 
      const double distance_threshold,
      std::vector<std::vector<double>>& ted_vectors
  );
  /// TODO
  std::vector<unsigned int> get_random_reference_set(
    std::vector<node::Node<Label>>& trees_collection,
    unsigned int k
  );
  /// TODO
  double u_t(std::vector<double>& v_1, std::vector<double>& v_2);
  /// TODO
  double l_t(std::vector<double>& v_1, std::vector<double>& v_2);
};

// Implementation details.
#include "guha_impl.h"

}

#endif // TREE_SIMILARITY_JOIN_GUHA_GUHA_H
