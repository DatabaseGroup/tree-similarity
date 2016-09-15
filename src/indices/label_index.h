#ifndef INDICES_LABEL_INDEX_H
#define INDICES_LABEL_INDEX_H

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
template<class _Type>
class LabelIndex {
private:
  data_structures::BTree<> posting_lists;

public:
  LabelIndex();
  ~LabelIndex();
};

} // namespace indices

#endif // INDICES_LABEL_INDEX_H