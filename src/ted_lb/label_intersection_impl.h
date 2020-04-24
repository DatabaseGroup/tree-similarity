// The MIT License (MIT)
// Copyright (c) 2020 Thomas Huetter.
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

/// Contains the implementation of the Label Intersection algorithm.

#pragma once

template <typename CostModel, typename TreeIndex>
double LabelIntersection<CostModel, TreeIndex>::ted(
    const TreeIndex& t1, const TreeIndex& t2) {
  
  // Count the number of overlaping labels.
  unsigned long int label_int = 0;

  // Verify for each node of the first tree, whether it appears in the second.
  for (auto& element: t1.inverted_list_label_id_to_postl_) {
    auto tree2 = t2.inverted_list_label_id_to_postl_;
    std::unordered_map<int,std::vector<int>>::iterator it =
      tree2.find(element.first);
    // If the node with a certain label exists in both trees, the smaller
    // number is the intersection for the specific label.
    if (it != tree2.end()) {
      label_int += std::min(element.second.size(), tree2[element.first].size());
    }
  }

  // Return the label intersection which is the size of the larger tree minus
  // the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - label_int;
}
