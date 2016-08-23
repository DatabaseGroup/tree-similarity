#ifndef TASM_TASM_H
#define TASM_TASM_H

#include "ring_buffer.h"
#include "../common.h"
#include "../zhang_shasha.h"
#include "../k_heap.h"
#include "../node_distance_pair.h"

#include <vector>
#include <queue>
#include <utility>

#include <iomanip> // setw

// TODO
// -  Examine if movement of ring buffer stuff to RingBuffer class makes sense
//    and if this is the case, do it (ring buffers share start, end, ...).

namespace tasm {

template<class _node = Node, class _costs = Costs<_node>>
std::vector<_node> prb_pruning (std::queue<_node>& postorder_queue,
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
template<class _node = Node, class _costs = Costs<_node>>
void prb_next (RingBuffer<_node>& ring_buffer, RingBuffer<size_t>& prefix_array,
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
template<class _node = Node, class _costs = Costs<_node>>
KHeap<NodeDistancePair<_node>> naive (_node& query, _node& document, const int& k) {
  //std::priority_queue<std::pair<_node, double>> ranking;
  KHeap<NodeDistancePair<_node>> ranking(k);
  
  if (k == 0) {
    return ranking;
  }

  double ted = 0.0;
  std::vector<_node*>* postorder_document = common::generate_postorder(&document);

  for (_node* subtree: *postorder_document) {
    ted = zs::compute_zhang_shasha<_node, _costs>(&query, subtree);
    std::cout << "ted: " << ted << " - " << subtree->get_label() << std::endl;
    if (ranking.size() < k || ted < ranking.front().get_distance()) {
      NodeDistancePair<_node> in_ranking(*subtree, ted);
      if (!ranking.insert(in_ranking)) {
        std::cout << "Replacing front with " << in_ranking.get_node().get_label() << " : " << in_ranking.get_distance() << std::endl;
        ranking.replace_front(in_ranking);
      }
    }
  }

  return ranking;
}

}

#endif // TASM_TASM_H