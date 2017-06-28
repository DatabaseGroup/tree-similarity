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

/// \file parser/bracket_notation_parser.h
///
/// \details
/// Implements the parser for bracket notation of the form:
///
/// TODO: Decide on the format. It this ok, or we need something simpler.
///
/// {"LABEL"{"LABEL"}{"LABEL"}}
///
/// where LABEL allows any character between the quotes BUT the three characters
/// "{} must be escaped as follows: \" \{ \}.
///
/// Example input with complex labels:
/// std::string s("{\"a\"{\"\\{[b],\\{key:\\\"value\\\"\\}\\}\"{\"\"}}}");
/// which is a path of three nodes with labels as follows:
///
///                                target LABEL (after removing escapes):
/// "a"                         -> a
///  |
/// "\{[b],\{key:\"value\"\}\}" -> {[b],{key:"value"}}
///  |
/// ""                          -> an empty label
/// TODO: Should the matched label be cleaned by removing escapes?
/// TODO: This parses to StringLabel only. Should the label's type be assigned
///       based on user's choice?

#ifndef BRACKET_NOTATION_PARSER_H
#define BRACKET_NOTATION_PARSER_H

#include <iostream>
#include <string>
#include <regex>
#include "../node/node.h"
#include "../label/string_label.h"

namespace parser {

class BracketNotationParser {
// Types and type aliases
public:
  using Label = label::StringLabel;

// Member functions
public:
  /// Takes the string of a tree in bracket notation, parses it to the Node
  /// structure with StringLabels, and returns refeence to the root.
  ///
  /// \param tree_string The string holding the tree in bracket notation.
  /// \return Reference to the root node.
  const node::Node<Label>& parse_string(const std::string& tree_string);

// Member variables
private:
  /// A stack to store nodes on a path to the root from the current node in the
  /// parsing process. Needed for maintaining correct parent-child relationships
  /// while parsing.
  std::vector<std::reference_wrapper<node::Node<Label>>> node_stack;

  /// Brackets for representing nodes relationships. Could be modified to
  /// other types of paretheses if necessary.
  const std::string kLeftBracket = "{";
  const std::string kRightBracket = "}";

  /// A regex string to match left and right brackets.
  const std::string kMatchLeftBracket = "\\" + kLeftBracket;
  const std::string kMatchRightBracket = "\\" + kRightBracket;

  /// A regex string to match two quotes surrounding zero or more of:
  /// - any character that's not a quote or a backslash or a curly bracket,
  /// OR
  /// - a backslash followed by any character.
  const std::string kMatchStringLabel = "\"([^\"\\\\" + kLeftBracket
      + kRightBracket + "]|\\\\.)*\"";

  /// A regex to match either left bracket or label or right bracket.
  const std::regex kR = std::regex(kMatchLeftBracket + "|" + kMatchStringLabel
                                   + "|" + kMatchRightBracket);

// Member functions
private:
  // TODO: Implement verification of the input format!
};

// Implementation details
#include "bracket_notation_parser_impl.h"

}

#endif // BRACKET_NOTATION_PARSER_H
