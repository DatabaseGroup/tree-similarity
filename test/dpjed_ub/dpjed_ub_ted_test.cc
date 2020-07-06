#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "dpjed_tree_index.h"

int main() {

  // Input file name.
  std::string input_file_name = std::string(argv[1]);
  // Input results file name.
  std::string results_file_name = std::string(argv[2]);

  // Type aliases.
  using Label = label::StringLabel;
  using JsonCM = cost_model::UnitCostModelJSON<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;
  using DpjedAlgo = ted_lb::LabelIntersection<JsonCM, node::TreeIndexDPJED>;
  
  // Initialize label dictionary - separate dictionary for each test tree
  // because it is easier to keep track of label ids.
  LabelDictionary ld;
  
  // Initialize cost model.
  JsonCM ucm(ld);
  DpjedAlgo dpjed_algorithm(ucm);

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
  node::TreeIndexDPJED ti1;
  node::TreeIndexDPJED ti2;

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;
  
  // Parse the dataset.
  parser::BracketNotationParser<Label> bnp;
  bnp.parse_collection(trees_collection, input_file_name);

  // iterate over all trees in the given collection
  for (unsigned int i = 1; i < trees_collection.size(); i++) {
    // Index input trees.
    node::index_tree(ti1, trees_collection[i], ld, ucm);
    node::index_tree(ti2, trees_collection[i-1], ld, ucm);

    // Compute the label intersection for two consecutive trees.
    double label_int = li_algorithm.ted(ti1, ti2);
    
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