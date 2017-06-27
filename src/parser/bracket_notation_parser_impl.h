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

#ifndef BRACKET_NOTATION_PARSER_IMPL_H
#define BRACKET_NOTATION_PARSER_IMPL_H

const node::Node<BracketNotationParser::Label>& BracketNotationParser::parse_string(
    const std::string& tree_string) {

  // Tokenize the input string - get iterator over tokens.
  auto tokens_begin = std::sregex_iterator(tree_string.begin(),
                                           tree_string.end(), kR);
  auto tokens_end = std::sregex_iterator();

  // Deal with the root node separately.
  ++tokens_begin; // Advance tokens to label.
  std::smatch match = *tokens_begin;
  std::string match_str = match.str();
  std::cout << "N:root:start" << std::endl;
  std::cout << "N:label = " << match_str << std::endl;
  node::Node<Label> root(Label(match_str.substr(1,match_str.size())));
  node_stack.push_back(std::ref(root)); // TODO: This passes root by value.
  // Not exactly, becuase std:ref()'s signature defines argument passed by reference.
  // Later std::ref() is called passing a reference to last child.

  ++tokens_begin; // Advance tokens to next node.

  // Sometimes get() has to be used on reference_wrapper to deduce the correct type.
  std::cout << "PARENT = " << node_stack.back().get().label().label() << std::endl;

  for (; tokens_begin != tokens_end; ++tokens_begin) {
    match = *tokens_begin;
    match_str = match.str();

    if (match_str == kLeftBracket) { // Enter node.
      ++tokens_begin; // Advance tokens to label.
      match = *tokens_begin;
      match_str = match.str();

      std::cout << "N:start" << std::endl;
      std::cout << "N:label = " << match_str << std::endl;

      // TODO: n is destroyed after each loop?!?!
      node::Node<Label> n(Label(match_str.substr(1,match_str.size())));

      std::cout << "PARENT = " << node_stack.back().get().label().label() << std::endl;

      // Move n to become a child.
      node_stack.back().get().add_child(std::move(n));

      // Put a reference to just-moved n (last child of its parent) on a stack.
      node_stack.push_back(std::ref(node_stack.back().get().get_last_childs_ref()));

      std::cout << "ADD_CHILD" << std::endl;
    }
    if (match_str == kRightBracket) { // Exit node.
      std::cout << "N:end" << std::endl;
      node_stack.pop_back();
    }
  }

  std::cout << "NODE_STACK_SIZE = " << node_stack.size() << std::endl;
  std::cout << "TREE_SIZE = " << root.get_tree_size() << std::endl;

  return std::move(root);
}

#endif // BRACKET_NOTATION_PARSER_IMPL_H
