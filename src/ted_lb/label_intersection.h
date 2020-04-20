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

#pragma once

#include <vector>
#include <cmath>

namespace ted_lb {

/**
 * Implements the label intersection lower bound for trees. The parameters are
 * passed as label histograms as produced by 
 * label_histogram_converter::Converter<Label>.
 */
class LabelIntersection{

public:
  /// Given two label histograms, this function computes the label intersection.
  unsigned long int compute(
  	const std::pair<int, std::unordered_map<int, int>>& histo1, 
  	const std::pair<int, std::unordered_map<int, int>>& histo2);
};

// Implementation details.
#include "label_intersection_impl.h"

}
