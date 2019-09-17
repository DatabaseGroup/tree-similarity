#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <limits>
#include "to_string_converters.h"
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "tree_indexer.h"
#include "touzet_depth_pruning_truncated_tree_fix_tree_index.h"

// Verifies the correctness of the values stored in the td matrix after
// executing Touzet's algorithm. See also a NOTE below.
int main() {

  // Type aliases.
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  
  // Initialise label dictionary - separate dictionary for each test tree
  // becuse it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialise cost model.
  CostModel ucm(ld);

  // Parse test cases from file.
  // NOTE: Currently there are no NaN values in the matrices because it
  //       makes the computation easier. We initialise all the values to
  //       infinity and then modify only those of the relevant subtree
  //       pairs.
  //       The expected results using NaN values are in:
  //       'touzet_td_test_data_with_nan.txt'
  //       The expected results without memory improvement are in:
  //       'touzet_td_test_data_without_memory_improvement.txt'
  std::ifstream test_cases_file("touzet_td_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  // Initialise Touzet's algorithm.
  ted::TouzetDepthPruningTruncatedTreeFixTreeIndex<CostModel, node::TreeIndexAll> touzet_algorithm(ucm);

  // Initialise two tree indexes.
  // Use TreeIndexAll that is a superset of all algorithms' indexes.
  node::TreeIndexAll ti1;
  node::TreeIndexAll ti2;

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string source_tree = line;
      std::getline(test_cases_file, line);
      std::string destination_tree = line;
      std::getline(test_cases_file, line);
      int k_value = std::stoi(line);

      std::cout << "--- TEST ---" << std::endl;

      // Read td matrix
      std::string correct_result = "";
      std::getline(test_cases_file, line);
      if (line[0] != '{') {
        std::cerr << "Error with td matrix format." << std::endl;
        return -1;
      } else {
        std::getline(test_cases_file, line);
        while (line[0] != '}') {
          correct_result += line + "\n";
          std::getline(test_cases_file, line);
        }
        correct_result.pop_back();
      }

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

      // Index input trees.
      node::index_tree(ti1, t1, ld, ucm);
      node::index_tree(ti2, t2, ld, ucm);

      // Execute the algorithm to perform node indexing.
      // Tests td_ entries after touzet with depth-based pruning.
      touzet_algorithm.ted_k(ti1, ti2, k_value);

      std::string computed_results = common::matrix_to_string(touzet_algorithm.td_);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect values in the td matrix:\n" << computed_results << "\ninstead of\n" << correct_result << std::endl;
        std::cerr << "for input trees: " << source_tree << " | " << destination_tree << std::endl;
        std::cerr << "and k: " << k_value << std::endl;
        std::cerr << common::format_matrix_to_string(touzet_algorithm.td_) << std::endl;
        std::cerr << "*************************************************" << std::endl;
        std::cerr << common::format_matrix_to_string(touzet_algorithm.fd_) << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
