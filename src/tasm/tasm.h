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

// TODO
// -  Examine if movement of ring buffer stuff to RingBuffer class makes sense
//    and if this is the case, do it (ring buffers share start, end, ...).
// -  Change template parameters to uppercase, e.g. '_Node' instead of '_node'
// -  Documentation (also KHeap)

namespace tasm {

template<class _Node = nodes::Node>
std::vector<_Node> prb_pruning(std::queue<_Node>& postorder_queue,
  const int threshold)
{
  std::vector<_Node> candidates; // does this have to be a set?

  RingBuffer<_Node> ring_buffer(threshold);
  RingBuffer<size_t> prefix_array(threshold);
  size_t start = 1;
  size_t end = 1;
  size_t appended = 0;
  size_t buffer_size = ring_buffer.get_size();

  prb_next(ring_buffer, prefix_array, start, end, appended, postorder_queue,
    threshold
  );

  while (start != end) {
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
template<class _Node = nodes::Node>
void prb_next(RingBuffer<_Node>& ring_buffer, RingBuffer<size_t>& prefix_array,
  size_t& start, size_t& end, size_t& appended, std::queue<_Node>& postorder_queue,
  const int threshold)
{
  const size_t buffer_size = ring_buffer.get_size();
  _Node dequeued_element;
  while (!postorder_queue.empty() || start != end) {
    if (!postorder_queue.empty()) {
      dequeued_element = postorder_queue.front();
      postorder_queue.pop();

      ring_buffer[end] = dequeued_element;   
      prefix_array[end] = (++appended) - dequeued_element.get_subtree_size() + 1;

      if (dequeued_element.get_subtree_size() <= threshold) {
        prefix_array[prefix_array[end] % buffer_size] = appended;
      }

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
template<class _Node = nodes::Node, class _Costs = nodes::Costs<_Node>>
data_structures::KHeap<wrappers::NodeDistancePair<_Node>> naive(_Node& query,
  _Node& document, const int& k)
{
  data_structures::KHeap<wrappers::NodeDistancePair<_Node>> ranking(k);
  
  if (k == 0) {
    return ranking;
  }

  double ted = 0.0;
  std::vector<_Node*>* postorder_document = common::generate_postorder(&document);

  for (_Node* subtree: *postorder_document) {
    ted = zhang_shasha::compute_zhang_shasha<_Node, _Costs>(&query, subtree);
    if (ranking.size() < k || ted < ranking.front().get_distance()) {
      wrappers::NodeDistancePair<_Node> in_ranking(*subtree, ted);
      if (!ranking.insert(in_ranking)) {
        ranking.replace_front(in_ranking);
      }
    }
  }

  return ranking;
}

template<class _Node = nodes::Node, class _Costs = nodes::Costs<_Node>>
unsigned int node_alignment_costs(_Node* query_node, _Node* document_node,
  _Costs& costs)
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
template<class _Node = nodes::Node, class _Costs = nodes::Costs<_Node>>
void tasm_dynamic(_Node& query, _Node& document, const int& k,
  data_structures::KHeap<wrappers::NodeDistancePair<_Node>>& ranking,
  bool plus = false)
{
  const int query_size = query.get_subtree_size();
  const int document_size = document.get_subtree_size();

  data_structures::Array2D<int> td(query_size, document_size);
  data_structures::Array2D<int> pd(query_size + 1, document_size + 1);

  std::vector<_Node*> query_leaves;
  std::vector<_Node*> document_leaves;
  zhang_shasha::make_leaves<_Node>(
    &query_leaves, &document_leaves, &query, &document
  );

  std::vector<_Node*>* query_postorder = common::generate_postorder(&query);
  std::vector<_Node*>* document_postorder = common::generate_postorder(&document);

  std::vector<int> query_lmld(query_postorder->size() + 1);
  std::vector<int> document_lmld(document_postorder->size() + 1);
  zhang_shasha::lmld<_Node>(&query, query_lmld);
  zhang_shasha::lmld<_Node>(&document, document_lmld);

  std::vector<int> query_kr;
  std::vector<int> document_kr;
  query_kr = zhang_shasha::kr(query_lmld, query_leaves.size());
  document_kr = zhang_shasha::kr(document_lmld, document_leaves.size());

  _Costs costs;

  _Node* query_subroot;
  _Node* document_subroot;
  int i = 0, j = 0;
  // compute all subtrees of the query/document and corresponding prefixes
  for ( std::vector<int>::const_iterator q_kr_cit = std::next(query_kr.cbegin());
        q_kr_cit != query_kr.cend(); ++q_kr_cit)
  {
    // *query_postorder->at(*q_kr_cit - 1) ... relevant subtree Qm of Q (m asc)
    // get (sub)root of current relevant query subtree (Qm)
    query_subroot = query_postorder->at(*q_kr_cit - 1);
    // compute prefixes of query (sub)tree
    wrappers::NodePrefixesPair<_Node> query_prefixes(*query_subroot);
    query_prefixes.create_subtrees_prefixes();

    for ( std::vector<int>::const_iterator d_kr_cit = std::next(document_kr.cbegin());
          d_kr_cit != document_kr.cend(); ++d_kr_cit)
    {
      // *document_postorder->at(*d_kr_cit - 1) ... relevant subtree Tn of T (n asc)
      pd[0][0] = 0;

      // get (sub)root of current relevant document subtree (Tn)
      document_subroot = document_postorder->at(*d_kr_cit - 1);
      // compute prefixes of document (sub)tree
      wrappers::NodePrefixesPair<_Node> document_prefixes(*document_subroot);
      document_prefixes.create_subtrees_prefixes();

      j = 1;
      for (auto& document_node: *document_prefixes.get_subtree(*document_subroot))
      {
        if (plus && ranking.full() &&
            (document_prefixes.get_prefix(*document_node)->size() > (ranking.front().get_distance() + query.get_subtree_size())))
        {
          break;
        }

        pd[0][j] = pd[0][j - 1] +
          node_alignment_costs(&nodes::empty_string_node, document_node, costs);

        i = 1;
        for (auto& query_node: *query_prefixes.get_subtree(*query_subroot)) {
          pd[i][0] = pd[i - 1][0] +
            node_alignment_costs(query_node, &nodes::empty_string_node, costs);

          // comparison of postorder vectors
          if ((*query_prefixes.get_subtree(*query_node) == *(query_prefixes.get_prefix(*query_node))) &&
              (*document_prefixes.get_subtree(*document_node) == *(document_prefixes.get_prefix(*document_node))))
          {
            pd[i][j] = std::min<unsigned int>(
              { pd[i - 1][j] +
                  node_alignment_costs(query_node, &nodes::empty_string_node, costs),
                pd[i][j - 1] +
                  node_alignment_costs(&nodes::empty_string_node, document_node, costs),
                pd[i - 1][j - 1] +
                  node_alignment_costs(query_node, document_node, costs)
              }
            );
            td[query_prefixes.get_subtree(*query_node)->back()->get_id() - 1][document_prefixes.get_subtree(*document_node)->back()->get_id() - 1] = pd[i][j];
          } else {
            pd[i][j] = std::min<unsigned int>(
              { pd[i - 1][j] +
                  node_alignment_costs(query_node, &nodes::empty_string_node, costs),
                pd[i][j - 1] +
                  node_alignment_costs(&nodes::empty_string_node, document_node, costs),
                static_cast<unsigned int>(
                  pd[i - query_prefixes.get_subtree(*query_node)->size()][j - document_prefixes.get_subtree(*document_node)->size()] +
                    td[query_prefixes.get_subtree(*query_node)->back()->get_id() - 1][document_prefixes.get_subtree(*document_node)->back()->get_id() - 1]
                )
              }
            );
          }

          ++i;
        }

        if ((*query_subroot == query) &&
            (*document_prefixes.get_subtree(*document_node) == *(document_prefixes.get_prefix(*document_node))))
        {
          wrappers::NodeDistancePair<_Node> tmp =
            wrappers::NodeDistancePair<_Node>(
              *document_prefixes.get_subtree(*document_node)->back(),
              td[query_subroot->get_id() - 1][document_prefixes.get_subtree(*document_node)->back()->get_id() - 1]
            );
          if (ranking.full()) {
            ranking.replace_front_if_greater(tmp);
          } else {
            ranking.insert(tmp);
          }

          // debug
          std::cout << ranking.max_size() << "-Heap: | ";
          for (auto it = ranking.begin(); it != ranking.end(); ++it) {
            std::cout << it->get_node().get_label() << ", " << it->get_distance() <<" | ";
          }
          std::cout << std::endl;
        }

        ++j;
      }
    }
  }
}

template<class _Node = nodes::Node, class _Costs = nodes::Costs<_Node>>
int compute_max_cost(_Node& root, const int k = 0) {
  _Costs costs;
  std::vector<_Node*>* postorder = common::generate_postorder<_Node>(&root, true);

  // TODO: max cannot be rename when rename is assumed to be (ins() + del())/2
  int max = std::max(costs.ins(&root), costs.del(&root));
  int max_new = 0;
  auto bound = ((k == 0) ? postorder->cend() : postorder->cbegin() + k);
  for (auto cit = postorder->cbegin(); cit != bound; ++cit) {
    if ((max_new = std::max(costs.ins(*cit), costs.del(*cit))) > max) {
      max = max_new;
    }
  }

  return max;
}

// ranking is modified during execution, therefore no return value
template<class _Node = nodes::Node, class _Costs = nodes::Costs<_Node>>
void tasm_dynamic_plus(_Node& query, _Node& document, const int& k,
  data_structures::KHeap<wrappers::NodeDistancePair<_Node>>& ranking)
{
  return tasm_dynamic<_Node, _Costs>(query, document, k, ranking, true);
}

template<class _Node = nodes::Node, class _Costs = nodes::Costs<_Node>>
data_structures::KHeap<wrappers::NodeDistancePair<_Node>> tasm_postorder(
  _Node& query, std::queue<_Node>& postorder_queue, const int& k)
{
  data_structures::KHeap<wrappers::NodeDistancePair<_Node>> ranking(k);

  int tau = query.get_subtree_size() *
    (compute_max_cost<_Node, _Costs>(query) + 1) +
    k * compute_max_cost<_Node, _Costs>(postorder_queue.back(), k);
  int tau_adapted = tau;

  RingBuffer<_Node> ring_buffer(k);
  RingBuffer<size_t> prefix_array(k);
  size_t start = 1;
  size_t end = 1;
  size_t appended = 0;
  size_t buffer_size = ring_buffer.get_size();
  size_t candidate_subroot;
  _Node subtree;

  prb_next(ring_buffer, prefix_array, start, end, appended, postorder_queue, tau);
  while (start != end) {
    candidate_subroot = prefix_array[start];
    while (candidate_subroot >= prefix_array[prefix_array[start] % buffer_size])
    {
      subtree = ring_buffer[prefix_array[start] % buffer_size];
      
      if (ranking.full()) {
        tau_adapted = std::min<int>(
          tau, ranking.front().get_distance() + query.get_subtree_size()
        );
      }

      if (!ranking.full() || (subtree.get_subtree_size() < tau_adapted)) {
        tasm_dynamic_plus<_Node, _Costs>(query, subtree, k, ranking);
        candidate_subroot = candidate_subroot - subtree.get_subtree_size();
      } else {
        ++candidate_subroot;
      }
    }

    start = (prefix_array[start] + 1) % buffer_size;
    prb_next(ring_buffer, prefix_array, start, end, appended, postorder_queue, tau);
  }

  return ranking;
}

} // namespace tasm

#endif // TASM_TASM_H