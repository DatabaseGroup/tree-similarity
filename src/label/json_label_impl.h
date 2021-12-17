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

/// \file label/json_label_impl.h
///
/// \details
/// Contains the implementation of the JSONLabel class.

#pragma once

JSONLabel::JSONLabel(const std::string& label) {
  label_ = label;

  // Set type to 0 in case of an object. An object is indicated by opening
  // and closing curly braces.
  if (label.compare("\\{\\}") == 0) {
    type_ = 0;
  }
  // Set type to 1 in case of an array. An array is indicated by opening
  // and closing brackets.
  else if (label.compare("[]") == 0) {
    type_ = 1;
  }
  // Set type to 2 in case of an key. A key is indicated by a colon after
  // the key itself.
  else if (label.length() >= 2 && label[label.length() - 1] == ':') {
    type_ = 2;
  }
  // Otherwise, set type to 3 in case of a value.
  else {
    type_ = 3;
  }
}

unsigned int JSONLabel::get_type() const {
  return type_;
}

const std::string& JSONLabel::get_label() const {
  return label_;
}

bool JSONLabel::operator==(const JSONLabel& other) const {
  return (type_ == other.get_type() && label_.compare(other.get_label()) == 0);
}

const std::string& JSONLabel::to_string() const {
  return label_;
}