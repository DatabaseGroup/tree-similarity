#ifndef NODE_PREFIXES_PAIR_H
#define NODE_PREFIXES_PAIR_H

#include <vector>
#include <map>

#include "../nodes/node.h"

namespace wrappers {

// TODO: make node_ of type _Node and adapt methods accordingly

template<class _Node = nodes::StringNode>
class NodePrefixesPair {
private:
  nodes::Node<_Node> node_;
  std::map<int, std::vector<nodes::Node<_Node>*>*> subtrees_;
  std::map<int, std::vector<nodes::Node<_Node>*>*> prefixes_;

  void extract_subtrees_prefixes(nodes::Node<_Node>* root, int& id);

public:
  NodePrefixesPair(nodes::Node<_Node>& node, const int& node_id);
  ~NodePrefixesPair();

  void add_prefix(std::vector<nodes::Node<_Node>*>* subroot);
  void create_subtrees_prefixes(const int& node_id);
  int get_number_of_prefixes() const;

  const nodes::Node<_Node> get_node() const;
  
  const std::map<nodes::Node<_Node>, std::vector<nodes::Node<_Node>*>*> get_prefixes() const;
  //const std::vector<nodes::Node<_Node>*>* get_prefix(nodes::Node<_Node>& subroot) const;
  const std::vector<nodes::Node<_Node>*>* get_prefix(int index) const;

  const std::map<nodes::Node<_Node>, std::vector<nodes::Node<_Node>*>*> get_subtrees() const;
  //const std::vector<nodes::Node<_Node>*>* get_subtree(nodes::Node<_Node>& subroot) const;
  const std::vector<nodes::Node<_Node>*>* get_subtree(int index) const;

  void set_node(nodes::Node<_Node>& node);
};

template<class _Node>
NodePrefixesPair<_Node>::NodePrefixesPair(nodes::Node<_Node>& node, const int& node_id)
  : node_(node)
{
  create_subtrees_prefixes(node_id);
}

template<class _Node>
NodePrefixesPair<_Node>::~NodePrefixesPair() {
  prefixes_.clear();
}

template<class _Node>
void NodePrefixesPair<_Node>::add_prefix(std::vector<nodes::Node<_Node>*>* subroot) {
  //prefixes_.push_back(subroot);
}

// TODO: not optimal since it executes another postorder traversal ...
template<class _Node>
void NodePrefixesPair<_Node>::create_subtrees_prefixes(const int& node_id) {
  int id = node_id - node_.get_subtree_size();
  extract_subtrees_prefixes(&node_, id);
}

// TODO: avoid code duplication! (common.h)
template<class _Node>
void NodePrefixesPair<_Node>::extract_subtrees_prefixes(nodes::Node<_Node>* node,
  int& id)
{
  if (node != nullptr) {
    if (node->get_children_number() > 0) {
      for (int i = 0; i < node->get_children_number(); ++i) {
        extract_subtrees_prefixes(node->get_child(i), id);
      }
    }

    ++id;
    // TODO: is this optimal? I doubt that!
    subtrees_.insert(std::make_pair(id, common::generate_postorder(node)));

    if (prefixes_.empty()) {
      prefixes_.insert(std::make_pair(id, new std::vector<nodes::Node<_Node>*>({ node })));
    } else {
      // uses insert return pair<iterator, bool> instantly to update the prefix
      prefixes_.insert(std::make_pair(id, new std::vector<nodes::Node<_Node>*>(
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
const nodes::Node<_Node> NodePrefixesPair<_Node>::get_node() const {
  return node_;
}

template<class _Node>
const std::map<nodes::Node<_Node>, std::vector<nodes::Node<_Node>*>*> NodePrefixesPair<_Node>::get_prefixes() const {
  return prefixes_;
}

template<class _Node>
void NodePrefixesPair<_Node>::set_node(nodes::Node<_Node>& node) {
  node_ = node;
}

template<class _Node>
const std::vector<nodes::Node<_Node>*>* NodePrefixesPair<_Node>::get_prefix(int index) const {
  return prefixes_.at(index);
  //return *prefixes_.begin();
}

template<class _Node>
const std::map<nodes::Node<_Node>, std::vector<nodes::Node<_Node>*>*> NodePrefixesPair<_Node>::get_subtrees() const {
  return subtrees_;
}

template<class _Node>
const std::vector<nodes::Node<_Node>*>* NodePrefixesPair<_Node>::get_subtree(int index) const {
  return subtrees_.at(index);
}

} // namespace wrappers

#endif // NODE_PREFIXES_PAIR_H