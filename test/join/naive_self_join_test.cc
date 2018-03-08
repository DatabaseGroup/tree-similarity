#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <algorithm>
#include <cmath>
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "naive_self_join.h"
#include "unit_cost_model.h"

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

  // Set similarity threshold - maximum number of allowed edit operations.
  double similarity_threshold = 1.00;

  // Correct result - currently hard-coded here.
  std::string correct_result = "{1,2,1},{1,3,1},{2,3,1},{2,5,1}";

  std::string file_path = "naive_self_join_test_data.txt";

  // Create the container to store all trees.
  std::vector<node::Node<Label>> trees_collection;

  // Parse the dataset.
  parser::BracketNotationParser bnp;
  bnp.parse_collection(trees_collection, file_path);

  join::NaiveSelfJoin<Label, CostModel> nsj;
  auto result_set = nsj.execute_join(trees_collection, similarity_threshold);
  std::string computed_result = vector_of_re_to_string(result_set);
  if (correct_result != computed_result) {
    std::cerr << "Incorrect result set: " << computed_result << " instead of "
        << correct_result << std::endl;
    return -1;
  }

  return 0;
}
