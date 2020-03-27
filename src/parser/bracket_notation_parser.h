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
/// {LABEL{LABEL}{LABEL}}
///
/// where LABEL allows any character BUT the the structure brackets (by default
/// the curly brackets: {}) must be escaped as follows: \{ \}.
///
/// Example input with complex labels:
/// {a{\{[b],\{key:"value"\}\}{}}}
/// which is a path of three nodes with labels as follows:
///
///                         target LABEL (after removing escapes):
/// a                       -> a
/// |
/// \{[b],\{key:"value"\}\} -> {[b],{key:"value"}}
/// |
/// ""                      -> an empty label
/// TODO: Should the matched label be cleaned by removing escapes?
/// TODO: This parses to StringLabel only. Should the label's type be assigned
///       based on user's choice?

#pragma once

#include "node.h"
#include "string_label.h"

#include <iostream>
#include <fstream>
#include <string>
#include <regex>

namespace parser {

class BracketNotationParser {
// Types and type aliases
public:
  using Label = label::StringLabel;

// Member functions
public:
  /// Takes the string of a tree in bracket notation, parses it to the Node
  /// structure with StringLabels, and returns the root.
  ///
  /// \param tree_string The string holding the tree in bracket notation.
  ///
  /// \return Root of the parsed tree.
  node::Node<Label> parse_single(const std::string& tree_string);

  /// Takes a file with one tree (in bracket notation) per line and parses it
  /// to a vector of Node objects with StringLabels.
  ///
  /// NOTE: It executes parse_single for every line in the data file.
  /// NOTE: The notation of trees is assumed correct.
  ///
  /// \param trees_collection Container to store all trees.
  /// \param file_path The path to the file with set of trees.
  void parse_collection(std::vector<node::Node<Label>>& trees_collection,
      const std::string& file_path);

  /// Generates the tokens for the input string.
  ///
  /// \param tree_string The string holding the tree in bracket notation.
  /// \return Vector with all tokens.
  std::vector<std::string> get_tokens(const std::string& tree_string);

  /// Validates the bracket notation input.
  ///
  /// NOTE: This function could be merged with parse_string but this may
  ///       decrease readability.
  ///
  /// \param tree_string Tree in bracket notation.
  /// \return True if the input is correct and false otherwise.
  bool validate_input(const std::string& tree_string) const;
// Member variables
private:
  /// A stack to store nodes on a path to the root from the current node in the
  /// parsing process. Needed for maintaining correct parent-child relationships
  /// while parsing.
  std::vector<std::reference_wrapper<node::Node<Label>>> node_stack;

  /// Structure brackets for representing nodes relationships. Could be
  /// modified to other types of paretheses if necessary.
  const std::string kLeftBracket = "{";
  const std::string kRightBracket = "}";

  /// Structure elements of the bracket notation.
  const std::string kStructureElements = "{}";

  /// Escape character.
  const char kEscapeChar = '\\';

  /// A regex string to match left and right brackets.
  const std::string kMatchLeftBracket = "\\" + kLeftBracket;
  const std::string kMatchRightBracket = "\\" + kRightBracket;
};

// Implementation details
#include "bracket_notation_parser_impl.h"

}
