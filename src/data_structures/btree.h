#ifndef DATA_STRUCTURES_B_TREE_H
#define DATA_STRUCTURES_B_TREE_H

#include "../nodes/node.h"

namespace data_structures {

/**
 *  Tree data structure that keeps data sorted and allows searches, insertions
 *  and deletions in logarithmic amortized time.
 *
 *  Not to be confused with a B+ tree! The only difference here is, that the
 *  B+ tree nodes are only used to navigate to the correct leaf node (do not hold
 *  any data) whereas in the B tree the data is already stored in the node
 *  elements. In the B+ tree, the data is stored in an additional level below
 *  the leaf nodes and the leaf nodes contain pointers to this data sets.
 *
 *  This data structure is used in the LabelIndex class (indices/label_index.h)
 *  to organize the set of all label indices (posting lists).
 *
 *  Properties:
 *    - Nodes have many more than two children.
 *    - A node may contain more than just a single element.
 *    - No duplicates supported/allowed (data of the duplicate key gets replaced).
 *
 *  A B-tree depends on a positive constant integer m. This tells us how many
 *  elements a single node is able to hold. The root node may have as few as one
 *  child, all other nodes are guaranteed to have at least ceil(m / 2) elements.
 *  The maximum number of elements in a node is m. Elements of a node are stored
 *  using a partially-filled array, sorted ascending wrt. the key (smallest is
 *  at index 0). The number of subtrees below a non-leaf node is always one more
 *  than the number of elements in the node. For any non-leaf node, an element
 *  at index i is greater than all the elements in subtree number i of the node.
 *  Analogous, an element at index i is smaller than all the elements in subtree
 *  number (i + 1) of the node. All leaf nodes in the B-tree have the same depth.
 *  Thus, the B-tree is a self-balancing tree data structure avoiding some of
 *  the problems of unbalanced tree data structures.
 */
template<class _Key, class _Data, size_t _M>
class BTree {
private:
  // only for internal usage
  struct BTreeNode {
    typedef std::array<std::pair<_Key, _Data>, _M> EntriesType;
    typedef std::array<BTreeNode*, _M + 1> ChildrenType;
    // hopefully better performance using std::array (evaluated at compile time)
    EntriesType entries_{};
    ChildrenType children_{};
    size_t max_index_{};

    BTreeNode() { }
    ~BTreeNode() { }

    // debug, to be removed once tests succeed
    void print() const {
      std::cout << "{ ";
      for (const std::pair<_Key, _Data>& entry: entries_) {
        std::cout << "(" << entry.first << ", " << entry.second << ") ";
      }
      std::cout << "}" << std::endl;
    }
  };

private:
  BTreeNode* root_;
  size_t size_;

  size_t lower_bound(BTreeNode* node, const _Key& key) const;
  std::pair<BTreeNode*, size_t> find_node(BTreeNode* node, const _Key& key) const;
  BTreeNode* split(const std::pair<BTreeNode*, size_t> to_split);

  // debug, remove once tests succeed and print() is removed
  void print(BTreeNode* root, int level) const;

public:
  BTree();
  ~BTree();

  BTreeNode* insert(const _Key& key, const _Data& data);
  BTreeNode* insert(BTreeNode* root, const _Key& key, const _Data& data);
  _Data get(const _Key& key) const;
  _Data search(const BTreeNode* root, const _Key& key) const;
  void remove();

  bool empty() const;
  size_t size() const;

  bool is_leaf(const BTreeNode* node) const;
  bool is_full(const BTreeNode* node) const;

