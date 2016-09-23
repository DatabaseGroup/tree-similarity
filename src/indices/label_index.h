#ifndef INDICES_LABEL_INDEX_H
#define INDICES_LABEL_INDEX_H

#include <list>
#include <vector>

#include "../data_structures/btree.h"

namespace indices {

/**
 *  Represents a set of posting lists as described in "Indexing for Subtree
 *  Similarity-Search using Edit Distance" by Sarah Cohen, Department of Computer
 *  Science and Engineering, Hebrew University of Jerusalem, Israel.
 *
 *  Basically this data structure encapsulates a set of posting lists (one for
 *  each label in the query). For each posting list, a pointer is initialized
 *  which points at the first Dewey identifier in the posting list. Based on this
 *  pointer, two operations are supported:
 *    - next()  Search all pointers to posting lists for the one pointing to the
 *              smallest Dewey identifier (according to pre-ordering). The
 *              corresponding pair is returned an the pointer is incremented.
 *    - empty() Returns true if all pointers to posting lists have reached the
 *              end of their lists.
 *  In essence, this is the Label Index, described on p. 51 of the paper.
 */
template<class _LabelType, class _ListEntryType, size_t _M = 10>
class LabelIndex {
protected:
  // For every label l in a tree, we store the list of Dewey identifiers of nodes
  // v such that v is labeled l
  // BTree is used to store (label, list of Dewey identifiers) pairs. A node can
  // store up to m = 10 keys (hence, 11 children)
  // TODO: maybe use std::vector instead of std::list and store position?
  // according to Bjarne Stroustrup, vector >> list in almost every situation
  // TODO: DeweyIdentifier class instead of std::vector<int>?
  data_structures::BTree<_LabelType, std::list<_ListEntryType>, _M> posting_lists;

public:
  LabelIndex();
  ~LabelIndex();

  size_t size() const;
  bool empty() const;
};

template<class _LabelType, class _ListEntryType, size_t _M>
LabelIndex<_LabelType, _ListEntryType, _M>::LabelIndex() { }

template<class _LabelType, class _ListEntryType, size_t _M>
LabelIndex<_LabelType, _ListEntryType, _M>::~LabelIndex() { }

template<class _LabelType, class _ListEntryType, size_t _M>
size_t LabelIndex<_LabelType, _ListEntryType, _M>::size() const {
  return posting_lists.size();
}

template<class _LabelType, class _ListEntryType, size_t _M>
bool LabelIndex<_LabelType, _ListEntryType, _M>::empty() const {
  return posting_lists.empty();
}

} // namespace indices

#endif // INDICES_LABEL_INDEX_H