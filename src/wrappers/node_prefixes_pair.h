#ifndef NODE_PREFIXES_PAIR_H
#define NODE_PREFIXES_PAIR_H

#include <vector>
#include <map>

#include "../nodes/node.h"

namespace wrappers {

template<class _Node = nodes::Node>
class NodePrefixesPair {
private:
  _Node node_;
  std::map<int, std::vector<_Node*>*> subtrees_;
  std::map<int, std::vector<_Node*>*> prefixes_;

  void extract_subtrees_prefixes(_Node* root);

public:
  NodePrefixesPair(_Node& node);
  ~NodePrefixesPair();

  void add_prefix(std::vector<_Node*>* subroot);
  void create_subtrees_prefixes();
  int get_number_of_prefixes() const;

  const _Node get_node() const;
  
  const std::map<_Node, std::vector<_Node*>*> get_prefixes() const;
  const std::vector<_Node*>* get_prefix(_Node& subroot) const;
  const std::vector<_Node*>* get_prefix(int index) const;

  const std::map<_Node, std::vector<_Node*>*> get_subtrees() const;
  const std::vector<_Node*>* get_subtree(_Node& subroot) const;

  void set_node(_Node& node);
};

template<class _Node>
NodePrefixesPair<_Node>::NodePrefixesPair(_Node& node) : node_(node) {
  create_subtrees_prefixes();
}

template<class _Node>
NodePrefixesPair<_Node>::~NodePrefixesPair() {
  prefixes_.clear();
}

template<class _Node>
void NodePrefixesPair<_Node>::add_prefix(std::vector<_Node*>* subroot) {
  //prefixes_.push_back(subroot);
}

// TODO: not optimal since it executes another postorder traversal ...
template<class _Node>
void NodePrefixesPair<_Node>::create_subtrees_prefixes() {
  extract_subtrees_prefixes(&node_);
}

// TODO: avoid code duplication! (common.h)
template<class _Node>
void NodePrefixesPair<_Node>::extract_subtrees_prefixes(_Node* node) {
  if (node != nullptr) {
    if (node->get_children_number() > 0) {
      for (int i = 0; i < node->get_children_number(); ++i) {
        extract_subtrees_prefixes(node->get_child(i));
      }
    }

    // TODO: is this optimal? I doubt that!
    subtrees_.insert(std::make_pair(node->get_id(), common::generate_postorder(node, true)));

    if (prefixes_.empty()) {
      prefixes_.insert(std::make_pair(node->get_id(), new std::vector<_Node*>({ node })));
    } else {
      // uses insert return pair<iterator, bool> instantly to update the prefix
      prefixes_.insert(std::make_pair(node->get_id(), new std::vector<_Node*>(
        prefixes_.rbegin()->second->begin(), prefixes_.rbegin()->second->end()
      ))).first->second->push_back(node);
    }
  }
}

template<class _Node>
int NodePrefixesPair<_Node>::get_number_of_prefixes() const {
  return prefixes_.size();
}

template<class _Node>
const _Node NodePrefixesPair<_Node>::get_node() const {
  return node_;
}

template<class _Node>
const std::map<_Node, std::vector<_Node*>*> NodePrefixesPair<_Node>::get_prefixes() const {
  return prefixes_;
}

template<class _Node>
void NodePrefixesPair<_Node>::set_node(_Node& node) {
  node_ = node;
}

template<class _Node>
const std::vector<_Node*>* NodePrefixesPair<_Node>::get_prefix(_Node& subroot) const {
  return prefixes_.at(subroot.get_id());
}

template<class _Node>
const std::vector<_Node*>* NodePrefixesPair<_Node>::get_prefix(int index) const {
  //return prefixes_.at(index);
  return *prefixes_.begin();
}

template<class _Node>
const std::map<_Node, std::vector<_Node*>*> NodePrefixesPair<_Node>::get_subtrees() const {
  return subtrees_;
}

template<class _Node>
const std::vector<_Node*>* NodePrefixesPair<_Node>::get_subtree(_Node& subroot) const {
  return subtrees_.at(subroot.get_id());
}

} // namespace wrappers

#endif // NODE_PREFIXES_PAIR_H