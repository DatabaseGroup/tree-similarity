#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "greedy_ub.h"

/// Convert vector of int values to is string representation.
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
  std::ifstream test_cases_file("greedy_ub_pre_to_post_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise GreedyUB algorithm.
  ted_ub::GreedyUB<Label, CostModel> greedy_ub;

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      std::string correct_result = line;

      parser::BracketNotationParser bnp;

      // Validate test tree.
      if (!bnp.validate_input(input_tree)) {
        std::cerr << "Incorrect format of input tree: '" << input_tree << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Parse test tree.
      node::Node<Label> t1 = bnp.parse_single(input_tree);

      // Execute the algorithm to perform node indexing.
      // The k-value does not matter for this test - this tests the correctness
      // of an index only.
      greedy_ub.lb_mapping(t1, t1, 1);

      auto greedy_ub_test_items = greedy_ub.get_test_items();

      std::string computed_results = vector_to_string(greedy_ub_test_items.t1_pre_to_post);

      if (correct_result != computed_results) {
        std::cerr << "Post to pre result: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
