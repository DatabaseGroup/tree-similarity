#ifndef STRUCTURE_SEARCH_STRUCTURE_SEARCH_H
#define STRUCTURE_SEARCH_STRUCTURE_SEARCH_H

#include <vector>
#include <stack>
#include <list>
#include <set>

#include "../data_structures/k_heap.h"
#include "../data_structures/posting_list_container.h"

namespace structure_search {

template<class _NodeData>
void filter_and_add(nodes::Node<_NodeData>* query,
  std::multiset<_NodeData>& labels, std::multiset<_NodeData>& labels_query,
  data_structures::DeweyIdentifier& dewey_id, int& m,
  data_structures::KHeap<nodes::Node<_NodeData>>& results)
{
  
}

template<class _NodeData>
data_structures::KHeap<nodes::Node<_NodeData>> naive_search(
  nodes::Node<_NodeData>* query, std::vector<nodes::Node<_NodeData>*> trees,
  int& m, const int& k,
  data_structures::BTree<_NodeData, std::list<data_structures::DeweyIdentifier>>& label_index,
  std::multiset<_NodeData>& labels_query)
{
  data_structures::PostingListContainer<_NodeData> pl;

  std::pair<bool, std::pair<_NodeData, std::list<data_structures::DeweyIdentifier>>> fp;
  for (auto& label: labels_query) {
    fp = label_index.search(label);

    if (fp.first) {
      pl.push_back(std::make_pair(label, fp.second.second));
    }
  }

  std::cout << pl.size() << std::endl;
  std::stack<std::pair<data_structures::DeweyIdentifier, std::multiset<_NodeData>>> s;
  data_structures::KHeap<nodes::Node<_NodeData>> results(6);

  std::pair<data_structures::DeweyIdentifier, std::multiset<_NodeData>> previous;
  while (!pl.empty()) {
    auto p = pl.next();
    std::multiset<_NodeData> labels;

    _NodeData& current_label = p.second;
    data_structures::DeweyIdentifier& current_dewey_id = p.first;

    while (!s.empty() && !(previous = s.top()).first.is_prefix(p.first)) {
      s.pop();

      // TODO: is this the best/fastet way to build the union of 2 multisets
      // or rather use std::set_union ...
      // O(n log (n + k)), n = distance between the two multisets,
      // k = size of the calling multiset (labels)
      labels.insert(previous.second.begin(), previous.second.end());

      // no return value, m is modified since it's passed by reference
      filter_and_add(query, labels, labels_query, previous.first, m, results);
    }
  }

  return results;
}

}

#endif // STRUCTURE_SEARCH_STRUCTURE_SEARCH_H