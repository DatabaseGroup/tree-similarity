#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <fstream>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "to_string_converters.h"
#include "tree_indexer.h"

// TODO: Allow one argument which is the index test name.
int main(int argc, char** argv) {
  
  // Index test name.
  std::string index_test_name = std::string(argv[1]);
  
  // Indexes may use a vector or vector of vectors.
  // Map of index test name to index data type id.
  // 0 - std::vector<usigned int>
  // 1 - std::vector<std::vector<unsigned int>
  std::unordered_map<std::string, unsigned int> index_data_type_map = {
    {"postl_to_label_id_test", 0},
    {"postl_to_size_test", 0}
  };
  
  // Index data type.
  unsigned int index_data_type = index_data_type_map[index_test_name];
  
  // TODO: Initialise the TreeIndex with all indexes.
  //       Can be done here. Only the indexes are empty.
  node::TreeIndexAll tia;
  
  // A tree index is either a vector or vector of vectors.
  // We use a pointer because we can't use reference at this point.
  // v_index has to point to the updated index inside the test-cases loop.
  std::vector<unsigned int>* v_index;
  // std::vector<std::vector<unsigned int>>* vv_index;
  
  // Get a tree index by index test name.
  if (index_test_name == "postl_to_size_test") {
    v_index = &tia.postl_to_size_;
  } else if (index_test_name == "postl_to_label_id_test") {
    v_index = &tia.postl_to_label_id_;
  } else if (index_test_name == "postl_to_lld_test") {
    v_index = &tia.postl_to_lld_;
  } else if (index_test_name == "list_kr_test") {
    v_index = &tia.list_kr_;
  } else {
    std::cerr << "Error while choosing index to test. Index test name = " + (index_test_name) + "." << std::endl;
    return -1;
  }
  
  using Label = label::StringLabel;
  // Initialise label dictionary.
  label::LabelDictionary<Label> ld;

  // Parse test cases from file.
  std::ifstream test_data_file(index_test_name + "_data.txt");
  if (!test_data_file.is_open()) {
    std::cerr << "Error while opening file: " + (index_test_name + "_data.txt") + "." << std::endl;
    return -1;
  }
  
  // Iterate over all test cases from test_data_file.
  for (std::string line; std::getline( test_data_file, line);) {
    if (line[0] == '#') {
      // TODO: Make the format of test data file more flexible.
      std::getline(test_data_file, line);
      std::string input_tree = line;
      std::getline(test_data_file, line);
      std::string correct_result = line;
      
      parser::BracketNotationParser bnp;
      
      // Validate test tree.
      if (!bnp.validate_input(input_tree)) {
        std::cerr << "Incorrect format of input tree: '" << input_tree << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      
      // Parse test tree.
      node::Node<Label> tree = bnp.parse_single(input_tree);
      
      // Index the tree with all indexes.
      node::index_tree(tia, tree, ld);
      
      // Convert the computed results based on index output type.
      std::string computed_results;
      switch(index_data_type) {
        case 0 : computed_results = common::vector_to_string(*v_index);
          break;
        // TODO: vector of vectors.
        // case 1 : computed_results = common::vector_of_vectors_to_string(vv_index);
        //   break;
      }
      
      if (correct_result != computed_results) {
        std::cerr << "Incorrect " + index_test_name + " result: " <<
            computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
