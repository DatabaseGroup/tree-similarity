#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "lgm_tree_index.h"
#include "to_string_converters.h"

int main() {

  // Type aliases.
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;

  // Declare parser.
  parser::BracketNotationParser<Label> bnp;

  // Trees.
  node::Node<Label> t1;
  node::Node<Label> t2;
  
  // Initialise label dictionary - separate dictionary for each test tree
  // becuse it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialise cost model.
  CostModel ucm(ld);

  // Parse test cases from file.
  std::ifstream test_cases_file("lgm_ub_lb_mapping_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise String Edit Distance algorithm.
  ted_ub::LGMTreeIndex<CostModel, node::TreeIndexLGM> lgm_algorithm(ucm);
  
  // Initialise two tree indexes.
  // Use TreeIndexAll that is a superset of all algorithms' indexes.
  node::TreeIndexLGM ti1;
  node::TreeIndexLGM ti2;

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
      std::getline(test_cases_file, line);
      int k = std::stoi(line);
      std::getline(test_cases_file, line);
      std::string correct_result = line;

      // Parse test tree.
      try {
        t1 = bnp.parse_single(input_tree_1_string);
        t2 = bnp.parse_single(input_tree_2_string);
      }
      catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
      }

      // Index input trees.
      node::index_tree(ti1, t1, ld, ucm);
      node::index_tree(ti2, t2, ld, ucm);

      std::string computed_results;
      try {
        // Internals of LGM need to be initialized between test cases because
        // `ted_k` is not executed.
        lgm_algorithm.init(ti2);
        computed_results = common::vector_to_string(lgm_algorithm.lb_mapping(ti1, ti2, k));
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }

      if (correct_result != computed_results) {
        std::cerr << "Incorrect revised mapping: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "T1: " << input_tree_1_string << std::endl;
        std::cerr << "T2: " << input_tree_2_string << std::endl;
        std::cerr << "k: " << k << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
