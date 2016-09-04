#ifndef NODE_PREFIXES_PAIR_H
#define NODE_PREFIXES_PAIR_H

#include <vector>
#include <map>

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

namespace wrappers {

// TODO: make node_ of type _Node and adapt methods accordingly

template<class _NodeData = nodes::StringNodeData>
class NodePrefixesPair {
private:
  nodes::Node<_NodeData> node_;
  std::map<int, std::vector<nodes::Node<_NodeData>*>*> subtrees_;
  std::map<int, std::vector<nodes::Node<_NodeData>*>*> prefixes_;

  void extract_subtrees_prefixes(nodes::Node<_NodeData>* root, int& id);

public:
  NodePrefixesPair(nodes::Node<_NodeData>& node, const int& node_id);
  ~NodePrefixesPair();

  void add_prefix(std::vector<nodes::Node<_NodeData>*>* subroot);
  void create_subtrees_prefixes(const int& node_id);
  int get_number_of_prefixes() const;

  const nodes::Node<_NodeData> get_node() const;
  
  const std::map<nodes::Node<_NodeData>, std::vector<nodes::Node<_NodeData>*>*> get_prefixes() const;
  //const std::vector<nodes::Node<_NodeData>*>* get_prefix(nodes::Node<_NodeData>& subroot) const;
  const std::vector<nodes::Node<_NodeData>*>* get_prefix(int index) const;

  const std::map<nodes::Node<_NodeData>, std::vector<nodes::Node<_NodeData>*>*> get_subtrees() const;
  //const std::vector<nodes::Node<_NodeData>*>* get_subtree(nodes::Node<_NodeData>& subroot) const;
  const std::vector<nodes::Node<_NodeData>*>* get_subtree(int index) const;

  void set_node(nodes::Node<_NodeData>& node);
};

template<class _NodeData>
NodePrefixesPair<_NodeData>::NodePrefixesPair(nodes::Node<_NodeData>& node, const int& node_id)
  : node_(node)
{
  create_subtrees_prefixes(node_id);
}

template<class _NodeData>
NodePrefixesPair<_NodeData>::~NodePrefixesPair() {
  prefixes_.clear();
}

template<class _NodeData>
void NodePrefixesPair<_NodeData>::add_prefix(std::vector<nodes::Node<_NodeData>*>* subroot) {
  //prefixes_.push_back(subroot);
}

// TODO: not optimal since it executes another postorder traversal ...
template<class _NodeData>
void NodePrefixesPair<_NodeData>::create_subtrees_prefixes(const int& node_id) {
  int id = node_id - node_.get_subtree_size();
  extract_subtrees_prefixes(&node_, id);
}

// TODO: avoid code duplication! (common.h)
template<class _NodeData>
void NodePrefixesPair<_NodeData>::extract_subtrees_prefixes(nodes::Node<_NodeData>* node,
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
      prefixes_.insert(std::make_pair(id, new std::vector<nodes::Node<_NodeData>*>({ node })));
    } else {
      // uses insert return pair<iterator, bool> instantly to update the prefix
      prefixes_.insert(std::make_pair(id, new std::vector<nodes::Node<_NodeData>*>(
        prefixes_.rbegin()->second->begin(), prefixes_.rbegin()->second->end()
      ))).first->second->push_back(node);
    }
  }
}

template<class _NodeData>
int NodePrefixesPair<_NodeData>::get_number_of_prefixes() const {
  return prefixes_.size();
}

template<class _NodeData>
const nodes::Node<_NodeData> NodePrefixesPair<_NodeData>::get_node() const {
  return node_;
}

template<class _NodeData>
const std::map<nodes::Node<_NodeData>, std::vector<nodes::Node<_NodeData>*>*> NodePrefixesPair<_NodeData>::get_prefixes() const {
  return prefixes_;
}

template<class _NodeData>
void NodePrefixesPair<_NodeData>::set_node(nodes::Node<_NodeData>& node) {
  node_ = node;
}

template<class _NodeData>
const std::vector<nodes::Node<_NodeData>*>* NodePrefixesPair<_NodeData>::get_prefix(int index) const {
  return prefixes_.at(index);
  //return *prefixes_.begin();
}

template<class _NodeData>
const std::map<nodes::Node<_NodeData>, std::vector<nodes::Node<_NodeData>*>*> NodePrefixesPair<_NodeData>::get_subtrees() const {
  return subtrees_;
}

template<class _NodeData>
const std::vector<nodes::Node<_NodeData>*>* NodePrefixesPair<_NodeData>::get_subtree(int index) const {
  return subtrees_.at(index);
}

} // namespace wrappers

#endif // NODE_PREFIXES_PAIR_H