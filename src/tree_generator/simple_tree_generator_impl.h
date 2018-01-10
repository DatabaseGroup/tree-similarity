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

/// \file tree_generator/simple_tree_generator_impl.h
///
/// \details
/// Contains the implementation of the SimpleTreeGenerator class.

#ifndef TREE_GENERATOR_SIMPLE_TREE_GENERATOR_IMPL_H
#define TREE_GENERATOR_SIMPLE_TREE_GENERATOR_IMPL_H

SimpleTreeGenerator::SimpleTreeGenerator() : alphabet_(generate_random_alphabet(1, 1, 0.5)) {}

std::string SimpleTreeGenerator::generate_tree(unsigned int tree_size) {

  std::random_device rd;

  std::uniform_int_distribution<int> labels_dist(0, alphabet_.size()-1);

  std::vector<std::string> labels;
  for (int i = 0; i < tree_size; ++i) {
    labels.push_back(alphabet_[labels_dist(rd)]);
  }

  std::vector<int> nodes(tree_size);
  nodes[tree_size - 1] += 1;

  std::vector<int> max_right(tree_size, tree_size - 1);

  std::uniform_int_distribution<int> right_dist;
  int right_id;
  for (int i = 1; i < tree_size; ++i) {
    right_dist = std::uniform_int_distribution<int>(i, max_right[i]);
    right_id = right_dist(rd);
    for (int desc = i+1; desc <= right_id; ++desc) {
      max_right[desc] = right_id;
    }
    nodes[right_id] += 1;
  }

  // Make a tree.
  std::string tree = "";
  for (int i = 0; i < tree_size; ++i) {
      tree += kLeftBracket + labels[i];
      for (int r = 0; r < nodes[i]; ++r) {
        tree += kRightBracket;
      }
  }

  return tree;
}

std::string SimpleTreeGenerator::modify_tree(std::string& tree_string, int tree_size, int num_edits) {
  // Start with renames.
  parser::BracketNotationParser bnp;
  std::vector<std::string> tokens = bnp.get_tokens(tree_string);

  // // Convert vector to string and print.
  // std::string s;
  // for (auto e : tokens) {
  //   s += e + ",";
  // }
  // s.pop_back(); // Delete the last coma
  // std::cout << s << std::endl;

  std::random_device rd;
  std::uniform_int_distribution<int> nodes_dist(0, tree_size-1);

  int num_renames = num_edits;

  for (int r = 0; r < num_renames; ++r) {
    int random_node_id = nodes_dist(rd);
    std::cout << "random node: " << random_node_id << std::endl;
    // Find {-token of random_node_id.
    int id = 0; // Equal to {-token of node with id=0.
    int cur_node_id = 0;
    for (int i = 0; i < tokens.size(); ++i) {
      if (cur_node_id == random_node_id) {
        break;
      }
      id = i;
      if (tokens[i] == "{") {
        ++cur_node_id;
      }
    }
    // Change the label - possibly the same.
    std::uniform_int_distribution<int> labels_dist(0, alphabet_.size()-1);
    tokens[id+1] = alphabet_[labels_dist(rd)];
  }

  std::string tree = "";
  for (auto t : tokens) {
      tree += t;
  }
  return tree;
}

std::vector<std::string> SimpleTreeGenerator::generate_random_alphabet(
    unsigned int tree_size, unsigned int label_length, double ratio) {
  std::vector<std::string> alphabet = {"a", "b", "c", "d", "e", "f"};
  return alphabet;
}

#endif // TREE_GENERATOR_SIMPLE_TREE_GENERATOR_IMPL_H
