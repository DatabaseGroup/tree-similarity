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
 *
 *  References:
 *    - http://www.cs.carleton.edu/faculty/jgoldfea/cs201/spring11/inclass/Tree/BTreefinalNew.pdf
 *    - https://en.wikipedia.org/wiki/B-tree
 *    - https://www.cs.usfca.edu/~galles/visualization/BTree.html
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
    int next_index_{};

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

  int lower_bound_index(const _Key& key,
    typename BTreeNode::EntriesType::const_iterator begin,
    typename BTreeNode::EntriesType::const_iterator end) const;
  std::pair<BTreeNode*, std::pair<_Key, _Data>> insert(BTreeNode* node,
    std::pair<_Key, _Data>& entry);
  std::pair<BTreeNode*, std::pair<_Key, _Data>> split(
    BTreeNode* node, const std::pair<_Key, _Data>& entry);
  std::pair<_Key, _Data> search(const BTreeNode* node, const _Key& key) const;
  BTreeNode* remove(BTreeNode* node, const _Key& key);
  BTreeNode* rebalance(BTreeNode* node, BTreeNode* propagate, int& removal_index);
  BTreeNode* rotate_left(BTreeNode* node, BTreeNode* propagate, BTreeNode* right,
    int& separator_index);
  BTreeNode* rotate_right(BTreeNode* node, BTreeNode* propagate, BTreeNode* left,
    int& separator_index);
  BTreeNode* merge(BTreeNode* node, BTreeNode* propagate, BTreeNode* left,
    int& separator_index);
  BTreeNode* replace_separator(BTreeNode* node,
    typename std::pair<_Key, _Data>& separator);

  // debug, remove once tests succeed and print() is removed
  void print(BTreeNode* node, int level) const;

public:
  BTree();
  ~BTree();

  void insert(const _Key& key, const _Data& data);
  std::pair<_Key, _Data> search(const _Key& key) const;
  void remove(const _Key& key);
  bool empty() const;
  size_t size() const;

  // debug, remove once tests succeed (or replace by operator<<)
  void print() const;
};

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
  ++size_;

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

