#ifndef TASM_TASM_H
#define TASM_TASM_H

#include "ring_buffer.h"
#include "../common/common.h"
#include "../zhang_shasha/zhang_shasha.h"
#include "../data_structures/k_heap.h"
#include "../wrappers/node_distance_pair.h"
#include "../wrappers/node_prefixes_pair.h"

#include <vector>
#include <queue>
#include <utility>

#include <iomanip> // setw

// TODO
// -  Examine if movement of ring buffer stuff to RingBuffer class makes sense
//    and if this is the case, do it (ring buffers share start, end, ...).
// -  Change template parameters to uppercase, e.g. '_Node' instead of '_node'
// -  Documentation (also KHeap)

namespace tasm {

template<class _node = nodes::Node, class _costs = nodes::Costs<_node>>
std::vector<_node> prb_pruning(std::queue<_node>& postorder_queue,
  const int threshold)
{
  std::vector<_node> candidates; // does this have to be a set?

  RingBuffer<_node> ring_buffer(threshold);
  RingBuffer<size_t> prefix_array(threshold);
  size_t start = 1;
  size_t end = 1;
  size_t appended = 0;
  size_t buffer_size = ring_buffer.get_size();

  prb_next(ring_buffer, prefix_array, start, end, appended, postorder_queue,
    threshold
  );

  while (start != end) {
    //std::cout << "Add " << ring_buffer[prefix_array[start] % buffer_size].get_label() << " to candidates" << std::endl;
    candidates.push_back(ring_buffer[prefix_array[start] % buffer_size]);
    start = ((prefix_array[start] + 1) % buffer_size);
    prb_next(ring_buffer, prefix_array, start, end, appended, postorder_queue,
      threshold
    );
  }

  return candidates;
}

// lbl = ring_buffer, pfx = prefix_array, s = start, e = end, c = appended
// pq = postorder_queue, tau = threshold
template<class _node = nodes::Node, class _costs = nodes::Costs<_node>>
void prb_next(RingBuffer<_node>& ring_buffer, RingBuffer<size_t>& prefix_array,
  size_t& start, size_t& end, size_t& appended, std::queue<_node>& postorder_queue,
  const int threshold)
{
  const size_t buffer_size = ring_buffer.get_size();
  _node dequeued_element;
  while (!postorder_queue.empty() || start != end) {
    if (!postorder_queue.empty()) {
      dequeued_element = postorder_queue.front();
      postorder_queue.pop();

      ring_buffer[end] = dequeued_element;   
      prefix_array[end] = (++appended) - dequeued_element.get_subtree_size() + 1;

      if (dequeued_element.get_subtree_size() <= threshold) {
        prefix_array[prefix_array[end] % buffer_size] = appended;
      }

      // debug
      /*
      std::cout << "start = " << start << ", end = " << end <<
        ", appended = " << appended << std::endl;
      std::cout << prefix_array[end] << " mod " << buffer_size << " = " <<
        prefix_array[end] % buffer_size << std::endl;
      std::cout << "lbl = | ";
      for (int i = 0; i < buffer_size; ++i) {
        std::cout << std::setw(12);
        std::cout << ring_buffer[i].get_label();
        std::cout << " | ";
      }
      std::cout << std::endl;
      std::cout << "pfx = | ";
      for (int i = 0; i < buffer_size; ++i) {
        std::cout << std::setw(12);
        std::cout << prefix_array[i];
        std::cout << " | ";
      }
      std::cout << std::endl << std::endl;
      */
      // debug

      end = ((end + 1) % buffer_size);
    }

    if ((start == ((end + 1) % buffer_size)) || postorder_queue.empty()) {
      if (prefix_array[start] >= (appended + 1 - (end - start + buffer_size) % buffer_size)) {
        return;
      } else {
        start = ((start + 1) % buffer_size);
      }
    }
  }
}

// Computes the distance between the query and every subtree in the document.
// Requires n distance computations, thus requiring O(m^2*n^2) time and O(m*n)
// space (m ... size of the query tree, n ... size of the document)
template<class _node = nodes::Node, class _costs = nodes::Costs<_node>>
data_structures::KHeap<wrappers::NodeDistancePair<_node>> naive(_node& query,
  _node& document, const int& k)
{
  //std::priority_queue<std::pair<_node, double>> ranking;
  data_structures::KHeap<wrappers::NodeDistancePair<_node>> ranking(k);
  
  if (k == 0) {
    return ranking;
  }

  double ted = 0.0;
  std::vector<_node*>* postorder_document = common::generate_postorder(&document);

  for (_node* subtree: *postorder_document) {
    ted = zhang_shasha::compute_zhang_shasha<_node, _costs>(&query, subtree);
    if (ranking.size() < k || ted < ranking.front().get_distance()) {
      wrappers::NodeDistancePair<_node> in_ranking(*subtree, ted);
      if (!ranking.insert(in_ranking)) {
        ranking.replace_front(in_ranking);
      }
    }
  }

  return ranking;
}

template<class _node = nodes::Node, class _costs = nodes::Costs<_node>>
data_structures::KHeap<wrappers::NodeDistancePair<_node>> tasm_postorder(
  _node& query, std::queue<_node>& postorder_queue, const int& k)
{
  data_structures::KHeap<wrappers::NodeDistancePair<_node>> ranking(k);

  // TODO

  return ranking;
}

// TODO: optimize (avoid recomputation of prefixes, ...)
template<class _node = nodes::Node>
void prefixes(_node& subroot, std::vector<_node*>* postorder,
  std::vector<wrappers::NodePrefixesPair<_node>*>& subtrees)
{
  subtrees.push_back(new wrappers::NodePrefixesPair<_node>(subroot));
  for (int i = 0; i < subtrees.at(subroot.get_id() - 1)->get_root().get_id(); ++i) {
    subtrees.at(subroot.get_id() - 1)->add_prefix(
      new std::vector<_node*>(postorder->begin(), postorder->begin() + i + 1)
    );
  }
}

// get all subtrees (extended to only include relevant subtrees)
// TODO: optimize (avoid recomputation, ...)
template<class _node = nodes::Node>
void decompose(_node& root,
  std::vector<wrappers::NodePrefixesPair<_node>*>& subtrees)
{
  std::vector<_node*>* postorder_tree = common::generate_postorder(&root);
  for (auto& node: *postorder_tree) {
    prefixes<_node>(*node, postorder_tree, subtrees);
  }
}

template<class _node = nodes::Node, class _costs = nodes::Costs<_node>>
unsigned int node_alignment_costs(_node* query_node, _node* document_node,
  _costs& costs)
{
  bool no_document_node = (document_node == &nodes::empty_string_node);
  bool no_query_node = (query_node == &nodes::empty_string_node);
  
  if (no_document_node && !no_query_node) {
    return costs.del(query_node);
  } else if (!no_document_node && no_query_node) {
    return costs.ins(document_node);
  }

  return costs.ren(query_node, document_node);
}


// ranking is modified during execution, therefore no return value
template<class _node = nodes::Node, class _costs = nodes::Costs<_node>>
void tasm_dynamic(_node& query, _node& document, const int& k,
  data_structures::KHeap<_node>& ranking)
{
  const int query_size = query.get_subtree_size();
  const int document_size = document.get_subtree_size();

  data_structures::Array2D<int> td(query_size, document_size);
  data_structures::Array2D<int> pd(query_size + 1, document_size + 1);

  // compute all subtrees of the query/document and corresponding prefixes
  std::vector<wrappers::NodePrefixesPair<_node>*> query_subtrees;
  decompose<_node>(query, query_subtrees);
  std::vector<wrappers::NodePrefixesPair<_node>*> document_subtrees;
  decompose<_node>(document, document_subtrees);

  _costs costs;

  // for all relevant subtrees of the query
  for (auto& query_subtree: query_subtrees) { // TODO: only relevant
    std::vector<_node*>* query_nodes_asc = query_subtree->get_prefixes().at(query_subtree->get_prefixes().size() - 1);
    for(auto& document_subtree: document_subtrees) { // TODO: only relevant
      pd[0][0] = 0; // may be omitted since Array2D initializes all elements to zero
      std::vector<_node*>* document_nodes_asc = document_subtree->get_prefixes().at(document_subtree->get_prefixes().size() - 1);
      for (auto& document_node: *document_nodes_asc) {
        pd[0][document_node->get_id()] = pd[0][document_node->get_id()] +
          node_alignment_costs(&nodes::empty_string_node, document_node, costs);
        for (auto& query_node: *query_nodes_asc) {
          pd[query_node->get_id()][0] = pd[query_node->get_id()][0] +
            node_alignment_costs(query_node, &nodes::empty_string_node, costs);
        }
      }
    }
  }

  std::cout << "pd:" << std::endl;
  for (int i = 0; i < pd.get_rows(); ++i) {
    for (int j = 0; j < pd.get_columns(); ++j) {
      std::cout << std::setw(5);
      std::cout << pd[i][j];
    }
    std::cout << std::endl;
  }
}

} // namespace tasm

#endif // TASM_TASM_H