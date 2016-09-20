#ifndef DATA_STRUCTURES_B_TREE_H
#define DATA_STRUCTURES_B_TREE_H

#include <iterator>

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
    // not entirely memory-efficient but reduces code complexity a lot
    // TODO: maybe optimize memory usage
    typedef std::array<std::pair<_Key, _Data>, _M + 1> EntriesType;
    typedef std::array<BTreeNode*, _M + 2> ChildrenType;
    // hopefully better performance using std::array (evaluated at compile time)
    EntriesType entries_{};
    ChildrenType children_{};
    size_t next_index_{};

    BTreeNode() { }
    ~BTreeNode() { }

    bool leaf() const {
      return (children_.at(0) == nullptr);
    }

    bool full() const {
      return (next_index_ >= _M + 1);
    }

    // debug, to be removed once tests succeed
    void print() const { 
      std::cout << "{ ";
      for (int i = 0; i < next_index_; ++i) {
        std::cout << "(" << entries_.at(i).first << ", " << entries_.at(i).second << ") ";
      }
      std::cout << "} (next_index_ = " << next_index_ << ")";
    }
  };

private:
  BTreeNode* root_{};
  size_t size_{};
  const size_t half_{};
  const bool even_M_{};

  size_t lower_bound_index(const _Key& key,
    typename BTreeNode::EntriesType::const_iterator begin,
    typename BTreeNode::EntriesType::const_iterator end) const;
  typename std::pair<BTreeNode*, std::pair<_Key, _Data>> split(
    BTreeNode* node, const std::pair<_Key, _Data>& entry);
  std::pair<size_t, bool> replace_if(BTreeNode* node,
    const std::pair<_Key, _Data>& entry);
  void update_mid(std::pair<_Key, _Data>& mid, std::pair<_Key, _Data>& node_entry,
    const std::pair<_Key, _Data>& entry);

  // debug, remove once tests succeed and print() is removed
  void print(BTreeNode* root, int level) const;

public:
  BTree();
  ~BTree();

  void insert(const _Key& key, const _Data& data);
  typename std::pair<BTreeNode*, std::pair<_Key, _Data>> insert(BTreeNode* root,
    std::pair<_Key, _Data>& entry
  );
  std::pair<_Key, _Data> search(const _Key& key) const;
  std::pair<_Key, _Data> search(const BTreeNode* root, const _Key& key) const;
  void remove();

  bool empty() const;
  size_t size() const;

  // debug, remove once tests succeed (or replace by operator<<)
  void print() const;
};

