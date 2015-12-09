#ifndef STRING_EDIT_DISTANCE_H
#define STRING_EDIT_DISTANCE_H

#include <vector>
#include <iostream>

#include "node.h"
#include "array_2d.h"

namespace sed {

// Generic function to compute the distance between two trees under a specified
// cost model. Each tree is represented by its respective root node. A root node
// type is specified as first template parameter, the cost model type as second
// template parameter.
template<class _node = node>
int compute_string_edit_distance (_node* t1, _node* t2) {
  std::vector<int> s1, s2;
  int c0 = 0, temp = 0;

  //check if input is a string
  node* temp_node = t1;
  while (temp_node->get_children_number() == 1) {
    s1.push_back(temp_node->get_label_id());
    temp_node = temp_node->get_child(0);
  }

  if (temp_node->get_children_number() != 0) {
    //false input
    std::cout << "Error: first input is not a string!" << std::endl;
    return -1;
  } else {
    s1.push_back(temp_node->get_label_id());
  }

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
  array_2d<int> result(t1->get_subtree_size(), t2->get_subtree_size());

  for (int i = 0; i < t1->get_subtree_size(); ++i) {
    result[i][0] = i;
  }

  for (int i = 0; i < t2->get_subtree_size(); ++i) {
    result[0][i] = i;
  }

  for (int j = 1; j < t2->get_subtree_size(); ++j) {
    for (int i = 1; i < t1->get_subtree_size(); ++i) {
      c0 = (s1.at(i) == s2.at(j)) ? 0 : 1;

      //calculate minimum
      temp = (result[i - 1][j - 1] + c0 < result[i - 1][j] + 1) ?
        result[i - 1][j - 1] + c0 : result[i - 1][j] + 1;
      result[i][j] = (temp < result[i][j - 1] + 1) ? temp : result[i][j - 1] + 1;
    }
  }

  return result[t1->get_subtree_size() - 1][t2->get_subtree_size() - 1];
}

}

#endif // STRING_EDIT_DISTANCE_H
