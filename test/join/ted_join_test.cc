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
#include "tang_join_ti.h"
#include "guha_join_ti.h"
#include "bb_join_ti.h"
#include "histo_join_ti.h"
#include "dh_join_ti.h"
#include "lh_join_ti.h"
#include "ldh_join_ti.h"

// argc argument name omitted because not used.
int main(int, char** argv) {

  // Index test name.
  std::string ted_join_algorithm_name = std::string(argv[1]);
  // Input file name.
  std::string input_file_name = std::string(argv[2]);
  // Input results file name.
  std::string results_file_name = std::string(argv[3]);

  // Type aliases.
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  
  // Initialise label dictionary - separate dictionary for each test tree
  // becuse it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialise cost model.
  CostModel ucm(ld);
  
  // TODO: Read results from file.
  std::vector<size_t> results;
  std::ifstream results_file(results_file_name);
  std::size_t single_value = 0;
  for (std::string line; std::getline(results_file, line);) {
    sscanf(line.c_str(), "%zu", &single_value);
    results.push_back(single_value);
  }

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  
  // Parse the dataset.
  parser::BracketNotationParser bnp;
  bnp.parse_collection(trees_collection, input_file_name);

  int min_thres = 1;
  int max_thres = (int)results.size() - 1;
  int thres_step = 1;
  
  // Execute for different thresholds.
  // TODO: Naive should do only some thresholds: thresholds 1-16 take < 20sec each
  
  if (ted_join_algorithm_name == "naivejoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      join::NaiveJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      auto join_result = ted_join_algorithm.execute_join(trees_collection, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "tjoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>> sets_collection;
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::TJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection,
          sets_collection, candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "tangjoin") {
    // TODO: If TangJoinTI is initialized here, SIGSEGV is reported on label
    //       comparison in:
    //       #4  0x000055555557049b in join::TangJoinTI<label::StringLabel, ted::TouzetBaselineTreeIndex<cost_model::UnitCostModelLD<label::StringLabel>, node::TreeIndexTouzetBaseline> >::check_subgraphs (this=0x7fffffffdcf0, left_tree_node=0x555557f54450, right_tree_node=0x555555b73dc0) at /home/mpawlik/Remote/tree-similarity/src/join/tang/tang_join_ti_impl.h:172
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::unordered_set<std::pair<int, int>, join::hashintegerpair> candidates;
      std::vector<join::JoinResultElement> join_result;
      std::vector<node::BinaryNode<Label>> binary_trees_collection;
      join::TangJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection, binary_trees_collection,
          candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "guhajoin") {
    // NOTE: Three thresholds (1, 6, and 13) took ca. 270s. Thus, one threshold
    //       is fixed now, threshold 3, and takes ca. 97s.
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::GuhaJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_rsc_join(trees_collection, candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "bbjoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, std::unordered_map<int, int>>> histogram_collection;
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::BBJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection,
          histogram_collection, candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "hjoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, std::unordered_map<int, int>>> label_histogram_collection;
      std::vector<std::pair<int, std::unordered_map<int, int>>> degree_histogram_collection;
      std::vector<std::pair<int, std::unordered_map<int, int>>> leaf_distance_histogram_collection;
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::HJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection, label_histogram_collection, degree_histogram_collection, 
          leaf_distance_histogram_collection, candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "dhjoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, std::unordered_map<int, int>>> histogram_collection;
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::DHJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection, histogram_collection,
          candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "lhjoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, std::unordered_map<int, int>>> histogram_collection;
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::LHJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection, histogram_collection,
          candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  } else if (ted_join_algorithm_name == "ldhjoin") {
    for (int i = min_thres; i <= max_thres; i += thres_step) {
      std::vector<std::pair<int, std::unordered_map<int, int>>> histogram_collection;
      std::vector<std::pair<int, int>> candidates;
      std::vector<join::JoinResultElement> join_result;
      join::LDHJoinTI<Label, ted::TouzetBaselineTreeIndex<CostModel>> ted_join_algorithm;
      ted_join_algorithm.execute_join(trees_collection, histogram_collection,
          candidates, join_result, (double)i);
      if (join_result.size() != results[i - 1]) {
        std::cout << " ERROR Incorrect join result for threshold " << i << ": " <<
            join_result.size() << " instead of " << results[i - 1] << std::endl;
        return -1;
      }
    }
  }

  
  return 0;
}
