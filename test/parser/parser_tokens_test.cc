#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "bracket_notation_parser.h"

/// Convert vector of int values to is string representation.
///
/// TODO: Move this method to some util.
///
/// \param v Vector of int values.
/// \return String representation of v.
const std::string vector_to_string(const std::vector<std::string>& v) {
  std::string s;
  for (auto e : v) {
    s += e + ",";
  }
  s.pop_back(); // Delete the last coma
  return s;
}

int main() {

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

      parser::BracketNotationParser bnp;

      // Validate test tree.
      if (!bnp.validate_input(input_tree)) {
        std::cerr << "Incorrect format of input tree: '" << input_tree << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Parse test tree.
      std::string computed_results = vector_to_string(bnp.get_tokens(input_tree));

      if (correct_result != computed_results) {
        std::cerr << "Incorrect tokens: " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << input_tree << std::endl;
        return -1;
      }
    }
  }

  return 0;
}
