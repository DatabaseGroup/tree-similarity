#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"

int main() {
  using Label = label::StringLabel;

  // Correct result - currently hard-coded here.
  int correct_number_trees = 3;
  int correct_number_nodes = 1 + 13 + 20;

  std::string file_path = "parser_collection_size_test_data.txt";

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;

  parser::BracketNotationParser bnp;
  bnp.parse_collection(trees_collection, file_path);

  int computed_number_trees = trees_collection.size();
  int computed_number_nodes = 0;
  for (auto t : trees_collection) {
    computed_number_nodes += t.get_tree_size();
  }

  if (correct_number_trees != computed_number_trees) {
    std::cerr << "Incorrect number of trees in collection: " << computed_number_trees << " instead of " << correct_number_trees << std::endl;
    return -1;
  }

  if (correct_number_nodes != computed_number_nodes) {
    std::cerr << "Incorrect number of nodes in collection: " << computed_number_nodes << " instead of " << correct_number_nodes << std::endl;
    return -1;
  }

  return 0;
}
