#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "touzet.h"

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("touzet_subproblem_count_depth_pruning_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise Touzet algorithm.
  touzet::Algorithm<Label, CostModel> touzet_ted;

  // Read test cases from a file line by line.
  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      // Read the single test case.
      std::getline(test_cases_file, line);
      std::string input_tree_1_string = line;
      std::getline(test_cases_file, line);
      std::string input_tree_2_string = line;
      std::getline(test_cases_file, line);
      int k = std::stoi(line);
      std::getline(test_cases_file, line);
      unsigned long long int correct_result = std::stoll(line);

      parser::BracketNotationParser bnp;

      // Validate test trees.
      if (!bnp.validate_input(input_tree_1_string)) {
        std::cerr << "Incorrect format of source tree: '" << input_tree_1_string << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      if (!bnp.validate_input(input_tree_2_string)) {
        std::cerr << "Incorrect format of destination tree: '" << input_tree_2_string << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Parse test tree.
      node::Node<Label> t1 = bnp.parse_single(input_tree_1_string);
      node::Node<Label> t2 = bnp.parse_single(input_tree_2_string);

      // Execute the algorithm WITH depth-based pruning.
      touzet_ted.touzet_ted(t1, t2, k, true);
      const unsigned long long computed_results = touzet_ted.get_subproblem_count();
      if (correct_result != computed_results) {
        std::cerr << "Incorrect subproblem count (WITH depth-based pruning): " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "T1: " << input_tree_1_string << std::endl;
        std::cerr << "T2: " << input_tree_2_string << std::endl;
        std::cerr << "k: " << k << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
