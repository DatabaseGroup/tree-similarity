// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik.
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

/// \file join/join_result_element.h
///
/// \details
/// Tree pair in the result set. Contains the IDs of both trees and their TED value.

#pragma once

namespace join {

/// This is an element of the join's result set.
struct JoinResultElement {
  /// Tree id of the left-hand tree in the result element.
  int tree_id_1;
  /// Tree id of the right-hand tree in the result element.
  int tree_id_2;
  /// Tree edit distance between tree_id_1 and tree_id_2.
  double ted_value;
  /// Constructor.
  JoinResultElement(int tree_id_1, int tree_id_2, double ted_value);
};

join::JoinResultElement::JoinResultElement(int tree_id_1, int tree_id_2, double ted_value)
    : tree_id_1(tree_id_1), tree_id_2(tree_id_2), ted_value(ted_value) {}

}
