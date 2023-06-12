#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "bracket_notation_parser.h"
#include "to_string_converters.h"

int main() {

  using Label = label::StringLabel;

  // Declarations.
  parser::BracketNotationParser<Label> bnp;
  std::string computed_results;
  node::Node<Label> t;
  
  std::cout << "Tokens test." << std::endl;

  // Parse test cases from file.
  std::ifstream test_cases_file("parser_tokens_test_data.txt");

  if (!test_cases_file) {
    std::cerr << "ERROR: Problem reading the test file." << std::endl;
  }

  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      std::string correct_result = line;

      // Parse test tree.
      try {
        t = bnp.parse_single(input_tree);
      }
      catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return -1;
      }

      // Compute label sets as comma separated strings.
      computed_results = vector_to_string(bnp.get_tokens(input_tree));

      if (correct_result != computed_results) {
        std::cerr << "Incorrect tokens: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
