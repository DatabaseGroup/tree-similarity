// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
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

/// \file label/string_label_impl.h
///
/// \details
/// Contains the implementation of the StringLabel class.

#ifndef TREE_SIMILARITY_LABEL_STRING_LABEL_IMPL_H
#define TREE_SIMILARITY_LABEL_STRING_LABEL_IMPL_H

StringLabel::StringLabel(const std::string& label) : label_(label) {}

// const std::string& StringLabel::label() const {
//   return label_;
// }

bool StringLabel::operator==(const StringLabel& other) const {
  return (label_.compare(other.to_string()) == 0);
}

const std::string& StringLabel::to_string() const {
  return label_;
}

#endif // TREE_SIMILARITY_LABEL_STRING_LABEL_IMPL_H
