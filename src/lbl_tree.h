#ifndef LBL_TREE_H
#define LBL_TREE_H

#include "node.h"

//TODO Brackets-Parsen

class lbl_tree : public node {
protected:
  static int node_id_counter;

public:
  int t_id;
  std::vector<node*> tr_post;
  std::vector<node*> leaves;

  lbl_tree () : node () { t_id = 0; }
  lbl_tree (int t_id) : node() { this->t_id = t_id; }
  lbl_tree (int t_id, int label_id) : node(label_id) {
    this->t_id = t_id;
  };
	
  virtual ~lbl_tree() { };
  const int get_id() const;

  std::vector<node*> generate_postorder();
  void postorder(node*);

  void set_leaves(lbl_tree*, std::vector<node*>&);
  void make_leaves();
};

#endif // LBL_TREE_H
