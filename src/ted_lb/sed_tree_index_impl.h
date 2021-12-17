// The MIT License (MIT)
// Copyright (c) 2018 Nikolaus Augsten, Mateusz Pawlik.
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

/// Contains the implementation of the String Edit Distance algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double SEDTreeIndex<CostModel, TreeIndex>::ted(const TreeIndex& t1, const TreeIndex& t2) {
      
  // Reset subproblem counter.
  subproblem_counter_ = 0;
  
  // TODO: Swap input trees for memory optimisation, as below.
  // if (x.length() > y.length()) {
  //   String tmp = x;
  //   x = y;
  //   y = tmp;
  // }
  const int kT1Size = t1.tree_size_;
  const int kT2Size = t2.tree_size_;
  
  std::vector<double> cp(kT2Size + 1);
  std::vector<double> cc(kT2Size + 1);
  
  // Edit distance on postorder traversal.
  for (int j = 0; j <= kT2Size; ++j) {
    cp[j] = j;
  }
  for (int i = 1; i <= kT1Size; ++i) {
    cc[0] = i;
    for (int j = 1; j <= kT2Size; ++j) {
      ++subproblem_counter_;
      unsigned int c1 = t1.postl_to_label_id_[i - 1];
      unsigned int c2 = t2.postl_to_label_id_[j - 1];
      cc[j] = std::min({
          cp[j - 1] + ((c1 == c2) ? 0 : 1),
          cc[j - 1] + 1,
          cp[j] + 1
          });
    }
    std::swap(cp, cc);
  }
  double sed_post = cp[kT2Size];
  
  // Edit distance on preorder traversal.
  for (int j = 0; j <= kT2Size; ++j) {
    cp[j] = j;
  }
  for (int i = 1; i <= kT1Size; ++i) {
    cc[0] = i;
    for (int j = 1; j <= kT2Size; ++j) {
      ++subproblem_counter_;
      unsigned int c1 = t1.prel_to_label_id_[i - 1];
      unsigned int c2 = t2.prel_to_label_id_[j - 1];
      cc[j] = std::min({
          cp[j - 1] + ((c1 == c2) ? 0 : 1),
          cc[j - 1] + 1,
          cp[j] + 1
          });
    }
    std::swap(cp, cc);
  }
  double sed_pre = cp[kT2Size];
  
  return std::max(sed_post, sed_pre); // note: cc is asigned to cp, cc is overwritten!
}
