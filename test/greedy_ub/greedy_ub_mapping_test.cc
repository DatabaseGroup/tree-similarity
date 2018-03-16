#include <iostream>
// #include <time.h>
// #include "timing.h"
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <random>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "simple_tree_generator.h"
#include "zhang_shasha.h"
#include "greedy_ub.h"

/// Convert vector of pairs of int values to its string representation.
///
/// \param v Vector of int values.
/// \return String representation of v.
const std::string vector_to_string(const std::vector<std::pair<int, int>>& v) {
  std::string s("{");
  for (auto e : v) {
    s += "(" + std::to_string(e.first) + "," + std::to_string(e.second) + "),";
  }
  s.pop_back();
  s += "}";
  return s;
}

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("greedy_ub_mapping_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  parser::BracketNotationParser bnp;

  // Initialise GreedyUB algorithm.
  ted_ub::GreedyUB<Label, CostModel> greedy_ub;

  // Initialise Zhang and Shsha's algorithm.
  ted::ZhangShasha<Label, CostModel> zs_ted;


  // Fixed test cases.

  std::cout << "--- FIXED TEST ---" << std::endl;

  // Read test cases from a file line by line.
  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {

      std::cout << "--- TEST CASE ---" << std::endl;
      std::cout << line << std::endl;

      // Read the single test case.
      std::getline(test_cases_file, line);
      std::string input_tree_1_string = line;
      std::getline(test_cases_file, line);
      std::string input_tree_2_string = line;
      // std::getline(test_cases_file, line);
      // int k = std::stoi(line);
      std::getline(test_cases_file, line);
      std::string correct_result = line;

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

      std::string computed_results;
      try {
        computed_results = vector_to_string(greedy_ub.greedy_mapping(t1, t2));
        std::cout << greedy_ub.greedy_mapping_cost(t1, t2) << std::endl;
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }

      if (correct_result != computed_results) {
        std::cerr << "Incorrect TED result (WITHOUT redundancy): " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "T1: " << input_tree_1_string << std::endl;
        std::cerr << "T2: " << input_tree_2_string << std::endl;
        // std::cerr << "k: " << k << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
