#include "lbl_tree.h"

int lbl_tree::node_id_counter = 0;

const int lbl_tree::get_id() const {
	return id;
}

std::vector<node*> lbl_tree::generate_postorder () {
  node_id_counter = 1;
  lbl_tree::postorder((node*)this);
  return tr_post;
}

void lbl_tree::postorder (node* root) {
  if (root) { //not null
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); i++) {
        lbl_tree::postorder(root->get_child(i));
      }
    }

    root->set_id(node_id_counter);
    ++node_id_counter;
    tr_post.push_back(root);
  }
}

void lbl_tree::make_leaves () {
  set_leaves(this, leaves);
}

void lbl_tree::set_leaves (lbl_tree* root, std::vector<node*>& leaves) {
  if (root) { //not null
    if (root->get_children_number() > 0) {
      for (int i = 0; i < root->get_children_number(); i++) {
        root->set_leaves((lbl_tree*)root->get_child(i), leaves);
      }
    } else {
      leaves.push_back(root);
    }
  }
}
