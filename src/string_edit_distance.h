#ifndef STRING_EDIT_DISTANCE_H
#define STRING_EDIT_DISTANCE_H

#include <vector>
#include <iostream>

#include "node.h"
#include "array_2d.h"

namespace sed {

// Generic function to compute the distance between two line-trees (Strings) under a specified
// cost model. Each tree is represented by its respective root node. A root node
// type is specified as first template parameter, the cost model type as second
// template parameter.
template<class _node = Node, class _costs = Costs<_node>>
int compute_string_edit_distance (_node* t1, _node* t2, _costs c = _costs()) {
  std::vector<int> s1, s2;
  int c0 = 0, temp = 0;

  //check if input is a string
  Node* temp_node = t1;
  while (temp_node->get_children_number() == 1) {
    s1.push_back(temp_node->get_label_id());
    temp_node = temp_node->get_child(0);
  }
  //if there is a node having more than one child then the input is not a string (error)
  if (temp_node->get_children_number() != 0) {
    //false input
    std::cout << "Error: first input is not a string!" << std::endl;
    return -1;
  } else {
    s1.push_back(temp_node->get_label_id());
  }
//check String 2
  temp_node = t2;
  while (temp_node->get_children_number() == 1) {
    s2.push_back(temp_node->get_label_id());
    temp_node = temp_node->get_child(0);
  }

  if (temp_node->get_children_number() != 0) {
    //false input
    std::cout << "Error: second input is not a string!" << std::endl;
    return -1;
  } else {
    s2.push_back(temp_node->get_label_id());
  }

  //calculate string edit distance
  Array2D<double> result(t1->get_subtree_size(), t2->get_subtree_size());

//Costs for deleting of all characters of the left-hand string (assuming that the right-hand-string is a empty string)
  for (int i = 0; i < t1->get_subtree_size(); ++i) {
    result[i][0] = i * c.del();
  }

//Costs for inserting of all characters of the right-hand string (assuming that the left-hand-string is a empty string)
  for (int i = 0; i < t2->get_subtree_size(); ++i) {
    result[0][i] = i * c.ins();
  }

//calculate the costs for all pairs of substrings of the two input strings
  for (int j = 1; j < t2->get_subtree_size(); ++j) {
    for (int i = 1; i < t1->get_subtree_size(); ++i) {
      c0 = (s1.at(i) == s2.at(j)) ? 0 : c.ren();

      //calculate the minimum cost of:
      //   * inserting a character to the right-hand string
      //   * deleting a character of the left-hand string
      //   * renaming a character of the left-hand string to the value of the character of the right-hand string
      temp = (result[i - 1][j - 1] + c0 < result[i - 1][j] + c.del()) ? result[i - 1][j - 1] + c0 : result[i - 1][j] + c.del();
      result[i][j] = (temp < result[i][j - 1] + c.ins()) ? temp : result[i][j - 1] + c.ins();
    }
  }

  return result[t1->get_subtree_size() - 1][t2->get_subtree_size() - 1];
}

}

#endif // STRING_EDIT_DISTANCE_H
