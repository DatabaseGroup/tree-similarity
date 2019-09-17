// The MIT License (MIT)
// Copyright (c) 2019 Mateusz Pawlik
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

#include <vector>
#include <memory>
#include "node.h"
#include "matrix.h"
#include "tree_indexer.h"
#include "ted_algorithm.h"

namespace ted {

/**
 * Interface for all Touzet algorithms.
 * 
 * Implements the tree edit distance algorithm by Helene Touzet [1].
 *
 * The algorithm requires parameter k, that is the upper bound for the number
 * of allowed structural modifications (deletions and insertions). If the real
 * TED value is below k, the algorithm returns the real TED value.
 *
 * If the value of k is unknown. The algorithm is executed multiple times,
 * each time doubling k, until the stopping condition is met.
 *
 * This is the memory-improved version. The algorithm stores the
 * intermediate results values around diagonals of matrices td_ and fd_. These
 * diagonals are shifted left to use a reduced number of columns. The resulting
 * memory complexity is O(nk) instead of O(n^2).
 *
 * The algorithm is implemented as it operates on rectangular matrices.
 * The translation of indices is done in BandMatrix,
 * transparently to the algorithm's implementation. One only has to make sure,
 * that iterating over the BandMatrix is done correctly.
 *
 * [1] H. Touzet. Comparing similar ordered trees in linear-time. Journal of
 *     Discrete Algorithms. 2007.
 *
 * NOTE: node::TreeIndexTouzetBaseline is the base TreeIndex for all Touzet
 *       algorithms. It is enough for TouzetBaselineTreeIndex. Other indices
 *       should/could inherit from it.
 */
template <typename CostModel, typename TreeIndex = node::TreeIndexTouzetBaseline>
class TEDAlgorithmTouzet : public TEDAlgorithm<CostModel, TreeIndex> {

public:

  // Base class members made visible for this class.
  using TEDAlgorithm<CostModel, TreeIndex>::TEDAlgorithm;
  using TEDAlgorithm<CostModel, TreeIndex>::c_;
  using TEDAlgorithm<CostModel, TreeIndex>::subproblem_counter_;
  using TEDAlgorithm<CostModel, TreeIndex>::ted;

  /// Implements ted function from the TEDAlgorithm<CostModel, TreeIndex> class.
  /**
   * Originally, Touzet's algorithm requires a TED upper bound value to
   * return the exact distance. Touzet mentions only an incremental way of
   * how to avoid that. A stopping condition is needed, but not given.
   *
   * The algorithm is executed multiple times with increasing values of k.
   * When k exceeds the returned distance, the algorithm terminates.
   */
  double ted(const TreeIndex& t1, const TreeIndex& t2) {

    // `+1` due to possible 0 size difference - then increase by mutliplication
    // doesn't work.
    // NOTE: This `+1` influences BandMatrix width.
    int k = std::abs(t1.tree_size_ - t2.tree_size_) + 1;

    // TODO: When computing ted without k, the matrices are newly initialised.
    //       Maybe resizing would be more efficient - but it needs to be
    //       implemented.

    double distance = ted_k(t1, t2, k);
    while (k < distance) {
      k = k * 2;
      // std::cerr << k << std::endl;
      // td_ = BandMatrix<double>(t1.tree_size_, k);
      // td_.Matrix::fill_with(std::numeric_limits<double>::infinity());
      // fd_ = BandMatrix<double>(t1.tree_size_ + 1, k + 1);
      // fd_.Matrix::fill_with(std::numeric_limits<double>::infinity());
      distance = ted_k(t1, t2, k);
    }
    return distance;
  };

  void init_matrices(int t1_size, int k) {
    using data_structures::BandMatrix;
    // NOTE: The default constructor of Matrix is called while constructing
    //       the algorithm object.
    // QUESTION: Shouldn't we implement Matrix::resize() instead of constructing
    //           matrix again?
    // NOTE: The k may be larger than |T2| that uses more memory than needed.
    td_ = BandMatrix<double>(t1_size, k);
    // Fill in for initially marking all subtree pairs as not eligable, and for debugging.
    // td_.fill_with(std::numeric_limits<double>::signaling_NaN());
    td_.Matrix::fill_with(std::numeric_limits<double>::infinity());
    // NOTE: The band_width=e for Touzet's fd_ matrix varies. It is however
    //       smaller or equal to the initialised band_width=k+1. As long as we
    //       read and write using the original band_width, addresses are not
    //       messed up. We only have to ensure that we do not iterate over too
    //       many elements using k instead of e.
    fd_ = BandMatrix<double>(t1_size + 1, k + 1);
    fd_.Matrix::fill_with(std::numeric_limits<double>::infinity());
  };

