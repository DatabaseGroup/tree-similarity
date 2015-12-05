#include "string_edit_distance.h"

int string_edit_distance::calculate_distance(lbl_tree* t1, lbl_tree* t2,
  bool debug)
{
  int c = 0, temp = 0;

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
  int result[t1->get_subtree_size()][t2->get_subtree_size()];
  for (int i = 0; i < t1->get_subtree_size(); i++) {
    result[i][0] = i;
  }

  for (int i = 0; i < t2->get_subtree_size(); i++) {
    result[0][i] = i;
  }

/*
  std::cout << "String 1" << std::endl;
  for (unsigned int i = 0; i < s1.size(); i++) {
    std::cout << s1.at(i);
  }
  std::cout << std::endl;

  std::cout << "String 2" << std::endl;
  for (unsigned int i = 0; i < s2.size(); i++) {
    std::cout << s2.at(i);
  }
  std::cout << std::endl;
*/

  for (int j = 1; j < t2->get_subtree_size(); j++) {
    for (int i = 1; i < t1->get_subtree_size(); i++) {
      c = (s1.at(i) == s2.at(j)) ? 0 : 1;

      //calculate minimum
      temp = (result[i - 1][j - 1] + c < result[i - 1][j] + 1) ?
        result[i - 1][j - 1] + c : result[i - 1][j] + 1;
      result[i][j] = (temp < result[i][j - 1] + 1) ? temp : result[i][j - 1] + 1;
    }
  }

  return result[t1->get_subtree_size() - 1][t2->get_subtree_size() - 1];
}