  // debug, remove once tests succeed (or replace by operator<<)
  void print() const;
};

template<class _Key, class _Data, size_t _M>
size_t BTree<_Key, _Data, _M>::lower_bound(BTreeNode* node, const _Key& key) const
{
  if (node->max_index_ < 0) {
    return (-1); // TODO: exception
  }

  // O(log n) in the size of the entries_ array
  typename BTreeNode::EntriesType::const_iterator lower_bound =
    std::lower_bound(node->entries_.begin(),
      node->entries_.begin() + node->max_index_, key,
      [](const std::pair<_Key, _Data>& lhs, const _Key& rhs) -> bool {
        return (lhs.first < rhs);
      }
    );

  // return the index of the first element >= key
  return (lower_bound - node->entries_.begin());
}

template<class _Key, class _Data, size_t _M>
std::pair<typename BTree<_Key, _Data, _M>::BTreeNode*, size_t> BTree<_Key, _Data, _M>::find_node(
  BTreeNode* node, const _Key& key) const
{
  if (is_leaf(node)) {
    //return std::make_pair(node, lower_bound(node, key));
    return std::pair<BTreeNode*, size_t>(node, lower_bound(node, key));
  }

  size_t lower_bound = this->lower_bound(node, key); // compute position in node

  // check for duplicate key
  if (node->entries_.at(lower_bound).first == key) {
    return std::make_pair(node, lower_bound);
  }

  // may be lower_bound +- 1, to be tested/verified
  return find_node(node->children_.at(lower_bound), key);
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::split(
  const std::pair<BTreeNode*, size_t> to_split)
{
  BTreeNode* new_node = new BTreeNode();
  BTreeNode* node_to_split = to_split.first;
  const size_t& insertion_index = to_split.second;
  int i = 0;
  int half = std::floor(_M / 2);
  std::pair<_Key, _Data>& middle_element = node_to_split->entries_.at(half);
  bool is_part_of_first_half = (to_split.second < half);

  std::cout << "Middle element: (" << middle_element.first << ", " << middle_element.second << ")" << std::endl;

  for (i = half - 1; i >= 0; --i) {
    std::cout << "1 - i = " << i << " -> " << i << std::endl;
    new_node->entries_.at(i) = std::move(node_to_split->entries_.at(i));
  }
  new_node->max_index_ = half - 1;

  for (i = node_to_split->max_index_; i > half; --i) {
    std::cout << "2 - i = " << i << " -> " << i - half - 1 << std::endl;
    node_to_split->entries_.at(i - half - 1) = std::move(node_to_split->entries_.at(i));
  }

  std::cout << "NEW_NODE: " << std::endl;
  new_node->print();
  std::cout << "NODE_TO_SPLIT: " << std::endl;
  node_to_split->print();

  // if the root is split, we need to do some additional things
  /*
  if (node_to_split == root_) {
    BTreeNode* new_root = new BTreeNode();

    new_root->children_.at(0) = new_node;
    new_root->children_.at(1) = node_to_split;


  }*/
  return nullptr;
}

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::BTree() : size_(0) {
  root_ = new BTreeNode();
}

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::~BTree() {
  delete root_;
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::insert(
  const _Key& key, const _Data& data)
{
  return insert(root_, key, data);
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::insert(
  BTreeNode* node, const _Key& key, const _Data& data)
{
  std::pair<_Key, _Data> new_entry = std::make_pair(key, data);
  std::pair<BTreeNode*, size_t> to_insert = find_node(node, key);
  BTreeNode* node_to_insert = to_insert.first;
  size_t& insertion_index = to_insert.second;
  bool is_duplicate = (node_to_insert->entries_.at(insertion_index).first == key);

  if (is_leaf(node_to_insert)) {
    std::cout << "Leaf: Inserting at position " << insertion_index << std::endl;

    if (is_full(node_to_insert)) {
      std::cout << "Leaf: Is full" << std::endl;
      split(to_insert);
    }

    node_to_insert->entries_.at(insertion_index) = std::make_pair(key, data);
  } else {

  }

  if (!is_duplicate) {
    ++size_;
    ++node_to_insert->max_index_; // adjust search range in node
  }

  return nullptr;
}

template<class _Key, class _Data, size_t _M>
_Data BTree<_Key, _Data, _M>::get(const _Key& key) const {
  return search(root_, key);
}

template<class _Key, class _Data, size_t _M>
_Data BTree<_Key, _Data, _M>::search(const BTreeNode* root, const _Key& key) const 
{

}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::remove() {

  --size_;
}

template<class _Key, class _Data, size_t _M>
bool BTree<_Key, _Data, _M>::empty() const {
  return (size_ <= 0);
}

template<class _Key, class _Data, size_t _M>
size_t BTree<_Key, _Data, _M>::size() const {
  return size_;
}

template<class _Key, class _Data, size_t _M>
bool BTree<_Key, _Data, _M>::is_leaf(const BTreeNode* node) const {
  return (node->children_.at(0) == nullptr);
}

template<class _Key, class _Data, size_t _M>
bool BTree<_Key, _Data, _M>::is_full(const BTreeNode* node) const {
  return (node->max_index_ + 1 >= node->entries_.size());
}

// debug, remove once tests succeed (or replace by operator<<)
template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::print() const {
  std::cout << "Tree size: " << size_ << std::endl;
  print(root_, 0);
}

// debug, remove once tests succeed and print() is removed
template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::print(BTreeNode* root, int level) const {
  for (int i = 0; i < level; ++i) {
    std::cout << "  ";
  }
  root->print();
  
  if (!is_leaf(root)) {
    for (BTreeNode* child: root->children_) {
      print(child, level + 1);
    }
  }
}

} // namespace data_structures

#endif // DATA_STRUCTURES_B_TREE_H
