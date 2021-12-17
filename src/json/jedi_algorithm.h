// The MIT License (MIT)
// Copyright (c) 2021 Thomas Huetter
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

namespace json {

/**
 * Interface for all JEDI algorithms.
 */
template <typename CostModel, typename TreeIndex>
class JEDIAlgorithm {
public:
  /// Constructor. Takes cost model.
  JEDIAlgorithm(CostModel& c) : c_(c) {};
  
  /// Computes the tree edit distance between two trees.
  /**
   * \param t1 Source tree.
   * \param t2 Destination tree.
   * \return JSON edit distance value.
   */
  virtual double jedi(const TreeIndex& t1, const TreeIndex& t2) = 0;
  
  /// Computes the tree edit distance between two trees.
  /**
   * \param t1 Source tree.
   * \param t2 Destination tree.
   * \param threshold JSON similarity threshold.
   * \return JSON edit distance value.
   */
  virtual double jedi_k(const TreeIndex& t1, const TreeIndex& t2,
      const double threshold) = 0;
  
  /// Returns the number of subproblems encountered during JEDI computation.
  /**
   * \return The number of subproblems acountered in the last JEDI computation.
   */
  long long int get_subproblem_count() { return subproblem_counter_; };
  
  /// To initialise VerificationAlgorithm and TreeIndex variables in join algorithms.
  typedef CostModel AlgsCostModel;
  typedef TreeIndex AlgsTreeIndex;
  
protected:
  /// Cost model.
  const CostModel& c_;
  
  /// Subproblem counter - for experiments only.
  /**
   * Counts the number of non-trivial values filled in fd_ matrix: subproblems
   * where both forests are not empty.
   */
  long long int subproblem_counter_;
};

}
