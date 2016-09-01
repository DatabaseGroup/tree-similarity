#ifndef K_HEAP_H
#define K_HEAP_H

namespace data_structures {

/**
 * Encapsulates a maximum heap of a given type (generic) which only keeps the k
 * smallest elements (wrt. the operator< of the specified type). This class is
 * useful for a top-k ranking, e.g. in the TASM implementation.
 */
template<class Type>
class KHeap {
private:
  // maximum and current size of the heap
  size_t max_size_;
  size_t size_;
  // array to store the elements of the heap
  Type* data_;

  // Shifts the element at the given position up to fulfill the heap condition.
  //
  // Params:  start_position  The position to start from
  void shift_up(const size_t& start_position);

  // Shifts the element at the given position down to fulfill the heap condition.
  //
  // Params:  start_position  The position to start from
  void shift_down(const size_t& start_position);

public:
  // Constructor(s)
  KHeap(const int& max_size = 10);
  // Destructor
  ~KHeap();

  // Check if the heap is currently empty.
  //
  // Return:  True if the heap is empty, false otherwise
  const bool empty() const;

  // Check if the heap is currently full.
  //
  // Return:  True if the heap is full, false otherwise
  const bool full() const;

  // Get the current number of elements in the heap.
  //
  // Return:  The number of elements in the heap
  const size_t size() const;

  // Get the maximum number of elements the heap can hold.
  //
  // Return:  The maximum number of elements the heap can store
  const size_t max_size() const;

  // Get the top element of the heap (the root).
  //
  // Return:  A constant reference to the top element of the heap.
  Type const& front() const;

  // Get the "last" element of the heap (the last element of the array).
  //
  // Return:  A constant referenceto the last element of the heap array.
  Type const& back() const;

  // Insert a new element into the heap.
  //
  // Params:  element The element to be inserted
  //
  // Return:  True if the element was successfully inserted, false otherwise
  bool insert(Type element);

  // Remove the top element of the heap while maintaining the heap state.
  //
  // Return:  A constant reference to the former top element of the heap
  const Type& erase_front();

  // Replace the top element of the heap by another element while maintaining the
  // heap state.
  //
  // Params:  replacement The element to replace the top element of the heap
  //
  // Return:  A constant reference to the former top element of the heap
  const Type replace_front(Type replacement);

  // Replace the top element of the heap by another element only if the
  // replacement element is acutally greater than the current top element.
  //
  // Params:  replacement The element to replace the top element of the heap
  //
  // Return:  A constant reference to the former top element of the heap if it
  //          was replaced, the current top element otherwise
  const Type replace_front_if_greater(Type replacement);

  // STL-typical iterators for range-based loops
  Type* begin() const;
  Type* end() const;
  // please notice: by now, the reverse iterator has to be DECREMENTED in the
  // loop (not incremeneted like in the STL rbegin()/rend())
  Type* rbegin() const;
  Type* rend() const;
};

template<class Type>
KHeap<Type>::KHeap(const int& max_size) : max_size_(max_size) {
  data_ = new Type[max_size_] { };
  size_ = 0;
}

template<class Type>
KHeap<Type>::~KHeap() {
  delete[] data_;
}

template<class Type>
const bool KHeap<Type>::empty() const {
  return (size_ == 0);
}

template<class Type>
const bool KHeap<Type>::full() const {
  return (size_ == max_size_);
}

template<class Type>
const size_t KHeap<Type>::size() const {
  return size_;
}

template<class Type>
const size_t KHeap<Type>::max_size() const {
  return max_size_;
}

template<class Type>
Type const& KHeap<Type>::front() const {
  return data_[0];
}

template<class Type>
Type const& KHeap<Type>::back() const {
  return data_[size_];
}

template<class Type>
bool KHeap<Type>::insert(Type element) {
  if (size_ == max_size_) {
    return false;
  }

  data_[size_] = element;
  shift_up (size_);
  ++size_;
  return true;
}

template<class Type>
const Type& KHeap<Type>::erase_front() {
  Type& root = data_[0];

  data_[0] = data_[--size_];
  shift_down(0);

  return root;
}

template<class Type>
const Type KHeap<Type>::replace_front(Type replacement) {
  Type root = data_[0];
  data_[0] = replacement;
  
  if (replacement < root) {
    shift_down(0);
  }
  
  return root;
}

template<class Type>
const Type KHeap<Type>::replace_front_if_greater(Type replacement) {
  Type& root = data_[0];
  if (root < replacement) {
    return root;
  }

  return replace_front(replacement);
}

template<class Type>
Type* KHeap<Type>::begin() const {
  return data_;
}

template<class Type>
Type* KHeap<Type>::end() const {
  return &(data_[size_]); // data_[max_size_] for the complete array
}

template<class Type>
Type* KHeap<Type>::rbegin() const {
  return &(data_[size_ - 1]); // data[max_size_ - 1] for the complete array
}

template<class Type>
Type* KHeap<Type>::rend() const {
  return (data_ - 1);
}

template<class Type>
void KHeap<Type>::shift_up(const size_t& start_position) {
  const Type back = this->back();
  size_t parent_position = ((start_position - 1) / 2);

  size_t current_position = start_position;
  while (current_position > 0 && data_[parent_position] < back) {
    data_[current_position] = data_[parent_position];
    current_position = parent_position;
    parent_position = ((parent_position - 1) / 2);
  }

  data_[current_position] = back;
}

template<class Type>
void KHeap<Type>::shift_down(const size_t& start_position) {
  const Type front = this->front();
  size_t left_position = 0, right_position = 0, swap_position = 0;

  size_t current_position = start_position;
  while (current_position < (size_ / 2)) {
    left_position = 2 * current_position + 1;
    right_position = left_position + 1;

    if (right_position < size_ && data_[left_position] < data_[right_position]) {
      swap_position = right_position;
    } else {
      swap_position = left_position;
    }

    if (front >= data_[swap_position]) {
      break;
    }

    data_[current_position] = data_[swap_position];
    current_position = swap_position;
  }
  data_[current_position] = front;
}

} // namespace data_structures

#endif // K_HEAP_H