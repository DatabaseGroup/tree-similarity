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
  struct BTreeNode {
    // hopefully better performance using std::array (evaluated at compile time)
    std::array<std::pair<_Key, _Data>, _M> entries_{}; // std::pair<>& possible?
    std::array<BTreeNode*, _M + 1> children_{};
    size_t max_index_;

    BTreeNode() {
      max_index_ = 0;
    }

    BTreeNode(const _Key& key, const _Data& data) : BTreeNode() {
      entries_.at(0) = std::make_pair(key, data);
    }

    ~BTreeNode() { }

    bool insert(const _Key& key, const _Data& data) {
      if (full()) {
        return false;
      }

      size_t key_index = search(key);
      // move array elements if necessary
      if (key_index < max_index_) {
        move_entries(this, key_index); // maybe key_index +- 1 - to be tested
      }

      entries_.at(key_index + 1) = std::make_pair(key, data);
      ++max_index_;

      return true;
    }

    // shifts entries by 1 from start_index to end_index (inclusive)
    void move_entries(BTreeNode* other, const size_t& start_index = 0,
      const size_t& end_index = _M - 1, const int offset = 1)
    {
      if (end_index >= entries_.size() || start_index < 0) {
        return; // TODO: throw exception
      }

      for (int i = end_index - 1; i >= start_index; --i) {
        entries_.at(i + offset) = std::move(other->entries_.at(i));
      }
    }

    // returns index wrt. the entries array, children array is of different size
    size_t search(const _Key& key) const {
      if (max_index_ < 0) {
        return (-1); // TODO: exception
      }

      if (max_index_ == 0 && key == entries_.at(max_index_).first) {
        return max_index_;
      }

      typename std::array<std::pair<_Key, _Data>, _M>::iterator lower_end =
        std::lower_bound(entries_.begin(), entries_.begin() + max_index_, key,
          [](std::pair<_Key, _Data> lhs, std::pair<_Key, _Data> rhs) -> bool {
            return (lhs.first < rhs.first);
          }
        );

      return (lower_end - entries_.begin());
    }

    bool leaf() const {
      return (children_.at(0) == nullptr);
    }

    bool full() const {
      return (entries_.size() == max_index_);
    }


  };

private:
  BTreeNode* root_;
  size_t number_of_nodes_;

  BTreeNode* split(const BTreeNode* root);

public:
  BTree();
  ~BTree();

  void insert(const _Key& key, const _Data& data);
  BTreeNode* insert(const BTreeNode* root, const _Key& key, const _Data& data);
  _Data get(const _Key& key) const;
  _Data search(const BTreeNode* root, const _Key& key) const;
  void remove();

  bool empty() const;
};

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::BTree() : number_of_nodes_(0) {
  BTreeNode* root_ = new BTreeNode();
}

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::~BTree() {
  delete root_;
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::insert(const _Key& key, const _Data& data) {
  BTreeNode* new_node = insert(root_, key, data);
  ++number_of_nodes_;

  if (new_node == nullptr) {
    return;
  }

  // split root node
  //BTreeNode* new_root = new BTreeNode();
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::insert(
  const BTreeNode* root, const _Key& key, const _Data& data)
{
  if (!root->insert(key, data)) {
    // node is full, need to split
    split(root);
  }
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::split(
  const BTreeNode* root)
{
  BTreeNode* new_node = new BTreeNode();
  const int half = std::floor(_M / 2);
  new_node->move_entries(root, half, root->max_index_, half);
}

template<class _Key, class _Data, size_t _M>
_Data BTree<_Key, _Data, _M>::get(const _Key& key) const {
  return search(root_, key);
}

template<class _Key, class _Data, size_t _M>
_Data BTree<_Key, _Data, _M>::search(const BTreeNode* root, const _Key& key) const 
{
  size_t key_index = (-1);
  if (root->leaf()) {
    if ((key_index = root->search(key)) >= 0) {
      return root->entries.at(key_index).second;
    }
  } else { // non-leaf
    key_index = root->search(key);
    return search(root->children.at(key_index + 1), key);
  }
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::remove() {

  --number_of_nodes_;
}

template<class _Key, class _Data, size_t _M>
bool BTree<_Key, _Data, _M>::empty() const {
  return (number_of_nodes_ <= 0);
}

} // namespace data_structures

#endif // DATA_STRUCTURES_B_TREE_H
