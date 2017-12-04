#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "apted.h"
#include "to_string_converters.h"

int main() {

  using Label = label::StringLabel;

  // Parse test cases from file.
  std::ifstream test_cases_file("apted_preL_to_size_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  parser::BracketNotationParser bnp;

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      std::string correct_result = line;

      node::Node<Label> t1 = bnp.parse_single(input_tree);

      ted::APTEDNodeIndexer<Label> ni(t1);

      // TODO: Differs in test cases filename and the ni's member name.

      std::string computed_results = common::vector_to_string(ni.preL_to_size_);

      if (correct_result != computed_results) {
        std::cerr << "Incorrect subtree sizes result: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