  /// Computes the tree edit distance given a maximum number of deletions and insertions.
  /**
   * If k is a proper upper bound, the result is the exact TED.
   * Otherwise, the result is a TED upper bounds. It may be infinity if
   * k is too low and there is no edit mapping with maximum number of k
   * deletions and insertions.
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param k Maximum number of allowed structural modifications (deletions,
   *          insertions).
   * \return Tree edit distance regarding k.
   */
  virtual double ted_k(const TreeIndex& t1, const TreeIndex& t2,
      const int k) = 0;

  /// Matrix storing subtree distances.
  data_structures::BandMatrix<double> td_;
  /// Matrix storing subforest distances.
  data_structures::BandMatrix<double> fd_;

  /// Calculates the tree edit distance between two subtrees.
  /**
   * It considers the remaining budget of errors, `e`, and not the input `k`.
   * Uses dynamic programming, with previously computed results stored in td_.
   * Itself it fills in fd_ matrix.
   *
   * This is implementation of the original Touzet's algorithm with
   * a modification to store intermediate subtree pair distances. Used for
   * the algorithm versions that remove redundancy by useng keyroot nodes.
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param x Postorder ID of a subtree in the source tree.
   * \param y Postorder ID of a subtree in the destination tree.
   * \param e The remaining budget of structural modifications for (x,y).
   */
  virtual double tree_dist(const TreeIndex& t1, const TreeIndex& t2, const int x,
      const int y, const int k, const int e) {
    int x_size = t1.postl_to_size_[x];
    int y_size = t2.postl_to_size_[y];

    // std::cout << "TEDAlgorithmTouzet::tree_dist" << std::endl;
    
    // Calculates offsets that lets us translate i and j to correct postorder
    // ids.
    int x_off = x - x_size;
    int y_off = y - y_size;
    
    double inf = std::numeric_limits<double>::infinity();

    // Initial cases.
    fd_.at(0, 0) = 0.0; // (0,0) is always within e-strip.
    for (int j = 1; j <= std::min(y_size, e); ++j) { // i = 0; only j that are within e-strip.
      fd_.at(0, j) = fd_.read_at(0, j - 1) + c_.ins(t2.postl_to_label_id_[j + y_off]);
    }
    if (e + 1 <= y_size) { // the first j that is outside e-strip
      fd_.at(0, e + 1) = inf;
    }

    for (int i = 1; i <= std::min(x_size, e); ++i) { // j = 0; only i that are within e-strip.
      fd_.at(i, 0) = fd_.read_at(i - 1, 0) + c_.del(t1.postl_to_label_id_[i + x_off]);
    }
    if (e + 1 <= x_size) { // the first i that is outside e-strip
      fd_.at(e + 1, 0) = inf;
    }

    // Variables used in the nested loops.
    double candidate_result;
    int i_forest;
    int j_forest;
    double fd_read;
    double td_read;
      
    // General cases.
    for (int i = 1; i <= x_size; ++i) {
      if (i - e - 1 >= 1) { // First j that is outside e-strip - can be read for the cell to the right.
        fd_.at(i, i - e - 1) = inf;
      }
      i_forest = i - t1.postl_to_size_[i + x_off];
      for (int j = std::max(1, i - e); j <= std::min(i + e, y_size); ++j) { // only (i,j) that are in e-strip
        ++subproblem_counter_;
        
        j_forest = j - t2.postl_to_size_[j + y_off];
        
        candidate_result = inf;
        candidate_result = std::min(candidate_result, fd_.read_at(i - 1, j) + c_.del(t1.postl_to_label_id_[i + x_off]));
        candidate_result = std::min(candidate_result, fd_.read_at(i, j - 1) + c_.ins(t2.postl_to_label_id_[j + y_off]));

        fd_read = inf;
        // If one of the forests is a tree, look up the vlaues in fd_.
        // Otherwise, both forests are trees and the fd-part is empty.
        if (i_forest != 0 || j_forest != 0) { // TODO: Swap if-else conditions or use '>0'.
          // QUESTION: Is it possible that we read from outside the band? - No exception thrown.
          td_read = inf;
          if (std::abs((i + x_off) - (j + y_off)) <= k) {
            // NOTE: If e is too large, TouzetKRLoop hits a bug here.
            //       Also below in writing to `td_`.
            //       `e` controlls only the loop over columns in `fd_`.
            //       A forest may need a distance of a subtree pair form outside
            //       the k-band. Similarly in writing.
            td_read = td_.read_at(i + x_off, j + y_off);
          }
          // If the values to read are outside of the band, they exceed
          // the threshold or are not present in the band-matrix.
          // Read the value from fd_ only if they exist and are in the band.
          // TODO: And otherwise? -> It's initialized to inf.
          if (std::max(0, i_forest - e - 1) <= j_forest &&
              j_forest <= std::min(i_forest + e + 1, y_size)) {
            fd_read = fd_.read_at(i_forest, j_forest);
          }
          candidate_result = std::min(candidate_result, fd_read + td_read);
        } else { // Pair of two subtrees.
          fd_read = fd_.read_at(i - 1, j - 1) + c_.ren(t1.postl_to_label_id_[i + x_off], t2.postl_to_label_id_[j + y_off]);
          candidate_result = std::min(candidate_result, fd_read);
          // Store the result only if it is within e budget.
          // Otherwise the inifinity value is already there from init.
          if (candidate_result <= e ) {
            if (std::abs((i + x_off) - (j + y_off)) <= k) {
              // NOTE: If e is too large, TouzetKRLoop hits a bug here.
              //       See the note above.
              td_.at(i + x_off, j + y_off) = candidate_result;
            }
          }
        }

        // None of the values in fd_ can be greater than e-value for this
        // subtree pair.
        if (candidate_result > e) {
          fd_.at(i, j) = inf;
        } else {
          fd_.at(i, j) = candidate_result;
        }
      }
      if (i + e + 1 <= y_size) { // Last j that is outside e-strip - can be read from the row below.
        fd_.at(i, i + e + 1) = inf;
      }
    }

    // The distance between two subtrees cannot be greater than e-value for these
    // subtrees.
    if (candidate_result > static_cast<double>(e)) {
      return inf;
    }
    return candidate_result;
  };

