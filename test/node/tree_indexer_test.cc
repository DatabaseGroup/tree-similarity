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

// argc argument name omitted because not used.
// TODO: Allow one argument which is the index test name.
int main(int, char** argv) {
  
  // Index test name.
  std::string index_test_name = std::string(argv[1]);
  
  // TODO: Initialise the TreeIndex with all indexes.
  //       Can be done here. Only the indexes are empty.
  node::TreeIndexAll tia;
  
  // A tree index is either a vector or vector of vectors.
  // We use a pointer because we can't use reference at this point.
  // v_index has to point to the updated index inside the test-cases loop.
  std::vector<int>* v_index;
  std::vector<std::vector<int>>* vv_index;
  std::vector<bool>* v_bool_index;
  std::vector<double>* v_double_index;
  std::vector<long long int>* v_ll_index;
  std::unordered_map<int, std::vector<int>>* m_ivi_index;
  
  // Index data type:
  const int kVInt = 0; // std::vector<int> (default)
  const int kVVInt = 1; // std::vector<std::vector<int>>
  const int kVBool = 2; // std::vector<bool>
  const int kVDouble = 3; // std::vector<double>
  const int kVLLInt = 4; // std::vector<long long int>
  const int kMIntVInt = 5; // std::unordered_map<int, std::vector<int>>
  int index_data_type = kVInt;
  // TODO: Change index_data_type in an if statement below if needed.
  
  // Get a tree index by index test name.
  if (index_test_name == "postl_to_size_test") {
    v_index = &tia.postl_to_size_;
  } else if (index_test_name == "prel_to_size_test") {
    v_index = &tia.prel_to_size_;
  } else if (index_test_name == "postl_to_parent_test") {
    v_index = &tia.postl_to_parent_;
  } else if (index_test_name == "prel_to_parent_test") {
    v_index = &tia.prel_to_parent_;
  } else if (index_test_name == "postl_to_prel_test") {
    v_index = &tia.postl_to_prel_;
  } else if (index_test_name == "prel_to_postl_test") {
    v_index = &tia.prel_to_postl_;
  } else if (index_test_name == "prel_to_prer_test") {
    v_index = &tia.prel_to_prer_;
  } else if (index_test_name == "prer_to_prel_test") {
    v_index = &tia.prer_to_prel_;
  } else if (index_test_name == "prel_to_postr_test") {
    v_index = &tia.prel_to_postr_;
  } else if (index_test_name == "postr_to_prel_test") {
    v_index = &tia.postr_to_prel_;
  } else if (index_test_name == "postl_to_children_test") {
    vv_index = &tia.postl_to_children_;
    index_data_type = kVVInt;
  } else if (index_test_name == "prel_to_children_test") {
    vv_index = &tia.prel_to_children_;
    index_data_type = kVVInt;
  } else if (index_test_name == "prel_to_label_id_test") {
    v_index = &tia.prel_to_label_id_;
  } else if (index_test_name == "postl_to_label_id_test") {
    v_index = &tia.postl_to_label_id_;
  } else if (index_test_name == "postr_to_label_id_test") {
    v_index = &tia.postr_to_label_id_;
  } else if (index_test_name == "postl_to_lld_test") {
    v_index = &tia.postl_to_lld_;
  } else if (index_test_name == "prel_to_lld_test") {
    v_index = &tia.prel_to_lld_;
  } else if (index_test_name == "prel_to_rld_test") {
    v_index = &tia.prel_to_rld_;
  } else if (index_test_name == "postr_to_rld_test") {
    v_index = &tia.postr_to_rld_;
  } else if (index_test_name == "postl_to_label_id_test") {
    v_index = &tia.postl_to_label_id_;
  } else if (index_test_name == "postl_to_depth_test") {
    v_index = &tia.postl_to_depth_;
  } else if (index_test_name == "postl_to_lch_test") {
    v_index = &tia.postl_to_lch_;
  } else if (index_test_name == "postl_to_subtree_max_depth_test") {
    v_index = &tia.postl_to_subtree_max_depth_;
  } else if (index_test_name == "postl_to_kr_ancestor_test") {
    v_index = &tia.postl_to_kr_ancestor_;
  } else if (index_test_name == "prel_to_ln_test") {
    v_index = &tia.prel_to_ln_;
  } else if (index_test_name == "prer_to_ln_test") {
    v_index = &tia.prer_to_ln_;
  } else if (index_test_name == "prel_to_type_left_test") {
    v_bool_index = &tia.prel_to_type_left_;
    index_data_type = kVBool;
  } else if (index_test_name == "prel_to_type_right_test") {
    v_bool_index = &tia.prel_to_type_right_;
    index_data_type = kVBool;
  } else if (index_test_name == "prel_to_spf_cost_all_test") {
    v_ll_index = &tia.prel_to_cost_all_;
    index_data_type = kVLLInt;
  } else if (index_test_name == "prel_to_spf_cost_left_test") {
    v_ll_index = &tia.prel_to_cost_left_;
    index_data_type = kVLLInt;
  } else if (index_test_name == "prel_to_spf_cost_right_test") {
    v_ll_index = &tia.prel_to_cost_right_;
    index_data_type = kVLLInt;
  } else if (index_test_name == "prel_to_subtree_del_cost_test") {
    v_double_index = &tia.prel_to_subtree_del_cost_;
    index_data_type = kVDouble;
  } else if (index_test_name == "prel_to_subtree_ins_cost_test") {
    v_double_index = &tia.prel_to_subtree_ins_cost_;
    index_data_type = kVDouble;
  } else if (index_test_name == "list_kr_test") {
    v_index = &tia.list_kr_;
  } else if (index_test_name == "inverted_list_depth_to_postl_test") {
    vv_index = &tia.inverted_list_depth_to_postl_;
    index_data_type = kVVInt;
  } else if (index_test_name == "inverted_list_label_id_to_postl_test") {
    m_ivi_index = &tia.inverted_list_label_id_to_postl_;
    index_data_type = kMIntVInt;
  } else {
    std::cerr << "Error while choosing index to test. Index test name = " + (index_test_name) + "." << std::endl;
    return -1;
  }
  
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModelLD<Label>;
  using LabelDictionary = label::LabelDictionary<Label>;

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
      
      // Initialise label dictionary - separate dictionary for each test tree
      // becuse it is easier to keep track of label ids.
      LabelDictionary ld;
      
      // Initialise cost model.
      CostModel ucm(ld);
      
      // Index the tree with all indexes.
      node::index_tree(tia, tree, ld, ucm);
      
      // Convert the computed results based on index output type.
      std::string computed_results;
      switch(index_data_type) {
        case 0 : computed_results = common::vector_to_string(*v_index);
          break;
        case 1 : computed_results = common::vector_to_string(*vv_index);
          break;
        case 2 : computed_results = common::vector_to_string(*v_bool_index);
          break;
        case 3 : computed_results = common::vector_to_string(*v_double_index);
          break;
        case 4 : computed_results = common::vector_to_string(*v_ll_index);
          break;
        case 5 : computed_results = common::map_to_string(*m_ivi_index);
          break;
      }
      
      if (correct_result != computed_results) {
        std::cerr << "Incorrect " + index_test_name + " result: " <<
            computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "Input: " <<  input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
