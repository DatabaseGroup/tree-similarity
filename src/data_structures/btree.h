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
    typedef std::array<std::pair<_Key, _Data>, _M> EntriesType;
    typedef std::array<BTreeNode*, _M + 1> ChildrenType;
    // hopefully better performance using std::array (evaluated at compile time)
    EntriesType entries_{};
    ChildrenType children_{};
    size_t next_index_{};

    BTreeNode() { }
    ~BTreeNode() { }

    // debug, to be removed once tests succeed
    void print() const { 
      std::cout << "{ ";
      for (int i = 0; i < next_index_; ++i) {
        std::cout << "(" << entries_.at(i).first << ", " << entries_.at(i).second << ") ";
      }
      std::cout << "} (next_index_ = " << next_index_ << ")" << std::endl;
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
    BTreeNode* node, const std::pair<_Key, _Data>& entry,
    BTreeNode* new_child = nullptr);
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
    const std::pair<_Key, _Data>& entry
  );
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
size_t BTree<_Key, _Data, _M>::lower_bound_index(const _Key& key,
  typename BTree<_Key, _Data, _M>::BTreeNode::EntriesType::const_iterator begin,
  typename BTree<_Key, _Data, _M>::BTreeNode::EntriesType::const_iterator end) const
{
  //std::cout << begin->first << " - " << end->first << std::endl;

  // O(log n) in the size of the entries_ array
  typename BTreeNode::EntriesType::const_iterator lower_bound =
    std::lower_bound(begin, end, key,
      [](const std::pair<_Key, _Data>& lhs, const _Key& rhs) -> bool {
        return (lhs.first < rhs);
      }
    );

  //std::cout << "lower_bound_index = " << lower_bound - begin << std::endl;

  // return the index of the first element >= key
  return (lower_bound - begin);
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename std::pair<typename BTree<_Key, _Data, _M>::BTreeNode*, std::pair<_Key, _Data>> BTree<_Key, _Data, _M>::split(
  BTreeNode* node, const std::pair<_Key, _Data>& entry, BTreeNode* new_child)
{
  BTreeNode* new_node = new BTreeNode();
  int i = 0, j = 0;
  size_t half_index = half_ - (even_M_ ? 1 : 0);
  size_t insertion_index = 0;

  // Add new child if available (preserve ordering)
  if (new_child) {
    _Key last_key_new_child = new_child->entries_.at(new_child->next_index_ - 1).first;
    insertion_index = lower_bound_index(last_key_new_child,
      node->entries_.cbegin(), (is_full(node) ? node->entries_.cend() : node->entries_.cbegin() + node->next_index_)
    );

    for (i = node->next_index_ - 1; i > insertion_index; --i) {
      node->children_.at(i + 1) = std::move(node->children_.at(i));
    }
    node->children_.at(insertion_index + 1) = new_child;
  }

  insertion_index = lower_bound_index(entry.first,
    node->entries_.cbegin(), (is_full(node) ? node->entries_.cend() : node->entries_.cbegin() + node->next_index_)
  ) - half_ - 1;

  std::cout << "SPLIT: " << half_ << " "; node->print();
  print();

  std::pair<_Key, _Data> mid = entry;
  if ((half_ > 0) && (entry.first < node->entries_.at(half_ - 1).first)) {
    update_mid(mid, node->entries_.at(half_ - 1), entry);
    //++insertion_index;
  } else if (entry.first > node->entries_.at(half_).first) {
    update_mid(mid, node->entries_.at(half_), entry);
  }
  print();

  std::cout << "Mid: (" << mid.first << ", " << mid.second << ")" << std::endl;
  std::cout << "insertion_index = " << insertion_index << std::endl;

  //new_node->children_.at(half_index + 1) = std::move(node->children_.at(node->next_index_));
  std::cout << " +++ "; //node->children_.at(node->next_index_ - 1)->print();
  for (j = half_index, i = node->next_index_ - 1; j >= 0; --j) {
    std::cout << "1 - i = " << i << " -> j = " << j << std::endl;

    if (j == insertion_index) {
      new_node->entries_.at(j) = entry;
      continue;
    }

    if (node->entries_.at(i).first == entry.first) {
      node->entries_.at(i).second = entry.second;
    }
    
    new_node->entries_.at(j) = std::move(node->entries_.at(i));
    --i;
  }

  if (insertion_index == half_index) {
    new_node->entries_.at(insertion_index) = entry;
  }
  
  node->next_index_ = half_;
  new_node->next_index_ = half_ + (even_M_ ? 0 : 1);

  std::cout << "node: "; node->print();
  std::cout << "new_node: "; new_node->print();
  print();

  return std::make_pair(new_node, mid);
}

template<class _Key, class _Data, size_t _M>
std::pair<size_t, bool> BTree<_Key, _Data, _M>::replace_if(BTreeNode* node,
  const std::pair<_Key, _Data>& entry)
{
  bool replaced = false;
  // compute position in node
  size_t insertion_index = this->lower_bound_index(entry.first,
    node->entries_.cbegin(), (is_full(node) ? node->entries_.cend() : node->entries_.cbegin() + node->next_index_)
  );

  // check for duplicate key
  if ((insertion_index < node->entries_.size()) &&
      (node->entries_.at(insertion_index).first == entry.first)) 
  {
    node->entries_.at(insertion_index).second = entry.second;
    replaced = true;
  }

  return std::make_pair(insertion_index, replaced);
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::update_mid(std::pair<_Key, _Data>& mid,
  std::pair<_Key, _Data>& node_entry, const std::pair<_Key, _Data>& entry)
{
  std::cout << "update: " << mid.first << " -> " << node_entry.first << ", " << node_entry.first << " -> " << entry.first << std::endl;
  mid = node_entry;
  node_entry = entry;
}

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::BTree() : size_(0), half_(std::floor(_M / 2)),
  root_(new BTreeNode()), even_M_(_M % 2 == 0) { }

template<class _Key, class _Data, size_t _M>
BTree<_Key, _Data, _M>::~BTree() {
  delete root_;
}

template<class _Key, class _Data, size_t _M>
void BTree<_Key, _Data, _M>::insert(
  const _Key& key, const _Data& data)
{
  std::cout << "Inserting (" << key << ", " << data << ")" << std::endl;
  std::pair<_Key, _Data> new_entry = std::make_pair(key, data);
  insert(root_, new_entry);
}

// TODO: beautify signature (return type)
template<class _Key, class _Data, size_t _M>
typename std::pair<typename BTree<_Key, _Data, _M>::BTreeNode*, std::pair<_Key, _Data>> BTree<_Key, _Data, _M>::insert(
  BTreeNode* node, const std::pair<_Key, _Data>& entry)
{ 
  std::pair<BTreeNode*, std::pair<_Key, _Data>> propagated(node, entry);
  std::pair<size_t, bool> replace{};
  size_t insertion_index = 0;

  replace = replace_if(node, entry);
  if (replace.second) {
    return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
  }

  if (!is_leaf(node)) {
    // go down one level in the tree (in the correct branch)
    insertion_index = replace.first;
    propagated = insert(node->children_.at(insertion_index), entry);
  }

  bool propagate = (propagated.first != nullptr);
  // no further propagation necessary
  if (!propagate) {
    return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
  }
  
  if (is_full(node)) {
    //std::cout << "Is full" << ", next_index_ = " << node->next_index_ << ", size() = " << node->entries_.size() << std::endl;
    std::pair<BTreeNode*, std::pair<_Key, _Data>> splitted = split(node,
      propagated.second, propagated.first
    );

    // Am I the root node?
    if (node == root_) {
      BTreeNode* new_root = new BTreeNode();

      new_root->entries_.at(0) = splitted.second;
      new_root->children_.at(0) = node;
      //std::cout << "splitted.first: "; splitted.first->print();
      new_root->children_.at(1) = splitted.first;
      new_root->next_index_ = 1;
      ++size_;

      root_ = new_root;
      return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
    }
      
    return splitted;
  }

  // recompute position in node
  replace = replace_if(node, propagated.second);
  if (replace.second) {
    return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
  }

  // enough space; insertion in between -> shift remaining by 1 to the right
  insertion_index = replace.first;
  if (insertion_index < node->next_index_) {
    std::cout << "Shifting: " << insertion_index << std::endl;
    for (int i = node->next_index_ - 1; i >= 0 && i >= insertion_index; --i) {
      std::cout << "i = " << i << " -> " << i + 1 << std::endl;
      node->entries_.at(i + 1) = std::move(node->entries_.at(i));
      node->children_.at(i + 2) = std::move(node->children_.at(i + 1));
    }
  }

  node->entries_.at(insertion_index) = propagated.second;
  node->children_.at(insertion_index + 1) = propagated.first;
  ++size_;
  ++node->next_index_; // adjust search range in node

  return std::make_pair(nullptr, std::make_pair(_Key{}, _Data{}));
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
  return (node->next_index_ >= node->entries_.size());
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
void BTree<_Key, _Data, _M>::print(BTreeNode* root, int level) const {
  for (int i = 0; i < level; ++i) {
    std::cout << "  ";
  }
  root->print();
  
  if (!is_leaf(root)) {
    for (int i = 0; i <= root->next_index_; ++i) {
      print(root->children_.at(i), level + 1);
    }
  }
}

} // namespace data_structures

#endif // DATA_STRUCTURES_B_TREE_H
