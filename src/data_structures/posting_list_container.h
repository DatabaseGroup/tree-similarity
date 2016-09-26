#ifndef DATA_STRUCTURES_POSTING_LIST_CONTAINER_H
#define DATA_STRUCTURES_POSTING_LIST_CONTAINER_H

#include <list>

#include "dewey_identifier.h"

namespace data_structures {

template<class _NodeData>
class PostingListContainer {
private:
  std::list<std::pair<_NodeData, std::list<DeweyIdentifier>>> posting_lists;
  size_t number_of_dewey_ids;

public:
  PostingListContainer();
  ~PostingListContainer();

  void push_back(const std::pair<_NodeData, std::list<DeweyIdentifier>>& to_insert);
  std::pair<DeweyIdentifier, _NodeData> next();
  bool empty() const;
  size_t size() const;
};

template<class _NodeData>
PostingListContainer<_NodeData>::PostingListContainer() : number_of_dewey_ids(0)
{ }

template<class _NodeData>
PostingListContainer<_NodeData>::~PostingListContainer() { }

template<class _NodeData>
void PostingListContainer<_NodeData>::push_back(
  const std::pair<_NodeData, std::list<DeweyIdentifier>>& to_insert)
{
  try {
    posting_lists.push_back(to_insert);
    number_of_dewey_ids += to_insert.second.size();
  } catch(std::exception& e) {
    throw;
  }
}

template<class _NodeData>
std::pair<DeweyIdentifier, _NodeData> PostingListContainer<_NodeData>::next() {
  if (empty()) {
    return std::make_pair(DeweyIdentifier{}, _NodeData{});
  }

  // find first non-empty Dewey identifier list
  auto it = posting_lists.begin();
  while (it->second.empty() && (it != posting_lists.end())) {
    ++it;
  }

  // once found, find min dewey identifier wrt. the remaining lists
  DeweyIdentifier min_dewey = it->second.front();
  _NodeData min_data = it->first;
  typename std::list<std::pair<_NodeData, std::list<DeweyIdentifier>>>::iterator min_it = it;
  for (; it != posting_lists.end(); ++it) {
    if (!it->second.empty() && it->second.front() < min_dewey) {
      min_dewey = it->second.front();
      min_data = it->first;
      min_it = it;
    }
  }

  min_it->second.pop_front();
  --number_of_dewey_ids;

  return std::make_pair(min_dewey, min_data);
}

template<class _NodeData>
bool PostingListContainer<_NodeData>::empty() const {
  return (number_of_dewey_ids <= 0);
}

template<class _NodeData>
size_t PostingListContainer<_NodeData>::size() const {
  return posting_lists.size();
}

} // namespace data_structures

#endif // DATA_STRUCTURES_POSTING_LIST_CONTAINER_H