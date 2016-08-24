#ifndef TASM_RING_BUFFER_H
#define TASM_RING_BUFFER_H

namespace tasm {

template<class Type = nodes::Node>
class RingBuffer {
private:
  size_t size_;
  Type* data_;
  Type* start_;
  Type* end_;

public:
  RingBuffer(size_t threshold = 10);
  ~RingBuffer();

  const size_t get_size() const;
  const Type& pop_range(const size_t& start, const size_t& end);

  Type& operator[](size_t position);
};

template<class Type>
RingBuffer<Type>::RingBuffer(size_t threshold) : size_(threshold + 1) {
  data_ = new Type[size_];
  start_ = data_;
  end_ = start_ + threshold;
}

template<class Type>
RingBuffer<Type>::~RingBuffer() {
  delete[] data_;
}

template<class Type>
const size_t RingBuffer<Type>::get_size() const {
  return size_;
}

template<class Type>
Type& RingBuffer<Type>::operator[](size_t position) {
  return data_ [position];
}

} // namespace tasm

#endif // TASM_RING_BUFFER_H