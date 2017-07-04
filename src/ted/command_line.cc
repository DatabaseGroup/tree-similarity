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
/// Entry point of the tree similarity command-line interface.

#include "command_line.h"

int main(int argc, char** argv) {

  // const std::string s("{\"a\"{\"\\{[b],\\{key:\\\"value\\\"\\}\\}\"{\"\"}}}");

  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;

  // Verify parameters.
  if (argc != 3) {
    std::cerr << "Incorrect number of parameters." << std::endl;
    return -1;
  }

  // TODO: Trees passed as command-line arguments must escape label-quotes.

  std::cout << "Source tree: " << argv[1] << std::endl;
  std::cout << "Destination tree: " << argv[2] << std::endl;

  // TODO: Implement verification of the input format!

  parser::BracketNotationParser bnp;
  const node::Node<Label> source_tree = bnp.parse_string(argv[1]);
  const node::Node<Label> destination_tree = bnp.parse_string(argv[2]);

  zhang_shasha::Algorithm<Label, CostModel> zs_ted;
  std::cout << "TED = " << zs_ted.zhang_shasha_ted(source_tree, destination_tree) << std::endl;

  return 0;
}
