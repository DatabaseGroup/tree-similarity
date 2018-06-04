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
#include "apted.h"
#include "greedy_ub.h"

struct TestParams {
  std::string input_file;
  bool set_zs_k;
  bool fixed_k;
  int k;
  bool alg_touzet_ted;
  bool alg_touzet_ted_depth_pruning;
  bool alg_touzet_ted_kr_loop;
  bool alg_touzet_ted_kr_set;
  bool alg_zs_ted;
  bool alg_greedy_ub;
  bool alg_greedy_ub_deprecated;
  bool alg_apted;
  
  TestParams() {
    input_file = "";
    set_zs_k = false;
    fixed_k = false;
    k = 0;
    alg_touzet_ted = false;
    alg_touzet_ted_depth_pruning = false;
    alg_touzet_ted_kr_loop = false;
    alg_touzet_ted_kr_set = false;
    alg_zs_ted = false;
    alg_greedy_ub = false;
    alg_greedy_ub_deprecated = false;
    alg_apted = false;
  };
  
  void set_default() {
    input_file = "bozen_result_pairs_k7.txt";
    set_zs_k = true;
    fixed_k = false;
    alg_touzet_ted = true;
    alg_touzet_ted_depth_pruning = true;
    alg_touzet_ted_kr_loop = true;
    alg_touzet_ted_kr_set = true;
    alg_zs_ted = true;
    alg_greedy_ub = true;
    alg_greedy_ub_deprecated = true;
    alg_apted = true;
  };
};

struct TestInput {
  std::string description;
  std::string t1;
  std::string t2;
  int k;
  double expected_result; // May be empty if not used in a test.
};

void communicate_result_error(double computed, double expected) {
  std::cerr << "ERROR: Incorrect TED result " << computed << " instead of " << expected << std::endl;
};

int compare_results(double computed, double expected, TestParams& tp) {
  // if (tp.lower_than_threshold) {
  //   if (computed > expected) {
  //     communicate_result_error(computed, expected);
  //     return -1;
  //   }
  // } else 
  // if (computed  expected) {
  //   communicate_result_error(computed, expected);
  //   return -1;
  // }
  return 0;
}

