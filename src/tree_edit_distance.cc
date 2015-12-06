#include "tree_edit_distance.h"

tree_edit_distance::tree_edit_distance (tree_edit_distance_strategy* teds) {
  this->teds = teds;
}

int tree_edit_distance::execute (lbl_tree* t1, lbl_tree* t2, bool debug) {
  return teds->calculate_distance(t1, t2, debug);
}

void tree_edit_distance::set_strategy (tree_edit_distance_strategy* teds) {
  this->teds = teds;
}

tree_edit_distance_strategy* tree_edit_distance::get_strategy () {
  return teds;
}
