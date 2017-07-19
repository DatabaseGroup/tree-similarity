#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "touzet.h"

/// Convert matrix of double values to its string representation.
/// Due to unit-cost model we output int values.
///
/// \param m Matrix of int values.
/// \return String representation of v.
const std::string matrix_to_string(const data_structures::Matrix<double>& m) {
  std::string s("{");
  for (int x = 0; x < m.get_rows(); ++x){
    for (int y = 0; y < m.get_columns(); ++y){
      double e = m.read_at(x, y);
      if (e == std::numeric_limits<double>::infinity()) {
        s += "@,";
      } else {
        std::string e_string = std::to_string(e);
        s += e_string.substr(0, e_string.find(".")) + ",";
      }
    }
  }
  s.pop_back();
  s += "}";
  return s;
}

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("td_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise Touzet's algorithm.
  touzet::Algorithm<Label, CostModel> touzet_ted;

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string source_tree = line;
      std::getline(test_cases_file, line);
      std::string destination_tree = line;
      std::getline(test_cases_file, line);
      int k_value = std::stoi(line);
      std::getline(test_cases_file, line);
      std::string correct_result = line;

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
      node::Node<Label> t1 = bnp.parse_string(source_tree);
      node::Node<Label> t2 = bnp.parse_string(destination_tree);

      // Execute the algorithm to perform node indexing.
      touzet_ted.touzet_ted(t1, t2, k_value);

      auto touzet_test_items = touzet_ted.get_test_items();

      std::string computed_results = matrix_to_string(touzet_test_items.td);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect values in the td matrix:\n" << computed_results << "\ninstead of\n" << correct_result << std::endl;
        std::cerr << "for input trees: " << source_tree << " | " << destination_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
