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

/// \file label/label_dictionary_impl.h
///
/// \details
/// Contains the implementation of the LabelDictionary class

#pragma once

template<class Label>
LabelDictionary<Label>::LabelDictionary() : labels_count_(0) {}

template<class Label>
int LabelDictionary<Label>::insert(const Label& l) {
  // Try to insert the given label with labels_count as its id value.
  // If the label is already in the dictionary, it (and its id) will not be
  // overwritten.
  auto label_in_dictionary = dictionary_.insert({l, labels_count_});
  // If the label wasn't in the dictionary, it was inserted.
  // Increase the labels count for consecutive labels.
  if (label_in_dictionary.second) {
    ++labels_count_;
  }
  // Return the id of the given label.
  return label_in_dictionary.first->second;
}

template<class Label>
void LabelDictionary<Label>::clear() {
  dictionary_.clear();
  labels_count_ = 0;
}

template<class Label>
int LabelDictionary<Label>::size() const {
  return dictionary_.size();
}