  /// Calculates a budget of the remaining number of errors.
  /**
   * The remaining deletions and insertions are left for the pair of subtrees
   * (T1_x,T2_y) after computing the lower bound for the nodes around them.
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param x Postorder ID of a subtree in the source tree.
   * \param y Postorder ID of a subtree in the destination tree.
   * \param k Original threshold for the number of structural modifications.
   * \return e(x,y) = k - |(|T1|-(x+1))-(|T2|-(y+1))| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
   */
  int e_budget(const TreeIndex& t1, const TreeIndex& t2, const int x, const int y, const int k) const {
    // Lower bound formula (k - RA - L):
    // e(x,y) = k - |(|T1|-(x+1))-(|T2|-(y+1))| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
    // New lower bound formula (k - R - A - L):
    // e(x,y) = k - |(|T1|-(x+1)-depth(x))-(|T2|-(y+1)-depth(y))| - |depth(x)-depth(y)| - |((x+1)-|T1_x|)-((y+1)-|T2_y|)|
    int x_size = t1.postl_to_size_[x];
    int y_size = t2.postl_to_size_[y];
    // int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
    //                   std::abs(((x+1) - x_size) - ((y+1) - y_size));
    int lower_bound = std::abs((t1.tree_size_ - (x+1) - t1.postl_to_depth_[x]) -
        (t2.tree_size_ - (y+1) - t2.postl_to_depth_[y])) +
        std::abs(t1.postl_to_depth_[x] - t2.postl_to_depth_[y]) +
        std::abs(((x+1) - x_size) - ((y+1) - y_size));
    return (k - lower_bound); // TODO: Verify this -> BUG: There is a bug here possibly returning a negative number.
  };

  /// Verifies if subtrees T1_x and T2_y are k-relevant.
  /**
   * T1_x and T2_y are k-relevant if
   * |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k.
   *
   * NOTE: x and y are increased by one due to node indexing starting with 0.
   *
   * \param t1 TreeIndex of source tree.
   * \param t2 TreeIndex of destination tree.
   * \param x postorder id of a node in source tree T1.
   * \param y postorder id of a node in destination tree T2.
   * \param k maximum number of structural canges.
   * \return True if subtrees T1_x and T2_y are k-relevant, and false otherwise.
   */
  bool k_relevant(const TreeIndex& t1, const TreeIndex& t2, const int x, const int y, const int k) const {
    // The lower bound formula (RA + D + L):
    // |(|T1|-(x+1))-(|T2|-(y+1))| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
    // New lower bound formula (R + A + D + L):
    // |(|T1|-(x+1)-depth(x))-(|T2|-(y+1)-depth(y))| + |depth(x)-depth(y)| + ||T1_x|-|T2_y|| + |((x+1)-|T1_x|)-((y+1)-|T2_y|)| < k
    int x_size = t1.postl_to_size_[x];
    int y_size = t2.postl_to_size_[y];
    // int lower_bound = std::abs((t1_size_.back() - (x+1)) - (t2_size_.back() - (y+1))) +
    //                   std::abs(x_size - y_size) +
    //                   std::abs(((x+1) - x_size) - ((y+1) - y_size));
    
    int lower_bound = std::abs((t1.tree_size_ - (x+1) - t1.postl_to_depth_[x]) -
        (t2.tree_size_ - (y+1) - t2.postl_to_depth_[y])) +
        std::abs(t1.postl_to_depth_[x] - t2.postl_to_depth_[y]) +
        std::abs(x_size - y_size) +
        std::abs(((x+1) - x_size) - ((y+1) - y_size));

    // NOTE: The pair (x,y) is k-relevant if lower_bound <= k.
    //       lower_bound < k is not correct because then (x,y) would be
    //       k-irrelevant for lower_bound = k. That would further mean that the
    //       subtrees T1_x and T2_y cannot be mapped with the given budget.
    if (lower_bound <= k) {
      return true;
    }
    return false;
  };

};

}