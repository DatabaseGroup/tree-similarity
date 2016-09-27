#ifndef WRAPPERS_NODE_INDEX_VALUE_H
#define WRAPPERS_NODE_INDEX_VALUE_H

namespace wrappers {

template<class _NodeData>
class NodeIndexValue {
private:  
  const size_t size_;
  const size_t depth_;
  const _NodeData data_;

public:
  NodeIndexValue();
  NodeIndexValue(const size_t& size, const size_t& depth, const _NodeData& data);
  ~NodeIndexValue();

  const size_t& size() const;
  const size_t& depth() const;
  const _NodeData& data() const;
};

template<class _NodeData>
NodeIndexValue<_NodeData>::NodeIndexValue() { }

template<class _NodeData>
NodeIndexValue<_NodeData>::NodeIndexValue(const size_t& size, const size_t& depth,
  const _NodeData& data) : size_(size), depth_(depth), data_(data) { }

template<class _NodeData>
NodeIndexValue<_NodeData>::~NodeIndexValue() { }

template<class _NodeData>
const size_t& NodeIndexValue<_NodeData>::size() const {
  return size_;
}

template<class _NodeData>
const size_t& NodeIndexValue<_NodeData>::depth() const {
  return depth_;
}

template<class _NodeData>
const _NodeData& NodeIndexValue<_NodeData>::data() const {
  return data_;
}
  
} // namespace wrappers

#endif // WRAPPERS_NODE_INDEX_VALUE_H