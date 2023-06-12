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

#pragma once

/// This is currently a copy of the previous version but with the efficient
/// tokanization.
template<class Label>
node::Node<Label> BracketNotationParser<Label>::parse_single(
    const std::string& tree_string) {

  // Tokenize the input string.
  std::vector<std::string> tokens = get_tokens(tree_string);

  if (tree_string.size() < 2) {
    throw std::invalid_argument("PARSER-ERROR: Tree string needs at least two characters.");
  }

  if (tokens[0] != "{") {
    throw std::invalid_argument("PARSER-ERROR: First character must be an opening bracket.");
  }

  // Metadata for parser to verify the input.
  int open_nodes = 1;

  // Get iterator over input tokens.
  auto tokens_begin = tokens.begin();
  auto tokens_end = tokens.end();

  // Deal with the root node separately.
  ++tokens_begin; // Advance tokens to label.
  std::string match_str = *tokens_begin;
  if (match_str == kLeftBracket) { // Root has an empty label.
    match_str = "";
    // Do not advance tokens - we're already at kLeftBracket or kRightBracket.
  } else if (match_str == kRightBracket) { // Node has an empty label.
    --open_nodes;
    match_str = "";
    ++tokens_begin; // Advance tokens.
  } else { // Non-empty label.
    ++tokens_begin; // Advance tokens.
  }
  Label root_label(match_str);
  node::Node<Label> root(root_label);
  node_stack.push_back(std::ref(root));

  // Iterate all remaining tokens.
  while (tokens_begin != tokens_end) {
    match_str = *tokens_begin;

    if (match_str == kLeftBracket) { // Enter node.
      // ++tokens_begin; // Advance tokens to label.
      match_str = *std::next(tokens_begin,1);
      ++open_nodes;

      if (match_str == kLeftBracket || match_str == kRightBracket) { // Node has an empty label.
        match_str = "";
        // Do not advance tokens - we're already at kLeftBracket or kRightBracket.
      } else { // Non-empty label.
        ++tokens_begin; // Advance tokens to get label.
        match_str = *tokens_begin;
      }

      // Create new node.
      Label node_label(match_str);
      node::Node<Label> n(node_label);

      // Move n to become a child.
      // Return reference from add_child to the 'new-located' object.
      // Put a reference to just-moved n (last child of its parent) on a stack.
      node_stack.push_back(std::ref(node_stack.back().get().add_child(n)));
    }
    else if (match_str == kRightBracket) { // Exit node.
      --open_nodes;
      if (open_nodes < 0) {
        throw std::invalid_argument("PARSER-ERROR: Found a closing bracket for missing opening bracket.");
      }
      node_stack.pop_back();
    }
    else {
      throw std::invalid_argument("PARSER-ERROR: Found unexpected token '" + match_str + "'.");
    }
    ++tokens_begin; // Advance tokens.
  }

  if (open_nodes > 0) {
    throw std::invalid_argument("PARSER-ERROR: There are opening brackets that are never closed.");
  }
  if (open_nodes < 0) {
    throw std::invalid_argument("PARSER-ERROR: Found a closing bracket for missing opening bracket.");
  }

  return root;
}

template<class Label>
void BracketNotationParser<Label>::parse_collection(
    std::vector<node::Node<Label>>& trees_collection,
    const std::string& file_path) {
  std::ifstream trees_file(file_path);
  if (!trees_file) {
    throw std::invalid_argument("PARSER-ERROR: Problem with opening the file '" + file_path + "' in BracketNotationParser::parse_collection_efficient.");
  }
  // Read the trees line by line, parse, and move into the container.
  std::string tree_string;
  while (std::getline(trees_file, tree_string)) {
    try {
      trees_collection.push_back(parse_single(tree_string)); // -> This invokes a move constructor (due to push_back(<rvalue>)).
    }
    catch(const std::exception& e) {
      std::cout << e.what();
    }
  }
  trees_file.close();
}

/// This is only a tokanizer that returns a vector with correct tokens.
template<class Label>
std::vector<std::string> BracketNotationParser<Label>::get_tokens(
    const std::string& tree_string) {
  std::vector<std::string> tokens;

  // Get pointer to the structure elements.
  const char* s_elems = kStructureElements.c_str();

  // Get pointer to the beginning of the input string.
  const char* begin = tree_string.c_str();
  // Pointer to the beginning of consecutive searches.
  const char* next_begin = begin;
  // Remember iter from previous iteration in old_iter - for label begin.
  const char* old_iter = begin;
  // iter is a pointer to consecutive occurences of either '{' or '}'.
  for(const char* iter = strpbrk(next_begin, s_elems); iter != NULL; iter = strpbrk(next_begin, s_elems)) {
    // Next iteration will start from the position right of iter.
    next_begin = iter + 1;
    // Check if the character just before the found position is an escape_char.
    // Then, disregard the current position.
    if (iter > begin && *(iter-1) == kEscapeChar) {
      continue;
    }
    // If there is something between two consecutive brackets, it's potentially
    // a label - record it.
    if (iter > old_iter + 1) {
      tokens.push_back(typename std::vector<std::string>::value_type(old_iter+1, iter)); // Calls the allocator of string.
    }
    // Record the found bracket.
    tokens.push_back(typename std::vector<std::string>::value_type(iter, iter+1)); // Calls the allocator of string.
    old_iter = iter;
  }

  return tokens;
}