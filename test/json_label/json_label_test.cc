#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <unordered_map>
#include "unit_cost_model.h"
#include "json_label.h"
#include "node.h"
#include "label_histogram_converter.h"
#include "label_intersection.h"
#include "bracket_notation_parser.h"
#include "tree_indexer.h"
#include "zhang_shasha_tree_index.h"

// argc argument name omitted because not used.
int main(int, char** argv) {

  // Input file name.
  std::string input_file_name = std::string(argv[1]);
  // Input results file name.
  std::string results_file_name = std::string(argv[2]);

  // Type aliases.
  using Label = label::JSONLabel;
  using CostModel = cost_model::UnitCostModelJSON<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  using LabelInt = ted_lb::LabelIntersection;
  using ZhangSTI = ted::ZhangShashaTreeIndex<CostModel, node::TreeIndexAll>;
  
  // Initialize label dictionary - separate dictionary for each test tree
  // because it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialize cost model.
  CostModel ucm(ld);
  ZhangSTI zhang_shasha_algorithm(ucm);
  LabelInt label_intersection_algorithm;
  
  // Read results from file.
  std::vector<size_t> results;
  std::ifstream results_file(results_file_name);
  std::size_t single_value = 0;
  for (std::string line; std::getline(results_file, line);) {
    sscanf(line.c_str(), "%zu", &single_value);
    results.push_back(single_value);
  }

  // Initialize two tree indexes.
  // Use TreeIndexAll that is a superset of all algorithms' indexes.
  node::TreeIndexAll ti1;
  node::TreeIndexAll ti2;

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  
  // Parse the dataset.
  parser::BracketNotationParser<Label> bnp;
  bnp.parse_collection(trees_collection, input_file_name);

  // Initialize collection that holds the label histograms.
  label_histogram_converter::Converter<Label> lhc;
  std::vector<std::pair<int, std::unordered_map<int, int>>> 
    histogram_collection;

  // Generate a histogram of labels 
  lhc.create_histogram(trees_collection, histogram_collection);

  // iterate through all histograms in the given collection
  // for (auto& histogram: histogram_collection) {
  for (unsigned int i = 1; i < histogram_collection.size(); i++) {
    // Compute the label intersection for two consecutive trees.
    unsigned long int label_int = label_intersection_algorithm.compute(
        histogram_collection[i], histogram_collection[i-1]);
    // Verify the result against the given solution.
    if (label_int != results[i - 1]) {
      std::cout << " ERROR Incorrect label intersection for trees " << i << 
          "and " << i+1 << ": " << label_int << " instead of " << 
          results[i - 1] << std::endl;
      return -1;
    }
  }
  
  return 0;
}
