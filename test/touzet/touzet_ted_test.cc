#include <iostream>
#include <time.h>
#include "timing.h"
#include <string>
#include <vector>
#include <fstream>
#include <cmath>
#include <random>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "touzet.h"
#include "simple_tree_generator.h"
#include "zhang_shasha.h"

int main() {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Parse test cases from file.
  std::ifstream test_cases_file("touzet_ted_test_data.txt");
  if (!test_cases_file.is_open()) {
    std::cerr << "Error while opening file." << std::endl;
    return -1;
  }

  parser::BracketNotationParser bnp;

  // Initialise Touzet algorithm.
  ted::Touzet<Label, CostModel> touzet_ted;

  // Initialise Zhang and Shsha's algorithm.
  ted::ZhangShasha<Label, CostModel> zs_ted;


  // Fixed test cases.

  std::cout << "--- FIXED TEST ---" << std::endl;

  // Read test cases from a file line by line.
  for (std::string line; std::getline( test_cases_file, line);) {
    if (line[0] == '#') {

      std::cout << "--- TEST CASE ---" << std::endl;
      std::cout << line << std::endl;

      // Read the single test case.
      std::getline(test_cases_file, line);
      std::string input_tree_1_string = line;
      std::getline(test_cases_file, line);
      std::string input_tree_2_string = line;
      std::getline(test_cases_file, line);
      int k = std::stoi(line);
      std::getline(test_cases_file, line);
      double correct_result = std::stod(line);

      // parser::BracketNotationParser bnp;

      // Validate test trees.
      if (!bnp.validate_input(input_tree_1_string)) {
        std::cerr << "Incorrect format of source tree: '" << input_tree_1_string << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      if (!bnp.validate_input(input_tree_2_string)) {
        std::cerr << "Incorrect format of destination tree: '" << input_tree_2_string << "'. Is the number of opening and closing brackets equal?" << std::endl;
        return -1;
      }
      // Parse test tree.
      node::Node<Label> t1 = bnp.parse_single(input_tree_1_string);
      node::Node<Label> t2 = bnp.parse_single(input_tree_2_string);


      // Execute the algorithm WITHOUT redundancy.
      double computed_results;
      try {
        computed_results = touzet_ted.touzet_ted_kr_set(t1, t2, k);
      } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
      }

      if (correct_result != computed_results) {
        std::cerr << "Incorrect TED result (WITHOUT redundancy): " << computed_results << " instead of " << correct_result << std::endl;
        std::cerr << "T1: " << input_tree_1_string << std::endl;
        std::cerr << "T2: " << input_tree_2_string << std::endl;
        std::cerr << "k: " << k << std::endl;
        return -1;
      }
    }
  }

  // Randomised test cases.

  std::cout << "--- RANDOMIZED TEST ---" << std::endl;
  
  // Runtime measurement.
  Timing timing;
  
  // Initialise SimpleTreeGenerator and generate a dummy tree.
  tree_generator::SimpleTreeGenerator stg;
  int max_nodes = 100;
  int max_edits = 30; // 30%
  std::mt19937 rd;
  std::uniform_int_distribution<int> edits_dist(0, max_edits);
  std::string t1_gen_string;
  std::string t2_gen_string;
  double touzet_result;
  double touzet_result_d;
  double touzet_result_kr_loop;
  double touzet_result_kr_set;
  double zs_result;
  int k;
  for (int i = 0; i < 200; ++i) {
    t1_gen_string = stg.generate_tree(max_nodes);
    t2_gen_string = stg.modify_tree(t1_gen_string, max_nodes, edits_dist(rd));
    std::cout << t1_gen_string << std::endl;
    std::cout << t2_gen_string << std::endl;
    node::Node<Label> t1_gen = bnp.parse_single(t1_gen_string);
    node::Node<Label> t2_gen = bnp.parse_single(t2_gen_string);
    try {
      Timing::Interval * zs_time = timing.create_enroll("zs_time");
      zs_time->start();
      zs_result = zs_ted.zhang_shasha_ted(t1_gen, t2_gen);
      zs_time->stop();
      k = int(std::ceil(zs_result));
      std::cout << "                      k = " << k << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      std::cout << "              zs_result = " << zs_result << std::endl;
      std::cout << "               zs subpr = " << zs_ted.get_subproblem_count() << std::endl;
      std::cout << "             zs runtime = " << zs_time->getfloat() << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      Timing::Interval * touzet_time = timing.create_enroll("touzet_time");
      touzet_time->start();
      touzet_result = touzet_ted.touzet_ted(t1_gen, t2_gen, k);
      touzet_time->stop();
      std::cout << "          touzet_result = " << touzet_result << std::endl;
      std::cout << "           touzet subpr = " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "         touzet runtime = " << touzet_time->getfloat() << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      Timing::Interval * touzet_d_time = timing.create_enroll("touzet_d_time");
      touzet_d_time->start();
      touzet_result_d = touzet_ted.touzet_ted_depth_pruning(t1_gen, t2_gen, k);
      touzet_d_time->stop();
      std::cout << "        touzet_result_d = " << touzet_result_d << std::endl;
      std::cout << "         touzet d subpr = " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "       touzet d runtime = " << touzet_d_time->getfloat() << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      Timing::Interval * touzet_kr_loop_time = timing.create_enroll("touzet_kr_loop_time");
      touzet_kr_loop_time->start();
      touzet_result_kr_loop = touzet_ted.touzet_ted_kr_loop(t1_gen, t2_gen, k);
      touzet_kr_loop_time->stop();
      std::cout << "  touzet_result_kr_loop = " << touzet_result_kr_loop << std::endl;
      std::cout << "   touzet kr loop subpr = " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << " touzet kr loop runtime = " << touzet_kr_loop_time->getfloat() << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
      Timing::Interval * touzet_kr_set_time = timing.create_enroll("touzet_kr_set_time");
      touzet_kr_set_time->start();
      touzet_result_kr_set = touzet_ted.touzet_ted_kr_set(t1_gen, t2_gen, k);
      touzet_kr_set_time->stop();
      std::cout << "   touzet_result_kr_set = " << touzet_result_kr_set << std::endl;
      std::cout << "    touzet kr set subpr = " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "  touzet kr set runtime = " << touzet_kr_set_time->getfloat() << std::endl;
      std::cout << "---------------------------------------------" << std::endl;
    } catch (const std::exception& e) {
      std::cerr << e.what() << std::endl;
    }

    if (zs_result != touzet_result ||
        zs_result != touzet_result_d ||
        zs_result != touzet_result_kr_loop ||
        zs_result != touzet_result_kr_set
      ) {
      std::cerr << "Incorrect TED result (Touzet | Touzet_D | Touzet_KR_LOOP | Touzet_KR_SET vs ZS): " <<
          touzet_result << " | " <<
          touzet_result_d << " | " <<
          touzet_result_kr_loop << " | " <<
          touzet_result_kr_set <<
          " instead of " << zs_result << std::endl;
      std::cerr << "T1: " << t1_gen_string << std::endl;
      std::cerr << "T2: " << t2_gen_string << std::endl;
      std::cerr << "k: " << k << std::endl;
      return -1;
    }
  }



  return 0;
}
