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
/// Based on https://frosch.cosy.sbg.ac.at/mpawlik/tree-generator

#ifndef TREE_GENERATOR_SIMPLE_TREE_GENERATOR_H
#define TREE_GENERATOR_SIMPLE_TREE_GENERATOR_H

#include <iostream>
#include <vector>
#include <string>
#include <random>
#include "bracket_notation_parser.h"

namespace tree_generator {

class SimpleTreeGenerator {
public:
  SimpleTreeGenerator();
  std::string generate_tree(unsigned int tree_size);
  std::vector<std::string> generate_random_alphabet(unsigned int tree_size,
      unsigned int label_length, double ratio);
  std::string modify_tree(std::string& tree_string, int stree_size, int num_edits);
private:
  const std::string kLeftBracket = "{";
  const std::string kRightBracket = "}";
  std::vector<std::string> alphabet_;
};

// Implementation details
#include "simple_tree_generator_impl.h"

};

#endif // TREE_GENERATOR_SIMPLE_TREE_GENERATOR_H
