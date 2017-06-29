#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"

int main() {

  using Label = label::StringLabel;

  // Parse test cases from file.
  std::ifstream test_cases_file("size_test_data.txt");
  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      int correct_result = std::stoi(line);

      // Parse test tree.
      parser::BracketNotationParser bnp;
      node::Node<Label> t1 = bnp.parse_string(input_tree);

      int computed_results = t1.get_tree_size();

      if (correct_result != computed_results) {
        // TODO: Print test case input.
        std::cerr << "Incorrect number of nodes: " << computed_results << " instead of " << correct_result << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
