#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "zhang_shasha.h"

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  // TODO: Do something that we don't have to give a relative path to the input file.
  std::ifstream test_cases_file("../../../test/zhang_shasha/ted_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise ZS algorithm.
  zhang_shasha::Algorithm<Label, CostModel> zs_ted;

  // Read test cases from a file line by line.
  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      // Read the single test case.
      std::getline(test_cases_file, line);
      std::string input_tree_1_string = line;
      std::getline(test_cases_file, line);
      std::string input_tree_2_string = line;
      std::getline(test_cases_file, line);
      double correct_result = std::stod(line);

      // Parse test tree.
      parser::BracketNotationParser bnp;
      node::Node<Label> t1 = bnp.parse_string(input_tree_1_string);
      node::Node<Label> t2 = bnp.parse_string(input_tree_2_string);

      // Execute the algorithm.
      double computed_results = zs_ted.zhang_shasha_ted(t1, t2);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect TED result: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree_1_string << std::endl;
        std::cerr << input_tree_2_string << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
