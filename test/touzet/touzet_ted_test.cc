#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include <cmath>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "touzet.h"

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("touzet_ted_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise Touzet's algorithm.
  ted::Touzet<Label, CostModel> touzet_ted;

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::cout << "--- TEST ---" << std::endl;
      std::cout << line << std::endl;
      
      std::getline(test_cases_file, line);
      std::string source_tree = line;
      std::getline(test_cases_file, line);
      std::string destination_tree = line;
      std::getline(test_cases_file, line);
      int k_value = std::stoi(line);
      
      std::getline(test_cases_file, line);
      double expected_result = std::stod(line);

      parser::BracketNotationParser bnp;

      // Validate test tree.
      if (!bnp.validate_input(source_tree)) {
        std::cerr << "Incorrect format of input tree: '" << source_tree << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Validate test tree.
      if (!bnp.validate_input(destination_tree)) {
        std::cerr << "Incorrect format of input tree: '" << destination_tree << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Parse test tree.
      node::Node<Label> t1 = bnp.parse_single(source_tree);
      node::Node<Label> t2 = bnp.parse_single(destination_tree);

      // Execute the algorithm and get the result.
      double computed_result = touzet_ted.touzet_ted_depth_pruning_truncated_tree_fix(t1, t2, k_value);

      if (computed_result != expected_result) {
        std::cerr << "Incorrect ted result:\n" << std::to_string(computed_result) << "\ninstead of\n" << std::to_string(expected_result) << std::endl;
        std::cerr << "for input trees: " << source_tree << " | " << destination_tree << std::endl;
        std::cerr << "and k: " << k_value << std::endl;
        return -1;
      }
    }
  }

  return 0;
}