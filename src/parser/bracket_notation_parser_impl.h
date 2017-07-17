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

/// \file parser/bracket_notation_parser_impl.h
///
/// \details
/// Contains the implementation of the BracketNotationParser class.

#ifndef TREE_SIMILARITY_PARSER_BRACKET_NOTATION_PARSER_IMPL_H
#define TREE_SIMILARITY_PARSER_BRACKET_NOTATION_PARSER_IMPL_H

const node::Node<BracketNotationParser::Label> BracketNotationParser::parse_string(
    const std::string& tree_string) {

  // Tokenize the input string - get iterator over tokens.
  auto tokens_begin = std::sregex_iterator(tree_string.begin(),
                                           tree_string.end(), kR);
  auto tokens_end = std::sregex_iterator();

  // Deal with the root node separately.
  ++tokens_begin; // Advance tokens to label.
  std::smatch match = *tokens_begin;
  std::string match_str = match.str();
  if (match_str == kLeftBracket || match_str == kRightBracket) { // Root has an empty label.
    match_str = "";
    // Do not advance tokens - we're already at kLeftBracket or kRightBracket.
  } else { // Non-empty label.
    ++tokens_begin; // Advance tokens.
  }
  Label root_label(match_str);
  node::Node<Label> root(root_label);
  node_stack.push_back(std::ref(root));

  // Iterate all remaining tokens.
  while (tokens_begin != tokens_end) {
    match = *tokens_begin;
    match_str = match.str();

    if (match_str == kLeftBracket) { // Enter node.
      ++tokens_begin; // Advance tokens to label.
      match = *tokens_begin;
      match_str = match.str();

      if (match_str == kLeftBracket || match_str == kRightBracket) { // Node has an empty label.
        match_str = "";
        // Do not advance tokens - we're already at kLeftBracket or kRightBracket.
      } else { // Non-empty label.
        ++tokens_begin; // Advance tokens.
      }

      // Create new node.
      Label node_label(match_str);
      node::Node<Label> n(node_label);

      // Move n to become a child.
      // Return reference from add_child to the 'new-located' object.
      // Put a reference to just-moved n (last child of its parent) on a stack.
      node_stack.push_back(std::ref(node_stack.back().get().add_child(n)));
    }

    if (match_str == kRightBracket) { // Exit node.
      node_stack.pop_back();
      ++tokens_begin; // Advance tokens.
    }
  }
  return root;
}

const std::vector<std::string> BracketNotationParser::get_tokens(const std::string& tree_string) {
  std::vector<std::string> tokens;

  // Tokenize the input string - get iterator over tokens.
  auto tokens_begin = std::sregex_iterator(tree_string.begin(),
                                           tree_string.end(), kR);
  auto tokens_end = std::sregex_iterator();

  // Iterate all tokens and collect.
  for (std::sregex_iterator i = tokens_begin; i != tokens_end; ++i) {
    std::smatch match = *i;
    std::string match_str = match.str();
    tokens.push_back(match_str);
    std::cout << match_str << std::endl;
  }

  return tokens;
}

bool BracketNotationParser::validate_input(const std::string& tree_string) const {
  int bracket_diff_counter = 0; // Counts difference between the numbers of left and right brackets.
  int bracket_pair_counter = 0; // Counts number of bracket pairs - number of nodes assuming correct nesting.
  // Loop over all characters.
  for(auto it = tree_string.begin(); it != tree_string.end(); ++it) {
    if (*it == kEscapeChar) { // Skip next character if kEscapeChar is found.
      ++it;
    } else if (*it == kLeftBracket[0]) { // Increase bracket_counter when kLeftBracket found.
      bracket_diff_counter++;
      bracket_pair_counter++;
    } else if (*it == kRightBracket[0]) { // Decrease bracket_counter when kRightBracket found.
      bracket_diff_counter--;
    }
  }
  if (bracket_diff_counter != 0 || bracket_pair_counter == 0) {
    return false;
  }
  return true;
};

#endif // TREE_SIMILARITY_PARSER_BRACKET_NOTATION_PARSER_IMPL_H
