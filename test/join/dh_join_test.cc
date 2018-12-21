#include <algorithm>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "dh_join.h"
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
  // std::string file_path = "dh_join_test_data.txt";
  std::string file_path = "/Users/huetterth/Documents/studies/phd/ted/ted-datasets/sentiment/sentiment_sorted.bracket";

  // Set distance threshold - maximum number of allowed edit operations.
  // double distance_threshold = 7.00;
  for (int i = 1; i < 16; i++) {
    std::vector<std::pair<unsigned int, std::unordered_map<unsigned int, unsigned int>>> histogram_collection;
    std::vector<std::pair<unsigned int, unsigned int>> candidates;
    std::vector<join::JoinResultElement> join_result;

    // Create the container to store all trees.
    std::vector<node::Node<Label>> trees_collection;

    // Parse the dataset.
    parser::BracketNotationParser bnp;
    bnp.parse_collection(trees_collection, file_path);

    // DHJoin with Touzet verification
    join::DHJoin<Label, CostModel, VerificationTouzet> dhjoin;
    dhjoin.execute_join(trees_collection, histogram_collection, candidates, join_result, (double)i);
    
    std::cout << "tau=" << i << ",\tpc=" << dhjoin.get_number_of_pre_candidates() << 
      ", cand=" << candidates.size() << ",\t|result|=" << join_result.size() << std::endl;
  }

  return 0;
}

