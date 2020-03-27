// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik
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

namespace ted {

/**
 * Interface for all TED algorithms.
 */
template <typename CostModel, typename TreeIndex>
class TEDAlgorithm {
public:
  /// Constructor. Takes cost model.
  TEDAlgorithm(CostModel& c) : c_(c) {};
  
  /// Computes the tree edit distance between two trees.
  /**
   * \param t1 Source tree.
   * \param t2 Destination tree.
   * \return Tree edit distance value.
   */
  virtual double ted(const TreeIndex& t1, const TreeIndex& t2) = 0;
  
  /// Returns the number of subproblems encountered during TED computation.
  /**
   * \return The number of subproblems acountered in the last TED computation.
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
