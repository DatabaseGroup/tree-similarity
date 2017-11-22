#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "apted.h"

/// Convert vector of int values to its string representation.
///
/// \param v Vector of int values.
/// \return String representation of v.
const std::string vector_to_string(const std::vector<int>& v) {
  std::string s("{");
  for (auto e : v) {
    s += std::to_string(e) + ",";
  }
  s.pop_back();
  s += "}";
  return s;
}

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("apted_preL_to_size_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise Touzet's algorithm.
  ted::APTED<Label, CostModel> apted_ted;

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      std::string correct_result = line;

      // Parse test tree.
      parser::BracketNotationParser bnp;
      node::Node<Label> t1 = bnp.parse_single(input_tree);

      // Execute the algorithm to perform node indexing.
      apted_ted.apted_ted(t1, t1);

      auto apted_test_items = apted_ted.get_test_items();

      std::string computed_results = vector_to_string(apted_test_items.preL_to_size);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect subtree sizes result: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
