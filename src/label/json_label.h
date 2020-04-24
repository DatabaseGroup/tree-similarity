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

/// \file label/json_label.h
///
/// \details
/// Contains the declaration of the JSONLabel class (represents string and type
/// data which is associated with a JSON Node). 

#pragma once

#include <string>

namespace label {

/// \class JSONLabel
///
/// \details
/// Represents the (satellite) data a node of a JSON tree can hold. This class
/// only serves asencapsulation of this data and all operations on this data,
/// nothing else.
///
/// \remark
/// During the parsing process, the types keys and values cannot 
class JSONLabel {
public:
  JSONLabel(const std::string& label);

  /// Operator overloadings.
  /// @{
  bool operator==(const JSONLabel& other) const;
  /// @}

  /// Return the type field of the label.
  unsigned int get_type() const;

  /// Return the label field of the label.
  const std::string& get_label() const;

  /// Generates a string representation of the label.
  ///
  /// \return String representation of the label.
  const std::string& to_string() const;
private:
  /// The label associated with a node.
  std::string label_{};
  /// The type associated with a node. An object is encoded as 0, an array as 
  /// 1, a key as 2, and a value as 3.
  unsigned int type_{};
};

// Implementation details
#include "json_label_impl.h"

} // namespace label

// Custom specialization of std::hash - injected in namespace std.
// This is needed for LabelDictionary.
namespace std
{
  template<> struct hash<label::JSONLabel> {
    typedef label::JSONLabel argument_type;
    typedef std::size_t result_type;
    result_type operator()(argument_type const& s) const noexcept {
        result_type const h ( std::hash<std::string>{}(s.to_string() + "hash" + 
            std::to_string(s.get_type())) );
        return h;
    }
  };
}
