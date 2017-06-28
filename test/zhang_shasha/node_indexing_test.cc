#include <iostream>
#include <string>
#include "unit_cost_model.h"
#include "string_label.h"
#include "node.h"
#include "bracket_notation_parser.h"
#include "zhang_shasha.h"

// TODO: Compiling it from command line doesn't work becuase paths of includes
//       in files included here are incorrect.

// template<class _Label> using CostModel = cost_model::UnitCostModel<_Label>;

int main() {
  using Label = label::StringLabel;
  using CostModel = cost_model::UnitCostModel<Label>;


  CostModel cost_model;

  std::cout << "TEST" << std::endl;

  // TODO: Add test cases for node indexing.
  //       - Standard tree.
  //       - key-root nodes
  //       - leftmost leaf descendands
  //       - postorder -> node (node's label)

  std::string s = "{\"a\"{\"b\"{\"c\"}{\"d\"{\"e\"}}}{\"f\"{\"g\"}{\"h\"{\"i\"{\"j\"}{\"k\"}}}}{\"l\"{\"m\"}}}";
  parser::BracketNotationParser bnp;
  node::Node<Label> t1 = bnp.parse_string(s);

  zhang_shasha::Algorithm<Label, CostModel> zs_ted(t1, t1, cost_model);
  double result = zs_ted.zhang_shasha_ted();

  // From cmake documentation: The given test command is expected to exit with
  // code 0 to pass and non-zero to fail.

  if (result != 0.0) return -1;
  return 0;
}