template<class _Key, class _Data, size_t _M>
size_t BTree<_Key, _Data, _M>::lower_bound_index(const _Key& key,
  typename BTree<_Key, _Data, _M>::BTreeNode::EntriesType::const_iterator begin,
  typename BTree<_Key, _Data, _M>::BTreeNode::EntriesType::const_iterator end) const
{
  // O(log n) in the size of the entries_ array
  typename BTreeNode::EntriesType::const_iterator lower_bound =
    std::lower_bound(begin, end, key,
      [](const std::pair<_Key, _Data>& lhs, const _Key& rhs) -> bool {
        return (lhs.first < rhs);
      }
    );

  // return the index of the first element >= key
  return (lower_bound - begin);
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename std::pair<typename BTree<_Key, _Data, _M>::BTreeNode*, std::pair<_Key, _Data>> BTree<_Key, _Data, _M>::split(
  BTreeNode* node, const std::pair<_Key, _Data>& entry)
{
  std::pair<_Key, _Data>& mid = node->entries_.at(half_);
  BTreeNode* new_node = new BTreeNode();
  int i = 0, j = 0;
  int j_begin = half_ - (even_M_ ? 1 : 0);

  new_node->children_.at(j_begin + 1) = std::move(node->children_.at(node->next_index_));
  for (j = j_begin, i = node->next_index_ - 1; i > half_; --i, --j) {
    new_node->entries_.at(j) = std::move(node->entries_.at(i));
    new_node->children_.at(j) = std::move(node->children_.at(i));
  }

  node->next_index_ = half_;
  new_node->next_index_ = half_ + (even_M_ ? 0 : 1);

  return std::make_pair(new_node, mid);
}

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::BTree() : size_(0), half_(std::floor(_M / 2)),
  root_(new BTreeNode()), even_M_(_M % 2 == 0) { }

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::~BTree() {
  delete root_;
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::insert(const _Key& key, const _Data& data) {
  std::pair<_Key, _Data> new_entry = std::make_pair(key, data);
  std::pair<BTreeNode*, std::pair<_Key, _Data>> last = insert(root_, new_entry);
  
  // need to split the root node
  if (last.first != nullptr) {
    BTreeNode* new_root = new BTreeNode();

    new_root->entries_.at(0) = last.second;
    new_root->children_.at(0) = root_;
    new_root->children_.at(1) = last.first;
    new_root->next_index_ = 1;

    root_ = new_root;
  }
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename std::pair<typename BTree<_Key, _Data, _M>::BTreeNode*, std::pair<_Key, _Data>> BTree<_Key, _Data, _M>::insert(
  BTreeNode* node, std::pair<_Key, _Data>& entry)
{ 
  int i = 0;
  std::pair<BTreeNode*, std::pair<_Key, _Data>> propagate;
  size_t insertion_index = lower_bound_index(entry.first, node->entries_.cbegin(),
    node->entries_.cbegin() + node->next_index_
  );

  // duplicate
  if (node->entries_.at(insertion_index).first == entry.first) {
    node->entries_.at(insertion_index).second = entry.second;
    return propagate;
  }

  if (!node->leaf()) {
    propagate = insert(node->children_.at(insertion_index), entry);

    if (propagate.first == nullptr) {
      return propagate;
    }
  }

  for (i = node->next_index_ - 1; i >= static_cast<int>(insertion_index); --i) {
    node->entries_.at(i + 1) = std::move(node->entries_.at(i));
    node->children_.at(i + 2) = std::move(node->children_.at(i + 1));
  }

  if (propagate.first != nullptr) {
    node->entries_.at(insertion_index) = propagate.second;
    node->children_.at(insertion_index + 1) = propagate.first;
  } else {
    node->entries_.at(insertion_index) = entry;
    ++size_;
  }

  ++node->next_index_;

  if (node->full()) {
    return split(node, entry);
  }  

  return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
}



template<class _Key, class _Data, size_t _M>
std::pair<_Key, _Data> BTree<_Key, _Data, _M>::search(const _Key& key) const {
  return search(root_, key);
}

template<class _Key, class _Data, size_t _M>
std::pair<_Key, _Data> BTree<_Key, _Data, _M>::search(const BTreeNode* node,
  const _Key& key) const 
{
  size_t index = lower_bound_index(key, node->entries_.cbegin(),
    node->entries_.cbegin() + node->next_index_
  );

  if (node->entries_.at(index).first == key) {
    return node->entries_.at(index);
  }

  if (node->leaf()) {
    return std::make_pair(_Key{}, _Data{});
  }

  return search(node->children_.at(index), key);
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::remove() {
  if (empty()) {
    return; // TODO: exception
  }


}

template<class _Key, class _Data, size_t _M>
bool BTree<_Key, _Data, _M>::empty() const {
  return (size_ <= 0);
}

template<class _Key, class _Data, size_t _M>
size_t BTree<_Key, _Data, _M>::size() const {
  return size_;
}

// debug, remove once tests succeed (or replace by operator<<)
template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::print() const {
  std::cout << "Tree size: " << size_ << std::endl;
  print(root_, 0);
  std::cout << std::endl;
}

// debug, remove once tests succeed and print() is removed
template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::print(BTreeNode* node, int level) const {
  if (node == nullptr) {
    return;
  }

  for (int i = 0; i < level; ++i) {
    std::cout << "  ";
  }
  node->print(); std::cout << std::endl;
  
  if (!node->leaf()) {
    for (int i = 0; i <= node->next_index_; ++i) {
      print(node->children_.at(i), level + 1);
    }
  }
}

} // namespace data_structures

#endif // DATA_STRUCTURES_B_TREE_H
