#ifndef WRAPPERS_NODE_INDEX_VALUE_H
#define WRAPPERS_NODE_INDEX_VALUE_H

namespace wrappers {

template<class _NodeData>
class NodeIndexValue {
private:  
  size_t size_;
  size_t depth_;
  _NodeData data_;
  nodes::Node<_NodeData>* root_; // of subtree rooted at data_

public:
  NodeIndexValue();
  NodeIndexValue(const size_t& size, const size_t& depth, const _NodeData& data,
    nodes::Node<_NodeData>* root);
  ~NodeIndexValue();

  const size_t& size() const;
  const size_t& depth() const;
  const _NodeData& data() const;
  nodes::Node<_NodeData>* root() const;
};

template<class _NodeData>
NodeIndexValue<_NodeData>::NodeIndexValue()
  : NodeIndexValue(0, 0, _NodeData{}, nullptr)
{ }

template<class _NodeData>
NodeIndexValue<_NodeData>::NodeIndexValue(const size_t& size, const size_t& depth,
  const _NodeData& data, nodes::Node<_NodeData>* root)
  : size_(size), depth_(depth), data_(data), root_(root) { }

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

template<class _NodeData>
nodes::Node<_NodeData>* NodeIndexValue<_NodeData>::root() const {
  return root_;
}
  
} // namespace wrappers

#endif // WRAPPERS_NODE_INDEX_VALUE_H