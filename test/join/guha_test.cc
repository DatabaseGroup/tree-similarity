#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "guha.h"
#include "bracket_notation_parser.h"
#include "join_result_element.h"
#include "node.h"
#include "string_label.h"
#include "touzet.h"
#include "unit_cost_model.h"
#include "zhang_shasha.h"

int main() {
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;
  using VerificationTouzet = ted::Touzet<Label, CostModel>;

  // File path to input tree collection.
  std::string file_path = "join_test_data.txt";
  // Correct results for bolzano dataset (threshold 1 to 15).
  std::vector<unsigned int> results = {9, 37, 61, 109, 196, 344, 476, 596, 704, 840, 946, 1138, 1356, 1498, 1692};

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  // Parse the dataset.
  parser::BracketNotationParser bnp;
  bnp.parse_collection(trees_collection, file_path);
  
  // Execute for different thresholds.
  // Currently only threshold 1 because it takes too long (75sec).
  for (int i = 1; i < 2; i++) {
    std::vector<std::pair<unsigned int, unsigned int>> candidates;
    std::vector<join::JoinResultElement> join_result;

    // Guha with Touzet verification
    join::Guha<Label, CostModel, VerificationTouzet> guha;
    guha.execute_rsc_join(trees_collection, candidates, join_result, (double)i);
    
    if(join_result.size() != results[i-1]) {
      std::cout << " --- incorrect result for threshold " << i << ": " << join_result.size() 
        << " instead of " << results[i-1] << std::endl;
      return -1;
    }
  }

  return 0;
}

