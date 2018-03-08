// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file command_line.cc
///
/// \details
/// TED command-line interface.

#include "main.h"

int main(int argc, char** argv) {

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Runtime measurement variables (rusage).
  rusage before_rusage;
  rusage after_rusage;
  timeval runtime_utime;
  timeval runtime_stime;
  unsigned int runtime;

  // Verify parameters.
  if (argc != 3 && argc != 4 && argc != 5) {
    std::cerr << "Incorrect number of parameters." << std::endl;
    return -1;
  }

  // NOTE: Trees passed as command-line arguments must be sorrounded with ''.

  // std::cout << "Source tree: " << argv[1] << std::endl;
  // std::cout << "Destination tree: " << argv[2] << std::endl;

  parser::BracketNotationParser bnp;
  // Verify the input format before parsing.
  if (!bnp.validate_input(argv[1])) {
    std::cerr << "Incorrect format of source tree. Is the number of opening and closing brackets equal?" << std::endl;
    return -1;
  }
  if (!bnp.validate_input(argv[2])) {
    std::cerr << "Incorrect format of destination tree. Is the number of opening and closing brackets equal?" << std::endl;
    return -1;
  }

  getrusage(RUSAGE_SELF, &before_rusage);
  const node::Node<Label> source_tree = bnp.parse_single(argv[1]);
  const node::Node<Label> destination_tree = bnp.parse_single(argv[2]);
  getrusage(RUSAGE_SELF, &after_rusage);
  timersub(&after_rusage.ru_utime, &before_rusage.ru_utime, &runtime_utime);
  timersub(&after_rusage.ru_stime, &before_rusage.ru_stime, &runtime_stime);
  runtime = runtime_utime.tv_usec + runtime_utime.tv_sec * 1000000 +
      runtime_stime.tv_usec + runtime_stime.tv_sec * 1000000;
  std::cout << runtime << " ";

  if (argc == 3) {
    ted::ZhangShasha<Label, CostModel> zs_ted;
    getrusage(RUSAGE_SELF, &before_rusage);
    std::cout << zs_ted.zhang_shasha_ted(source_tree, destination_tree);
    getrusage(RUSAGE_SELF, &after_rusage);
    std::cout << " " << zs_ted.get_subproblem_count();
    timersub(&after_rusage.ru_utime, &before_rusage.ru_utime, &runtime_utime);
    timersub(&after_rusage.ru_stime, &before_rusage.ru_stime, &runtime_stime);
    runtime = runtime_utime.tv_usec + runtime_utime.tv_sec * 1000000 +
        runtime_stime.tv_usec + runtime_stime.tv_sec * 1000000;
    std::cout << " " << runtime << std::endl;
  }
  if (argc > 3) {
    int k = std::stoi(argv[3]);
    ted::Touzet<Label, CostModel> touzet_ted;
    getrusage(RUSAGE_SELF, &before_rusage);
    if (argc == 5) {
      std::cout << touzet_ted.touzet_ted_depth_pruning(source_tree, destination_tree, k);
    } else {
      std::cout << touzet_ted.touzet_ted(source_tree, destination_tree, k);
    }
    getrusage(RUSAGE_SELF, &after_rusage);
    std::cout << " " << touzet_ted.get_subproblem_count();
    timersub(&after_rusage.ru_utime, &before_rusage.ru_utime, &runtime_utime);
    timersub(&after_rusage.ru_stime, &before_rusage.ru_stime, &runtime_stime);
    runtime = runtime_utime.tv_usec + runtime_utime.tv_sec * 1000000 +
        runtime_stime.tv_usec + runtime_stime.tv_sec * 1000000;
    std::cout << " " << runtime << std::endl;
  }
  return 0;
}
