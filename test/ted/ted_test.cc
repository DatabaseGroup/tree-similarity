#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "zhang_shasha_tree_index.h"
#include "apted_tree_index.h"
#include "tree_indexer.h"
#include "touzet_baseline_tree_index.h"
#include "touzet_depth_pruning_tree_index.h"
#include "touzet_depth_pruning_truncated_tree_fix_tree_index.h"
#include "touzet_kr_loop_tree_index.h"
#include "touzet_kr_set_tree_index.h"

// First argument's name omitted because not used.
int main(int, char** argv) {

  // Index test name.
  std::string ted_algorithm_name = std::string(argv[1]);

  // Type aliases.
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  
  // Initialise label dictionary - separate dictionary for each test tree
  // becuse it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialise cost model.
  CostModel ucm(ld);
  
  // Initialise a pointer to any TED algorithm.
  // TODO: It needs a tree index that is a superset of all indexes used by
  //       tested algorithms.
  //       Default algorithm's tree index doesn't work.
  ted::TEDAlgorithm<CostModel, node::TreeIndexAll>* ted_algorithm;
  
  // TODO: All algorithms need to be initialised.
  // Initialise ZS algorithm.
  ted::ZhangShashaTreeIndex<CostModel, node::TreeIndexAll> zhang_shasha_algorithm(ucm);
  // Initialise ZS algorithm.
  ted::APTEDTreeIndex<CostModel, node::TreeIndexAll> apted_algorithm(ucm);
  
  // Initialise Touzet algorithm.
  ted::TouzetBaselineTreeIndex<CostModel, node::TreeIndexAll> touzet_baseline_algorithm(ucm);
  ted::TouzetDepthPruningTreeIndex<CostModel, node::TreeIndexAll> touzet_depth_pruning_algorithm(ucm);
  ted::TouzetDepthPruningTruncatedTreeFixTreeIndex<CostModel, node::TreeIndexAll> touzet_depth_pruning_truncated_tree_fix_algorithm(ucm);
  ted::TouzetKRLoopTreeIndex<CostModel, node::TreeIndexAll> touzet_kr_loop_algorithm(ucm);
  ted::TouzetKRSetTreeIndex<CostModel, node::TreeIndexAll> touzet_kr_set_algorithm(ucm);

  // Assign ted algorithm by its name.
  if (ted_algorithm_name == "zhang_shasha") {
    ted_algorithm = &zhang_shasha_algorithm;
  }
  else if (ted_algorithm_name == "apted") {
    ted_algorithm = &apted_algorithm;
  }
  else if (ted_algorithm_name == "touzet_baseline") {
    ted_algorithm = &touzet_baseline_algorithm;
  }
  else if (ted_algorithm_name == "touzet_depth_pruning") {
    ted_algorithm = &touzet_depth_pruning_algorithm;
  }
  else if (ted_algorithm_name == "touzet_depth_pruning_truncated_tree_fix") {
    ted_algorithm = &touzet_depth_pruning_truncated_tree_fix_algorithm;
  }
  else if (ted_algorithm_name == "touzet_kr_loop") {
    ted_algorithm = &touzet_kr_loop_algorithm;
  }
  else if (ted_algorithm_name == "touzet_kr_set") {
    ted_algorithm = &touzet_kr_set_algorithm;
  }
  else {
    std::cerr << "Error while choosing TED algorithm to test. TED algorithm name = " +
        (ted_algorithm_name) + "." << std::endl;
    return -1;
  }
  
  // Initialise two tree indexes.
  // Use TreeIndexAll that is a superset of all algorithms' indexes.
  node::TreeIndexAll ti1;
  node::TreeIndexAll ti2;
  
  // Parse test cases from file.
  std::ifstream test_cases_file("ted_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  long long int sub_count = 0;
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
      
      // Index input trees.
      node::index_tree(ti1, t1, ld, ucm);
      node::index_tree(ti2, t2, ld, ucm);
      
      // Execute the algorithm.
      double computed_results = ted_algorithm->ted(ti1, ti2);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect TED result: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree_1_string << std::endl;
        std::cerr << input_tree_2_string << std::endl;
        return -1;
      }
      sub_count += ted_algorithm->get_subproblem_count();
    }
  }
  std::cout << "#sub = " << sub_count << std::endl;

  return 0;
}
