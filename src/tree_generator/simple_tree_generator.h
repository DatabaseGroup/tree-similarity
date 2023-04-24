// The MIT License (MIT)
// Copyright (c) 2018 Mateusz Pawlik.
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

/// \file tree_generator/simple_tree_generator.h
///
/// \details
/// Implements a simple tree generator that doesn't produce a tree structure.
/// It generates a correct bracket notation representation of a tree of
/// a specific given size.
///
/// Additionally, the generated trees can be modified by given number of edits.
/// Random nodes can be deleted or renamed. Insertion is not implemented.
///
/// Based on https://frosch.cosy.sbg.ac.at/mpawlik/tree-generator

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "../parser/bracket_notation_parser.h"

namespace tree_generator {

class SimpleTreeGenerator {
public:
  SimpleTreeGenerator();
  /// Generates a random tree with number of nodes equal to tree_size.
  /// NOTE: The shapes of generated trees are more to the right.
  ///
  /// \param tree_size Number of nodes in the generated tree.
  /// \return A tree in bracket notation.
  std::string generate_tree(unsigned int tree_size);
  /// Generates a random alphabet of labels.
  ///
  /// \param tree_size Number of nodes in the generated tree.
  /// \param label_length Number of characters in the labels.
  /// \param ratio = |alphabet| / tree_size.
  /// \return Vector of random string labels.
  std::vector<std::string> generate_random_alphabet(unsigned int tree_size,
      unsigned int label_length, double ratio);
  /// Modifies a tree passed in bracket notation with the given number of edit
  /// operations.
  /// Currently, rename and deletion is implemented.
  ///
  /// NOTE: The original tree is not modified.
  /// NOTE: A rename may not rename a node if the same label is chosen.
  /// NOTE: A deletion may delete a node that has been renamed that results
  ///       in one edit operation.
  ///
  /// \param tree_string Tree to modify in the bracket notation.
  /// \param tree_size Number of nodes in the tree.
  /// \param num_edits Number of edit operations to apply.
  /// \return A modified tree in bracket notation.
  std::string modify_tree(std::string& tree_string, int tree_size, int num_edits);
private:
  /// Structure brackets for representing nodes relationships.
  const std::string kLeftBracket = "{";
  const std::string kRightBracket = "}";
  /// List of labels to choose from for genrating trees.
  std::vector<std::string> alphabet_;
};

// Implementation details
#include "simple_tree_generator_impl.h"

};
