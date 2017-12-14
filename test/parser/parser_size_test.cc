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
  std::ifstream test_cases_file("parser_size_test_data.txt");

  if (!test_cases_file) {
    std::cerr << "ERROR: Problem reading the test file." << std::endl;
  }

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      int correct_result = std::stoi(line);

      parser::BracketNotationParser bnp;

      // Validate test tree.
      if (!bnp.validate_input(input_tree)) {
        std::cerr << "Incorrect format of input tree: '" << input_tree << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Parse test tree.
      node::Node<Label> t = bnp.parse_single(input_tree);

      int computed_results = t.get_tree_size();

      if (correct_result != computed_results) {
        std::cerr << "Incorrect number of nodes: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