int single_test_case(TestParams& tp, TestInput& ti) {
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;
  
  std::cout << "-------------------------------------------------" << std::endl;
  std::cout << "DESCRIPTION" << std::endl;
  std::cout << ti.description << std::endl;
  
  // Validate test trees.
  parser::BracketNotationParser bnp;
  if (!bnp.validate_input(ti.t1)) {
    std::cerr << "Incorrect format of source tree: '" << ti.t1 << "'. Is the number of opening and closing brackets equal?" << std::endl;
    return -1;
  }
  if (!bnp.validate_input(ti.t2)) {
    std::cerr << "Incorrect format of destination tree: '" << ti.t2 << "'. Is the number of opening and closing brackets equal?" << std::endl;
    return -1;
  }
  // Parse test tree.
  node::Node<Label> t1 = bnp.parse_single(ti.t1);
  node::Node<Label> t2 = bnp.parse_single(ti.t2);
  
  std::cout << "INPUT T1\n" << ti.t1 << std::endl;
  std::cout << "INPUT T2\n" << ti.t2 << std::endl;

  // Initialise Touzet algorithm.
  ted::Touzet<Label, CostModel> touzet_ted;
  
  // Initialise Zhang and Shsha's algorithm.
  ted::ZhangShasha<Label, CostModel> zs_ted;
  
  // Initialise GreedyUB algorithm.
  ted_ub::GreedyUB<Label, CostModel> greedy_ub;
  
  // Initialise APTED algorithm.
  ted::APTED<Label, CostModel> apted_ted;
  
  // Runtime measurement.
  Timing timing;
  Timing::Interval runtime("runtime");
    
  double computed_result = -1;
  double expected_result = -1;
  double result_zs = -1;
  
  if (tp.set_zs_k || tp.alg_zs_ted) {
    runtime.reset();
    runtime.start();
    result_zs = zs_ted.zhang_shasha_ted(t1, t2);
    runtime.stop();
  }
  
  // if (tp.verify_expected) {
  //   expected_result = ti.expected_result;
  // }
  
  expected_result = result_zs;
  
  if (tp.set_zs_k) {
    ti.k = std::ceil(result_zs);
  }
  if (tp.fixed_k) {
    ti.k = tp.k;
  }
  
  std::cout << "K = " << ti.k << std::endl;
  std::cout << "EXPECTED = " << expected_result << std::endl;
  
  if (tp.set_zs_k || tp.alg_zs_ted) {
    std::cout << "ZS_TED" << std::endl;
    std::cout << " ted : " << result_zs << std::endl;
    std::cout << "  sp : " << zs_ted.get_subproblem_count() << std::endl;
    std::cout << "time : " << runtime.getfloat() << std::endl;
  }
  
  try {
    if (tp.alg_touzet_ted) {
      runtime.reset();
      runtime.start();
      computed_result = touzet_ted.touzet_ted(t1, t2, ti.k);
      runtime.stop();
      std::cout << "TOUZET_TED" << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "  sp : " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
      if (compare_results(computed_result, expected_result, tp) < 0) {
        return -1;
      }
    }
    if (tp.alg_touzet_ted_depth_pruning) {
      runtime.reset();
      runtime.start();
      computed_result = touzet_ted.touzet_ted_depth_pruning(t1, t2, ti.k);
      runtime.stop();
      std::cout << "TOUZET_TED_DEPTH_PRUNING" << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "  sp : " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
      if (compare_results(computed_result, expected_result, tp) < 0) {
        return -1;
      }
    }
    if (tp.alg_touzet_ted_kr_loop) {
      runtime.reset();
      runtime.start();
      computed_result = touzet_ted.touzet_ted_kr_loop(t1, t2, ti.k);
      runtime.stop();
      std::cout << "TOUZET_TED_KR_LOOP" << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "  sp : " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
      if (compare_results(computed_result, expected_result, tp) < 0) {
        return -1;
      }
    }
    if (tp.alg_touzet_ted_kr_set) {
      runtime.reset();
      runtime.start();
      computed_result = touzet_ted.touzet_ted_kr_set(t1, t2, ti.k);
      runtime.stop();
      std::cout << "TOUZET_TED_KR_SET" << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "  sp : " << touzet_ted.get_subproblem_count() << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
      if (compare_results(computed_result, expected_result, tp) < 0) {
        return -1;
      }
    }
    if (tp.alg_greedy_ub) {
      runtime.reset();
      runtime.start();
      // std::vector<std::pair<int, int>> m = greedy_ub.lb_mapping(t1, t2, 7);
      std::vector<std::pair<int, int>> m = greedy_ub.lb_mapping_fill_gaps(t1, t2, ti.k);
      computed_result = greedy_ub.mapping_cost(m);//touzet_ted.touzet_ted_kr_set(t1, t2, ti.k);
      runtime.stop();
      std::cout << "GREEDY_UB" << std::endl;
      // std::cout << "   m : ";
      // for (auto e : m) {
      //   std::cout << "(" << e.first << "," << e.second << ") ";
      // }
      // std::cout << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
      if (compare_results(computed_result, expected_result, tp) < 0) {
        return -1;
      }
    }
    if (tp.alg_greedy_ub_deprecated) {
      runtime.reset();
      runtime.start();
      // std::vector<std::pair<int, int>> m = greedy_ub.lb_mapping(t1, t2, 7);
      std::vector<std::pair<int, int>> m = greedy_ub.lb_mapping_fill_gaps_deprecated(t1, t2, ti.k);
      computed_result = greedy_ub.mapping_cost(m);//touzet_ted.touzet_ted_kr_set(t1, t2, ti.k);
      runtime.stop();
      std::cout << "GREEDY_UB_DEPRECATED" << std::endl;
      // std::cout << "   m : ";
      // for (auto e : m) {
      //   std::cout << "(" << e.first << "," << e.second << ") ";
      // }
      // std::cout << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
      if (compare_results(computed_result, expected_result, tp) < 0) {
        return -1;
      }
    }
    if (tp.alg_apted) {
      runtime.reset();
      runtime.start();
      computed_result = apted_ted.apted_ted(t1, t2);
      runtime.stop();
      std::cout << "APTED_TED" << std::endl;
      std::cout << " ted : " << computed_result << std::endl;
      std::cout << "  sp : " << apted_ted.get_subproblem_count() << std::endl;
      std::cout << "time : " << runtime.getfloat() << std::endl;
    }
  } catch (const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  
  return 0;
};



int main(int argc, char** argv) {
  // Command line interface.
  //
  // - File with test cases.
  // - Choose algorithm's version (or more) to execute.
  // - Either verify expected value or compare the result to ZS.
  //   - If set k to ZS, k=ZS.
  //   - If verify, k and expected are given.
  // - Include random.
  
  TestParams tp;
  
  if (argc > 1) {
    std::vector<std::string> arguments(argv, argv + argc);
    for (auto a : arguments) {
      std::string string_a(a);
      if (string_a.find("--input=") != std::string::npos) {
        tp.input_file = string_a.substr(string_a.find('=') + 1, string_a.size());
      }
      if (string_a == "--set-zs-k") {
        tp.set_zs_k = true;
        tp.fixed_k = false;
      }
      if (string_a.find("--fixed-k=") != std::string::npos) {
        tp.k = std::stoi(string_a.substr(string_a.find('=') + 1, string_a.size()));
        tp.fixed_k = true;
        tp.set_zs_k = false;
      }
      if (string_a == "-t") {
        tp.alg_touzet_ted = true;
      }
      if (string_a == "-td") {
        tp.alg_touzet_ted_depth_pruning = true;
      }
      if (string_a == "-tl") {
        tp.alg_touzet_ted_kr_loop = true;
      }
      if (string_a == "-ts") {
        tp.alg_touzet_ted_kr_set = true;
      }
      if (string_a == "-g") {
        tp.alg_greedy_ub = true;
      }
      if (string_a == "-gd") {
        tp.alg_greedy_ub_deprecated = true;
      }
      if (string_a == "-zs") {
        tp.alg_zs_ted = true;
      }
      if (string_a == "-apted") {
        tp.alg_apted = true;
      }
    }
    if (tp.input_file == "") {
      std::cerr << "ERROR Nothing to do." << std::endl;
      return -1;
    }
  } else {
    tp.set_default();
  }
  
  std::cout << "************************************* TEST PARAMS" << std::endl;
  std::cout << "input_file = " << tp.input_file << std::endl;
  std::cout << "set_zs_k = " << tp.set_zs_k << std::endl;
  std::cout << "fixed_k = " << tp.fixed_k << std::endl;
  std::cout << "k = " << tp.k << std::endl;
  std::cout << "alg_touzet_ted = " << tp.alg_touzet_ted << std::endl;
  std::cout << "alg_touzet_ted_depth_pruning = " << tp.alg_touzet_ted_depth_pruning << std::endl;
  std::cout << "alg_touzet_ted_kr_loop = " << tp.alg_touzet_ted_kr_loop << std::endl;
  std::cout << "alg_touzet_ted_kr_set = " << tp.alg_touzet_ted_kr_set << std::endl;
  std::cout << "alg_zs_ted = " << tp.alg_zs_ted << std::endl;
  std::cout << "alg_greedy_ub = " << tp.alg_greedy_ub << std::endl;
  std::cout << "alg_greedy_ub_deprecated = " << tp.alg_greedy_ub_deprecated << std::endl;
  
  std::ifstream test_cases_file;
  if (tp.input_file != "") {
    // Parse test cases from file.
    test_cases_file = std::ifstream(tp.input_file);
    if (!test_cases_file.is_open()) {
      std::cerr << "ERROR Problem while opening file." << std::endl;
      return -1;
    }
  }

  // Fixed test cases.
  std::cout << "************************************** FIXED TEST" << std::endl;

  // Read test cases from a file line by line.
  for (std::string line; std::getline(test_cases_file, line);) {
    TestInput ti;
    // Enable multiline comments.
    while (line[0] == '#') {
      ti.description += line;
      std::getline(test_cases_file, line);
    }
    // Read the single test case.
    ti.t1 = line;
    std::getline(test_cases_file, line);
    ti.t2 = line;
    int stc_res = single_test_case(tp, ti);
    if (stc_res < 0) {
      return -1;
    }
  }

  return 0;
}