template<class _Key, class _Data, size_t _M>
std::pair<_Key, _Data> BTree<_Key, _Data, _M>::search(const _Key& key) const {
  return search(root_, key);
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::remove(const _Key& key) {
  if (empty()) {
    return; // TODO: exception
  }

  --size_;
  BTreeNode* propagate = remove(root_, key);

  if ((propagate != nullptr) && (root_->next_index_ == 0)) {
    //delete root_;
    root_ = propagate;
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

template<class _Key, class _Data, size_t _M>
int BTree<_Key, _Data, _M>::lower_bound_index(const _Key& key,
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
typename std::pair<typename BTree<_Key, _Data, _M>::BTreeNode*, std::pair<_Key, _Data>> BTree<_Key, _Data, _M>::insert(
  BTreeNode* node, std::pair<_Key, _Data>& entry)
{ 
  int i = 0;
  std::pair<BTreeNode*, std::pair<_Key, _Data>> propagate;
  size_t insertion_index = lower_bound_index(entry.first, node->entries_.cbegin(),
    node->entries_.cbegin() + node->next_index_
  );

  // duplicate
  if (insertion_index < node->next_index_ && node->entries_.at(insertion_index).first == entry.first) {
    node->entries_.at(insertion_index).second = entry.second;
    --size_;
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
    
  }

  ++node->next_index_;

  if (node->full()) {
    return split(node, entry);
  }  

  return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
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
std::pair<_Key, _Data> BTree<_Key, _Data, _M>::search(const BTreeNode* node,
  const _Key& key) const 
{
  size_t index = lower_bound_index(key, node->entries_.cbegin(),
    node->entries_.cbegin() + node->next_index_
  );

  if ((index < node->next_index_) && (node->entries_.at(index).first == key)) {
    return node->entries_.at(index);
  }

  if (node->leaf()) {
    return std::make_pair(_Key{}, _Data{});
  }

  return search(node->children_.at(index), key);
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::remove(
  BTreeNode* node, const _Key& key)
{
  if (node == nullptr) {
    return nullptr;
  }

  int removal_index = lower_bound_index(key, node->entries_.cbegin(),
    node->entries_.cbegin() + node->next_index_
  );

  // leaf node and key is found
  if (node->leaf()) {
    if (node->entries_.at(removal_index).first == key) {
      for (int i = removal_index; i < node->next_index_ - 1; ++i) {
        node->entries_.at(i) = std::move(node->entries_.at(i + 1));
      }
      --node->next_index_;

      if (node->next_index_ >= half_) {
        // tree stays balanced (leaf has enough element), nothing to propagate
        return nullptr; 
      }

      // balance tree (leaf has too little elements), propagate leaf upwards
      return node;
    }

    // tree stays balanced (key not found), nothing to propagate
    ++size_;
    return nullptr;
  }

  // internal node
  // key found in internal node
  if ((removal_index < node->next_index_) &&
    (node->entries_.at(removal_index).first == key))
  {
    // choose new separator (largest element of left subtree wrt. key position)
    // and replace element at key position with new separator
    BTreeNode* propagate = replace_separator(node->children_.at(removal_index),
      node->entries_.at(removal_index)
    );

    // rebalance tree if necessary (rebalancing may be necessary up to the root)
    if (propagate == nullptr) {
      return propagate;
    }

    return rebalance(node, propagate, removal_index);
  }

  // if propagate != nullptr, then the current node needs to be balanced
  // (propagated from the corresponding child which was also balanced)
  BTreeNode* propagate = remove(node->children_.at(removal_index), key);

  if (propagate == nullptr) {
    // tree is balanced, nothing to propagate
    return propagate;
  }

  // rebalance tree
  return rebalance(node, propagate, removal_index);
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::rebalance(
  BTreeNode* node, BTreeNode* propagate, int& removal_index)
{
  BTreeNode* left = nullptr;
  BTreeNode* right = nullptr;
  int separator_index = removal_index;

  if (removal_index > 0) {
    left = node->children_.at(removal_index - 1);
  }

  if (removal_index < node->next_index_) {
    right = node->children_.at(removal_index + 1);
  }

  int i = 0;
  bool has_left_sibling = (left != nullptr);
  bool has_right_sibling = (right != nullptr);

  // check whether right sibling of propagate is usable
  if (has_right_sibling && (right->next_index_ > half_)) {
    // propagate is not the right-most child (it has a right sibling)
    return rotate_left(node, propagate, right, separator_index);
  } else if (has_left_sibling && (left->next_index_ > half_)) {
    // propagate is not the left-most child (it has a left sibling)
    return rotate_right(node, propagate, left, --separator_index);
  } 

  // propagate has no left or right sibling holding enough elements (> M/2)
  if (left != nullptr) {
    //std::cout << "Merge 1: " << (propagate == left) << std::endl;
    return merge(node, propagate, left, --separator_index);
  }

  //std::cout << "Merge 2" << std::endl;
  return merge(node, right, propagate, separator_index);
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::rotate_left(
  BTreeNode* node, BTreeNode* propagate, BTreeNode* right, int& separator_index)
{
  // move separator to the end of the deficient node
  propagate->entries_.at(propagate->next_index_) = std::move(node->entries_.at(separator_index));
  ++propagate->next_index_;

  // replace separator with the first element of right sibling
  node->entries_.at(separator_index) = std::move(right->entries_.at(0));
  
  // move left-most child of right accordingly (to propagate)
  if (!propagate->leaf()) {
    propagate->children_.at(propagate->next_index_) = right->children_.at(0);
  }

  // shift elements of right sibling by 1 to the left
  right->children_.at(0) = right->children_.at(1);
  for (int i = 0; i < right->next_index_; ++i) {
    right->entries_.at(i) = std::move(right->entries_.at(i + 1));
    right->children_.at(i + 1) = std::move(right->children_.at(i + 2));
  }
  --right->next_index_;

  // tree is balanced, thus nothing to propagate
  return nullptr;
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::rotate_right(
  BTreeNode* node, BTreeNode* propagate, BTreeNode* left, int& separator_index)
{
  // move separator to the start of the deficient node
  // thus shift elements of deficient node by 1 to the right
  if (!propagate->leaf()) {
    propagate->children_.at(propagate->next_index_+ 1) =
      std::move(propagate->children_.at(propagate->next_index_));
  }

  for (int i = propagate->next_index_ - 1; i >= 0; --i) {
    propagate->entries_.at(i + 1) = std::move(propagate->entries_.at(i));
    propagate->children_.at(i + 1) = std::move(propagate->children_.at(i));
  }
  propagate->entries_.at(0) = std::move(node->entries_.at(separator_index));
  ++propagate->next_index_;

  // replace separator with the last element of the left sibling
  node->entries_.at(separator_index) = std::move(left->entries_.at(left->next_index_ - 1));

  // move right-most child accordingly (to propagate)
  if (!propagate->leaf()) {
    propagate->children_.at(0) = std::move(left->children_.at(left->next_index_));
  }
  --left->next_index_;

  // tree is balanced, thus nothing to propagate
  return nullptr;
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::merge(
  BTreeNode* node, BTreeNode* propagate, BTreeNode* left, int& separator_index)
{
  // move separator to the end of the left sibling
  left->entries_.at(left->next_index_) = std::move(node->entries_.at(separator_index));
  ++left->next_index_;

  // remove separator and corresponding child (empty child)
  int i = 0;
  for (i = separator_index; i < node->next_index_; ++i) {
    node->entries_.at(i) = std::move(node->entries_.at(i + 1));
    node->children_.at(i + 1) = std::move(node->children_.at(i + 2));
  }
  node->children_.at(i + 1) = nullptr;
  --node->next_index_;

  // move elements from the right node to the left sibling
  for (i = 0; i < propagate->next_index_; ++i) {
    left->entries_.at(left->next_index_) = std::move(propagate->entries_.at(i));
    left->children_.at(left->next_index_) = std::move(propagate->children_.at(i));
    ++left->next_index_;
  }
  
  if (!left->full()) {
    left->children_.at(left->next_index_) = std::move(propagate->children_.at(i));
  }

  delete propagate; // is empty now

  if (node->next_index_ < half_) {
    return ((node == root_) ? left : node);
  }

  // tree is balanced (enough elements in node), thus nothing to propagate
  return nullptr;
}

template<class _Key, class _Data, size_t _M>
typename BTree<_Key, _Data, _M>::BTreeNode* BTree<_Key, _Data, _M>::replace_separator(
    BTreeNode* node, std::pair<_Key, _Data>& separator)
{
  if (node == nullptr) {
    return nullptr;
  }

  if (node->leaf()) {
    separator = std::move(node->entries_.at(node->next_index_ - 1));
    --node->next_index_; // remove new separator from node, essentially

    if (node->next_index_ < half_) {
      return node;
    }

    return nullptr;
  }

  BTreeNode* propagate = replace_separator(node->children_.at(node->next_index_),
    separator
  );

  if (propagate != nullptr) {
    return rebalance(node, propagate, node->next_index_);
  }

  return nullptr;
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
