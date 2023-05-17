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

#include <iostream>
#include "node.h"
#include "string_label.h"
#include "unit_cost_model.h"
#include "bracket_notation_parser.h"
#include "apted_tree_index.h"

/// Simple command-line tool for executing Tree Edit Distance.
int main(int argc, char** argv) {

    using Label = label::StringLabel;
    using CostModelLD = cost_model::UnitCostModelLD<Label>;
    using LabelDictionary = label::LabelDictionary<Label>;

  // Verify parameters.
  if (argc != 4) {
    std::cerr << "Incorrect number of parameters. Sample usage: ./ted string {x{a}} {x{b}}" << std::endl;
    return -1;
  }

  std::string source_tree_string;
  std::string dest_tree_string;

    parser::BracketNotationParser<Label> bnp;
    // Verify the input format before parsing.

  if (std::strcmp(argv[1], "string") == 0) {
    source_tree_string = argv[2];
    dest_tree_string = argv[3];
  } else if (std::strcmp(argv[1], "file") == 0) {
    std::ifstream tree_file(argv[2]);
    std::getline(tree_file, source_tree_string);
    tree_file.close();

        tree_file = std::ifstream(argv[3]);
        std::getline(tree_file, dest_tree_string);
        tree_file.close();
    }
    else {
        std::cerr << "Incorrect input format. Use either string or file." << std::endl;
        return -1;
    }

    const node::Node<Label> source_tree = bnp.parse_single(source_tree_string);
    const node::Node<Label> destination_tree = bnp.parse_single(dest_tree_string);

  std::cout << "Size of source tree:" << source_tree.get_tree_size() << std::endl;
  std::cout << "Size of destination tree:" << destination_tree.get_tree_size() << std::endl;

  LabelDictionary ld;
  CostModelLD ucm(ld);
  ted::APTEDTreeIndex<CostModelLD, node::TreeIndexAPTED> apted_algorithm(ucm);
  node::TreeIndexAPTED ti1;
  node::TreeIndexAPTED ti2;
  node::index_tree(ti1, source_tree, ld, ucm);
  node::index_tree(ti2, destination_tree, ld, ucm);
  std::cout << "Distance TED:" << apted_algorithm.ted(ti1, ti2) << std::endl;

    return 0;
}