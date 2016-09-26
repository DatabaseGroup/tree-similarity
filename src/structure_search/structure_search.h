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
data_structures::KHeap<nodes::Node<_NodeData>> naive_search(
  nodes::Node<_NodeData>* query, std::vector<nodes::Node<_NodeData>*> trees,
  int& m, const int& k,
  data_structures::BTree<_NodeData, std::list<data_structures::DeweyIdentifier>>& label_index,
  std::set<_NodeData>& labels_query)
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

  while (!pl.empty()) {
    auto p = pl.next();
    std::multiset<_NodeData> labels;

    //while (!s.empty() && !s.top().first.is_prefix(p.first)) {

    //}
  }

  return results;
}

}

#endif // STRUCTURE_SEARCH_STRUCTURE_SEARCH_H