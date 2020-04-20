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

/// Contains the implementation of the label intersection algorithm.

#pragma once

unsigned long int LabelIntersection::compute(
    const std::pair<int, std::unordered_map<int, int>>& histo1, 
    const std::pair<int, std::unordered_map<int, int>>& histo2) {
  
  // Get the histogram out of the histogram object that contains the tree size.
  auto h2 = histo2.second;
  unsigned long int label_int = 0;

  // Verify for each element of one histogram, whether it exists in the other.
  for (auto& element: histo1.second) {
    std::unordered_map<int,int>::iterator it = h2.find(element.first);
    // If the element of the first histogram exists in the second one
    if (it != h2.end()) {
      label_int += std::min(element.second, h2[element.first]);
    }
  }

  // Return the label intersection which is the size of the larger tree minus
  // the number of nodes in common.
  return std::max(histo1.first, histo2.first) - label_int;
}
