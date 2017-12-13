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

/// This is currently a copy of the previous version but with the efficient
/// tokanization.
node::Node<BracketNotationParser::Label> BracketNotationParser::parse_single_efficient(
    const std::string& tree_string) {

  std::vector<std::string> tokens = get_tokens_efficient(tree_string);

  // Tokenize the input string - get iterator over tokens.
  auto tokens_begin = tokens.begin();
  auto tokens_end = tokens.end();

  // Deal with the root node separately.
  ++tokens_begin; // Advance tokens to label.
  std::string match_str = *tokens_begin;
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
    match_str = *tokens_begin;

    if (match_str == kLeftBracket) { // Enter node.
      ++tokens_begin; // Advance tokens to label.
      match_str = *tokens_begin;

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

node::Node<BracketNotationParser::Label> BracketNotationParser::parse_single(
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

void BracketNotationParser::parse_collection_efficient(
    std::vector<node::Node<BracketNotationParser::Label>>& trees_collection,
    const std::string& file_path) {
  std::ifstream trees_file;
  trees_file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // Open the file.
    trees_file.open(file_path);
    // Read the trees line by line, parse, and move into the container.
    for (std::string tree_string; std::getline(trees_file, tree_string);) {
      trees_collection.push_back(parse_single_efficient(tree_string)); // -> This invokes a move constructor (due to push_back(<rvalue>)).
    }
    // Close the file.
    trees_file.close();
  } catch (std::ios_base::failure e) {
    std::cerr << "Exception opening/reading/closing file '" << file_path << "' in BracketNotationParser::parse_collection_efficient" << std::endl;
    std::cerr << "  +--> 'e.what()': " << e.what() << std::endl;
    std::cerr << "  +--> 'strerror(errno)': " << strerror(errno) << std::endl;
  }
}

void BracketNotationParser::parse_collection(
    std::vector<node::Node<BracketNotationParser::Label>>& trees_collection,
    const std::string& file_path) {
  // Open the file.
  std::ifstream trees_file(file_path);
  if (!trees_file) {
    // TODO: Error handling.
    // std::cerr << "ERROR: Problem reading the file." << std::endl;
  }
  // Read the trees line by line, parse, and move into the container.
  for (std::string tree_string; std::getline(trees_file, tree_string);) {
    trees_collection.push_back(parse_single(tree_string)); // -> This invokes a move constructor (due to push_back(<rvalue>)).
  }
}

/// This is only a tokanizer that returns a vector with correct tokens.
std::vector<std::string> BracketNotationParser::get_tokens_efficient(
    const std::string& tree_string) {
  std::vector<std::string> tokens;

  // Structure elements to search.
  char const* s_elems = "{}";
  // Escape character.
  char const* escape_char = "\\";

  // Get pointer to the beginning of the input string.
  char const* begin = tree_string.c_str();
  // Pointer to the beginning of consecutive searches.
  char const* next_begin = begin;
  // Start from the second character.
  char const* iter = begin;
  // Remember iter from previous iteration in old_iter - for label begin.
  char const* old_iter = begin;
  // iter is a pointer to consecutive occurences of either '{' or '}'.
  for(iter = strpbrk(next_begin, s_elems); iter != NULL; iter = strpbrk(next_begin, s_elems)) {
    // Next iteration will start from the position right of iter.
    next_begin = iter + 1;
    // Check if the character just before the found position is an escape_char.
    // Then, disregard the current position.
    if (iter > begin && *(iter-1) == *escape_char) {
      continue;
    }
    // If there is something between two consecutive brackets, it's potentially
    // a label - record it.
    if (iter > old_iter + 1) {
      tokens.push_back(typename std::vector<std::string>::value_type(old_iter+1, iter));
    }
    // Record the found bracket.
    tokens.push_back(typename std::vector<std::string>::value_type(iter, iter+1));
    old_iter = iter;
  }

  return tokens;
}

std::vector<std::string> BracketNotationParser::get_tokens(
    const std::string& tree_string) {
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
