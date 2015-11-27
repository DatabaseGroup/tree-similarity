#ifndef TREE_H
#define TREE_H

#include <vector>

#include "node.h"

template<class _node = node>
class tree {
private:
  _node* root;
  std::vector<_node*> nodes;

public:
  tree() {}
  ~tree() {}

};

#endif // TREE_H
