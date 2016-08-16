#ifndef TASM_TASM_H
#define TASM_TASM_H

#include "ring_buffer.h"

#include <vector>
#include <queue>

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
    candidates.push_back(ring_buffer[start]);
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
      
      // debug
      //std::cout << "subtree size = " << dequeued_element.get_subtree_size() << std::endl;

      ring_buffer[end] = dequeued_element;   
      prefix_array[end] = (++appended) - dequeued_element.get_subtree_size();

      if (dequeued_element.get_subtree_size() <= threshold) {
        prefix_array[prefix_array[end] % buffer_size] = appended;
      }

      // debug
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

}

#endif // TASM_TASM_H