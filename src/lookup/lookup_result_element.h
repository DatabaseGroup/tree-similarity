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

/// \file lookup/lookup_result_element.h
///
/// \details
/// Tree pair in the result set. Contains the IDs of both trees and their 
/// JEDI value.

#ifndef TREE_SIMILARITY_LOOKUP_LOOKUP_RESULT_ELEMENT_IMPL_H
#define TREE_SIMILARITY_LOOKUP_LOOKUP_RESULT_ELEMENT_IMPL_H

namespace lookup {

/// This is an element of the join's result set.
struct LookupResultElement {
  /// Tree id of the query tree.
  int tree_id_1;
  /// Tree id of the result tree.
  int tree_id_2;
  /// Lower bound between tree_id_1 and tree_id_2.
  double lower_bound;
  /// Upper bound between tree_id_1 and tree_id_2.
  double upper_bound;
  /// JSON edit distance between tree_id_1 and tree_id_2.
  double jedi_value;
  /// Constructor.
  LookupResultElement(int tree_id_1, int tree_id_2, double lower_bound, 
      double upper_bound, double jedi_value);
};

lookup::LookupResultElement::LookupResultElement(int tree_id_1, int tree_id_2, 
    double lower_bound, double upper_bound, double jedi_value)
    : tree_id_1(tree_id_1), tree_id_2(tree_id_2), lower_bound(lower_bound), 
    upper_bound(upper_bound), jedi_value(jedi_value) {};

}

#endif // TREE_SIMILARITY_LOOKUP_LOOKUP_RESULT_ELEMENT_IMPL_H