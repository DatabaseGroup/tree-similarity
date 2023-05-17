#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include "bracket_notation_parser.h"

int main() {

  using Label = label::StringLabel;
  
  parser::BracketNotationParser<Label> bnp;
  node::Node<Label> tree;
  int test_cases = 0;
  int identified_cases = 0;
  
  std::cout << "Validation test." << std::endl;

  // Parse test cases from file.
  std::ifstream test_cases_file("parser_validation_test_data.txt");

  if (!test_cases_file) {
    std::cerr << "ERROR: Problem reading the test file." << std::endl;
  }

  for (std::string line; std::getline(test_cases_file, line);) {
    if (line[0] == '#') { // Identify first line of test case by '#'.
      ++test_cases;
      std::getline(test_cases_file, line);
      std::string input_tree = line;
      std::getline(test_cases_file, line);
      std::string correct_result = line;

      // Parse test trees and check for exception identification.
      try {
        tree = bnp.parse_single(input_tree);
      }
      catch(const std::exception& e) {
        std::cerr << e.what() << std::endl;
        if (correct_result != e.what()) {
          std::cerr << "Validation test failed: " << e.what() <<
            " instead of " << correct_result << std::endl;
          return -1;
        }
        ++identified_cases;
      }
    }
  }
  
  if (test_cases != identified_cases) {
    std::cerr << "Validation test failed: only identified " << 
      identified_cases << " out of " << test_cases << std::endl;
    return -1;
  }

  return 0;
}
