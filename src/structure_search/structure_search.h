#ifndef STRUCTURE_SEARCH_STRUCTURE_SEARCH_H
#define STRUCTURE_SEARCH_STRUCTURE_SEARCH_H

#include <vector>
#include <stack>
#include <list>
#include <set>
#include <algorithm>
#include <cmath>

#include "../data_structures/k_heap.h"
#include "../data_structures/posting_list_container.h"
#include "../wrappers/node_index_value.h"
#include "../zhang_shasha/zhang_shasha.h"

namespace structure_search {

template<class _NodeData>
void filter_and_add(nodes::Node<_NodeData>* query,
  std::multiset<_NodeData>& labels, std::multiset<_NodeData>& labels_query,
  data_structures::DeweyIdentifier& dewey_id, int& m,
  data_structures::KHeap<std::pair<int, nodes::Node<_NodeData>*>>& results,
  data_structures::BTree<data_structures::DeweyIdentifier, wrappers::NodeIndexValue<_NodeData>>& node_index)
{
  std::multiset<_NodeData> intersection;
  std::set_intersection(labels.begin(), labels.end(), labels_query.begin(),
    labels_query.end(), std::inserter(intersection, intersection.begin())
  );

  std::cout << "FilterAndAdd: Searching for dewey_id ";
  for (const int& i: dewey_id.id_) { std::cout << i << "."; }
  std::cout << std::endl;
  wrappers::NodeIndexValue<_NodeData> node_information;
  std::pair<bool, std::pair<data_structures::DeweyIdentifier, wrappers::NodeIndexValue<_NodeData>>> search_result =
    node_index.search(dewey_id);

  if(!search_result.first) {
    // nothing found in node index
    return;
  }
  
  node_information = node_index.search(dewey_id).second.second;
  
  size_t intersection_size = intersection.size();
  int label_diff = std::max((labels_query.size() - intersection_size),
    (node_information.size() - intersection_size)
  );
  // TODO: replace 3 by depth of the query
  int depth_diff = static_cast<int>(std::abs(3 - node_information.depth()));

  if ((label_diff <= m) && (depth_diff <= m)) {
    // TODO: replace template parameters with actual correct ones (custom costs)
    int edit_distance = static_cast<int>(
      zhang_shasha::compute_zhang_shasha<_NodeData, nodes::StringCosts<_NodeData>>(
        query, node_information.root()
      )
    );

    if ((edit_distance < m) || ((edit_distance == m) && !results.full())) {
      results.insert(std::make_pair(edit_distance, node_information.root()));
    }
  }

  if (results.full()) {
    m = results.front().first;
  }
}

void generate_intermediate_dewey_ids(
  const data_structures::DeweyIdentifier& begin,
  const data_structures::DeweyIdentifier& end,
  std::vector<data_structures::DeweyIdentifier>& to_fill)
{
  if (begin.length() >= end.length()) {
    return;
  }

  size_t length_diff = end.length() - begin.length() - 1;
  // TODO: possibly use reference here (prefix)
  data_structures::DeweyIdentifier prefix = begin;
  for (int i = length_diff; i < end.length(); ++i) {
    to_fill.push_back(data_structures::DeweyIdentifier(prefix, end.at(i)));
    prefix = to_fill.back();
  }
}

template<class _NodeData>
data_structures::KHeap<std::pair<int, nodes::Node<_NodeData>*>> naive_search(
  nodes::Node<_NodeData>* query, std::vector<nodes::Node<_NodeData>*> trees,
  int& m, const int& k,
  data_structures::BTree<_NodeData, std::list<data_structures::DeweyIdentifier>>& label_index,
  data_structures::BTree<data_structures::DeweyIdentifier, wrappers::NodeIndexValue<_NodeData>>& node_index,
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
  data_structures::KHeap<std::pair<int, nodes::Node<_NodeData>*>> results(6);

  std::pair<data_structures::DeweyIdentifier, std::multiset<_NodeData>> previous;
  while (!pl.empty()) {
    auto p = pl.next();
    std::multiset<_NodeData> labels;

    _NodeData& current_data = p.second;
    data_structures::DeweyIdentifier& current_dewey_id = p.first;

    while (!s.empty() && !(previous = s.top()).first.is_prefix(p.first)) {
      s.pop();

      // TODO: is this the best/fastet way to build the union of 2 multisets
      // or rather use std::set_union ...
      // O(n log (n + k)), n = distance between the two multisets,
      // k = size of the calling multiset (labels)
      labels.insert(previous.second.begin(), previous.second.end());

      // no return value, m is modified since it's passed by reference
      filter_and_add(query, labels, labels_query, previous.first, m, results, node_index);
    }

    data_structures::DeweyIdentifier top_dewey_id{};
    if (!s.empty()) {
      previous = s.top();
      s.pop();

      std::multiset<_NodeData> labels_union = { labels };
      labels_union.insert(previous.second.begin(), previous.second.end());
      s.push(std::make_pair(previous.first, labels_union));

      top_dewey_id = previous.first;
    }


    std::vector<data_structures::DeweyIdentifier> intermediate_dewey_ids;
    generate_intermediate_dewey_ids(top_dewey_id, current_dewey_id,
      intermediate_dewey_ids
    );

    for (const data_structures::DeweyIdentifier& id: intermediate_dewey_ids) {
      s.push(std::make_pair(id, std::multiset<_NodeData>{ }));
    }

    s.push(std::make_pair(current_dewey_id, std::multiset<_NodeData>{ current_data }));
  }

  return results;
}

}

#endif // STRUCTURE_SEARCH_STRUCTURE_SEARCH_H