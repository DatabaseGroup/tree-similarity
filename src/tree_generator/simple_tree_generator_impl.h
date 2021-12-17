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

#pragma once

SimpleTreeGenerator::SimpleTreeGenerator() : alphabet_(generate_random_alphabet(1, 1, 0.5)) {}

std::string SimpleTreeGenerator::generate_tree(unsigned int tree_size) {

  std::mt19937 rd;

  std::uniform_int_distribution<int> labels_dist(0, alphabet_.size()-1);

  std::vector<std::string> labels;
  for (unsigned int i = 0; i < tree_size; ++i) {
    labels.push_back(alphabet_[labels_dist(rd)]);
  }

  std::vector<int> nodes(tree_size);
  nodes[tree_size - 1] += 1;

  std::vector<int> max_right(tree_size, tree_size - 1);

  std::uniform_int_distribution<int> right_dist;
  int right_id;
  for (unsigned int i = 1; i < tree_size; ++i) {
    right_dist = std::uniform_int_distribution<int>(i, max_right[i]);
    right_id = right_dist(rd);
    for (int desc = i+1; desc <= right_id; ++desc) {
      max_right[desc] = right_id;
    }
    nodes[right_id] += 1;
  }

  // Make a tree.
  std::string tree = "";
  for (unsigned int i = 0; i < tree_size; ++i) {
      tree += kLeftBracket + labels[i];
      for (int r = 0; r < nodes[i]; ++r) {
        tree += kRightBracket;
      }
  }

  return tree;
}

std::string SimpleTreeGenerator::modify_tree(std::string& tree_string, int tree_size, int num_edits) {

  if (num_edits == 0) {
    std::string tree;
    return tree.assign(tree_string);
  }

  // Tokenize tree and work on tokens.
  parser::BracketNotationParser bnp;
  std::vector<std::string> tokens = bnp.get_tokens(tree_string);

  // // Convert vector to string and print.
  // std::string s;
  // for (auto e : tokens) {
  //   s += e + ",";
  // }
  // s.pop_back(); // Delete the last coma
  // std::cout << s << std::endl;

  std::mt19937 rd;
  std::uniform_int_distribution<int> nodes_dist(0, tree_size-1);
  std::uniform_int_distribution<int> labels_dist(0, alphabet_.size()-1);
  std::uniform_int_distribution<int> edits_dist(1, num_edits); // At least one edit.

  int num_renames = edits_dist(rd);
  int num_deletions = num_edits - num_renames;

  int random_node_id;

  // Rename nodes.
  for (int r = 0; r < num_renames; ++r) {
    random_node_id = nodes_dist(rd);
    std::cout << "random node to RENAME: " << random_node_id << std::endl;
    // Find {-token of random_node_id.
    int id = 0; // Equal to {-token of node with id=0.
    int cur_node_id = 0;
    for (unsigned int i = 1; i < tokens.size(); ++i) {
      if (cur_node_id == random_node_id) {
        break;
      }
      if (tokens[i] == "{") {
        ++cur_node_id;
        id = i;
      }
    }
    // Change the label - possibly the same.
    tokens[id+1] = alphabet_[labels_dist(rd)];
  }

  // Delete nodes.
  for (int d = 0; d < num_deletions; ++d) {
    random_node_id = nodes_dist(rd);
    if (random_node_id == 0) ++random_node_id; // Root can't be deleted.
    std::cout << "random node to DELETE: " << random_node_id << std::endl;
    // Find {-token of random_node_id.
    int id_left = 0; // Equal to {-token of node with id=0.
    int cur_node_id = 0;
    for (unsigned int i = 1; i < tokens.size(); ++i) {
      if (cur_node_id == random_node_id) {
        break;
      }
      if (tokens[i] == "{") {
        ++cur_node_id;
        id_left = i;
      }
    }
    // Find }-token of random_node_id.
    // Do it by counting left and right brackets. When their numbers gets even,
    // we have the right bracket.
    int id_right = id_left; // id_right has to be right of id_left.
    int in_l = 1;
    int in_r = 0;
    for (unsigned int i = id_left+1; i < tokens.size(); ++i) {
      if (tokens[i] == "{") {
        ++in_l;
      } else if (tokens[i] == "}") {
        ++in_r;
      }
      if (in_l == in_r) {
        id_right = i;
        break;
      }
    }
    // Erase tokens of node to delete. This is enough, becuase afterwards
    // empty-string tokens are concatenated when the final string is composed.
    tokens[id_left] = "";
    tokens[id_left+1] = "";
    tokens[id_right] = "";
    // Update the distribution bounds.
    nodes_dist = std::uniform_int_distribution<int>(0, tree_size-1-(d+1));
  }

  // Compose the final tree by concatenating tokens.
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
