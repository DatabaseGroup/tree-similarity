#pragma once

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "tree_indexer.h"
#include "to_string_converters.h"
#include "touzet_baseline_tree_index.h"
#include "naive_join_ti.h"
#include "t_join_ti.h"

int main(int argc, char** argv) {

  // Index test name.
  std::string ted_join_algorithm_name = std::string(argv[1]);

  // Type aliases.
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  
  // Initialise label dictionary - separate dictionary for each test tree
  // becuse it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialise cost model.
  CostModel ucm(ld);
  
  // // Initialise a pointer to any TED join algorithm.
  // join::NaiveJoin<Label, ted::ZhangShashaTreeIndex<CostModel,
  //     node::TreeIndexZhangShasha>* ted_join_algorithm;
  // 
  // // Assign ted algorithm by its name.
  // if (ted_algorithm_name == "naive") {
  //   ted_join_algorithm = &zhang_shasha_algorithm;
  // } else {
  //   std::cerr << "Error while choosing TED join algorithm to test. TED join algorithm name = " +
  //       (ted_join_algorithm_name) + "." << std::endl;
  //   return -1;
  // }
  
  // Correct results for bolzano dataset (threshold 1 to 15).
  std::vector<unsigned int> results = {9, 37, 61, 109, 196, 344, 476, 596, 704, 840, 946, 1138, 1356, 1498, 1692};
  
  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  
  // Parse the dataset.
  parser::BracketNotationParser bnp;
  bnp.parse_collection(trees_collection, "join_test_data.txt");
  
  // join::NaiveJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;

  int min_thres = 1;
  int max_thres = 15;
  int thres_step = 5;
  
  // Execute for different thresholds.
  // TODO: Naive should do only some thresholds: thresholds 1-16 take < 20sec each
  
  if (ted_join_algorithm_name == "naive") {
    join::NaiveJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      auto join_result = ted_join_algorithm.execute_join(trees_collection, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "tjoin") {
    join::TJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<unsigned int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;
      std::vector<std::pair<unsigned int, unsigned int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      ted_join_algorithm.execute_join(trees_collection,
          sets_collection, candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  }
  
  return 0;
}
