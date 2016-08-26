#ifndef NODE_PREFIXES_PAIR_H
#define NODE_PREFIXES_PAIR_H

#include <vector>

#include "../nodes/node.h"

namespace wrappers {

template<class _Node = nodes::Node>
class NodePrefixesPair {
private:
  _Node root_;
  std::vector<std::vector<_Node*>*> prefixes_;

public:
  NodePrefixesPair(_Node& root);
  ~NodePrefixesPair();

  void add_prefix(std::vector<_Node*>* subroot);
  int get_number_of_prefixes() const;

  const _Node get_root() const;
  const std::vector<std::vector<_Node*>*> get_prefixes() const;
  const std::vector<_Node*>* get_prefix(_Node& subroot) const;

  void set_root(_Node& root);
};

template<class _Node>
NodePrefixesPair<_Node>::NodePrefixesPair(_Node& root) : root_(root) { }

template<class _Node>
NodePrefixesPair<_Node>::~NodePrefixesPair() {
  prefixes_.clear();
}

template<class _Node>
void NodePrefixesPair<_Node>::add_prefix(std::vector<_Node*>* subroot) {
  prefixes_.push_back(subroot);
}

template<class _Node>
int NodePrefixesPair<_Node>::get_number_of_prefixes() const {
  return prefixes_.size();
}

template<class _Node>
const _Node NodePrefixesPair<_Node>::get_root() const {
  return root_;
}

template<class _Node>
const std::vector<std::vector<_Node*>*> NodePrefixesPair<_Node>::get_prefixes() const {
  return prefixes_;
}

template<class _Node>
void NodePrefixesPair<_Node>::set_root(_Node& root) {
  root_ = root;
}

template<class _Node>
const std::vector<_Node*>* NodePrefixesPair<_Node>::get_prefix(_Node& subroot) const {
  
}

} // namespace wrappers

#endif // NODE_PREFIXES_PAIR_H