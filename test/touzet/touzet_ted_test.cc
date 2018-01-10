#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "touzet.h"
#include "simple_tree_generator.h"
#include "zhang_shasha.h"

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("touzet_ted_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  parser::BracketNotationParser bnp;

  // Initialise Touzet algorithm.
  ted::Touzet<Label, CostModel> touzet_ted;

  // Initialise Zhang and Shsha's algorithm.
  ted::ZhangShasha<Label, CostModel> zs_ted;

  // Randomised test cases.

  // Initialise SimpleTreeGenerator and generate a dummy tree.
  tree_generator::SimpleTreeGenerator stg;
  // node::Node<Label> t1_gen;
  // node::Node<Label> t2_gen;
  std::string t1_gen_string;
  std::string t2_gen_string;
  double touzet_result;
  double zs_result;
  int k;
  for (int i = 0; i < 10; ++i) {
    t1_gen_string = stg.generate_tree(10);
    t2_gen_string = stg.modify_tree(t1_gen_string, 10, 3);
    std::cout << t1_gen_string << std::endl;
    std::cout << t2_gen_string << std::endl;
    node::Node<Label> t1_gen = bnp.parse_single(t1_gen_string);
    node::Node<Label> t2_gen = bnp.parse_single(t2_gen_string);
    try {
      zs_result = zs_ted.zhang_shasha_ted(t1_gen, t2_gen);
      k = int(std::ceil(zs_result));
      touzet_result = touzet_ted.touzet_ted(t1_gen, t2_gen, k);
      std::cout << k << std::endl;
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }

    if (zs_result != touzet_result) {
      std::cerr << "Incorrect TED result (Touzet vs ZS): " << touzet_result << " instead of " << zs_result << std::endl;
      std::cerr << "T1: " << t1_gen_string << std::endl;
      std::cerr << "T2: " << t2_gen_string << std::endl;
      std::cerr << "k: " << k << std::endl;
      return -1;
    }
  }

  // Fixed test cases.

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
      double correct_result = std::stod(line);

      // parser::BracketNotationParser bnp;

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

      // Execute the algorithm WITHOUT depth-based pruning.
      double computed_results;
      try {
        computed_results = touzet_ted.touzet_ted(t1, t2, k);
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }

      if (correct_result != computed_results) {
        std::cerr << "Incorrect TED result (WITHOUT depth-based pruning): " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "T1: " << input_tree_1_string << std::endl;
        std::cerr << "T2: " << input_tree_2_string << std::endl;
        std::cerr << "k: " << k << std::endl;
        return -1;
      }
      // // Execute the algorithm WITH depth-based pruning.
      // computed_results = touzet_ted.touzet_ted(t1, t2, k, true);
      // if (correct_result != computed_results) {
      //   std::cerr << "Incorrect TED result (WITH depth-based pruning): " << computed_results << " instead of " << correct_result << std::endl;
      //   std::cerr << "T1: " << input_tree_1_string << std::endl;
      //   std::cerr << "T2: " << input_tree_2_string << std::endl;
      //   std::cerr << "k: " << k << std::endl;
      //   return -1;
      // }
    }
  }

  return 0;
}
