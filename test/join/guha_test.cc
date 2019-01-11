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

/// Convert vector of ResultElement to its string representation.
/// Sorts the vector in case the result elements don't come in the input order.
///
/// TODO: Move this method to some util.
///
/// \param v Vector of ResultElement.
/// \return String representation of v.
std::string vector_of_re_to_string(std::vector<join::JoinResultElement>& v) {
  // Sort the vector using lambda function implemented here.
  std::sort(v.begin(), v.end(), [](const join::JoinResultElement& a,
                                   const join::JoinResultElement& b){
    if (a.tree_id_1 < b.tree_id_1) {
      return true;
    } else if (a.tree_id_1 == b.tree_id_1) {
      if (a.tree_id_2 < b.tree_id_2) {
        return true;
      } else if (a.tree_id_2 == b.tree_id_2) {
        if (a.ted_value < b.ted_value) {
          return true;
        }
      }
    }
    return false;
  });
  // Convert vector to string.
  std::string s;
  for (auto e : v) {
    s += "{" + std::to_string(e.tree_id_1) + "," + std::to_string(e.tree_id_2)
        + "," + std::to_string(std::lround(e.ted_value)) + "},";
  }
  s.pop_back(); // Delete the last comma.
  return s;
}

int main() {
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;
  using VerificationTouzet = ted::Touzet<Label, CostModel>;

  // Correct result - currently hard-coded here.
  std::string correct_result = "{0,1,1},{0,2,1},{1,2,1},{2,4,1}";

  // File path to input tree collection.
  // std::string file_path = "guha_test_data.txt";
  std::string file_path = "/home/mpawlik/Remote/ted-datasets/bolzano/bolzano_sorted.bracket";

  // Set distance threshold - maximum number of allowed edit operations.
  // double distance_threshold = 1.00;
  
  for (int k = 2; k <= 32; k = k*2) {
    for (int i = 1; i <= 16; i = i*2) {
      std::vector<std::pair<unsigned int, unsigned int>> candidates;
      std::vector<join::JoinResultElement> join_result;

      // Create the container to store all trees.
      std::vector<node::Node<Label>> trees_collection;

      // Parse the dataset.
      parser::BracketNotationParser bnp;
      bnp.parse_collection(trees_collection, file_path);

      // Guha with Touzet verification
      join::Guha<Label, CostModel, VerificationTouzet> guha;
      guha.execute_rsb_join(trees_collection, candidates, join_result, (double)i, k);
      
      std::cout << "tau=" << i << "\tk=" << k << "\tcand=" <<
          candidates.size() << "\t|result|=" << join_result.size() <<
          "\t#l_t_candidates=" << guha.get_l_t_candidates() <<
          "\t#u_t_result_pairs=" << guha.get_u_t_result_pairs() <<
          "\t#sed_candidates=" << guha.get_sed_candidates() <<
          "\t#cted_result_pairs=" << guha.get_cted_result_pairs() << std::endl;
    }
  }

  
  // std::vector<std::pair<unsigned int, unsigned int>> candidates;
  // std::vector<join::JoinResultElement> join_result;
  // 
  // // Create the container to store all trees.
  // std::vector<node::Node<Label>> trees_collection;
  // 
  // // Parse the dataset.
  // parser::BracketNotationParser bnp;
  // bnp.parse_collection(trees_collection, file_path);
  // 
  // // Guha with Touzet verification
  // join::Guha<Label, CostModel, VerificationTouzet> guha;
  // guha.execute_join(trees_collection, candidates, join_result, distance_threshold, 10);
  
  // std::string computed_result = vector_of_re_to_string(join_result);
  // if (correct_result != computed_result) {
  //   std::cerr << "Incorrect result set: " << computed_result << " instead of "
  //       << correct_result << std::endl;
  //   return -1;
  // }

  return 0;
}

