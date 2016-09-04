#ifndef NODE_DISTANCE_PAIR_H
#define NODE_DISTANCE_PAIR_H

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

namespace wrappers {

// TODO: Make node_ of type _NodeData and adapt get_node/constr. accordingly

template<class _NodeData = nodes::StringNodeData>
class NodeDistancePair {
private:
  nodes::Node<_NodeData> node_;
  double distance_;

public:
  NodeDistancePair();
  NodeDistancePair(const nodes::Node<_NodeData> node, double distance);
  ~NodeDistancePair();

  double get_distance() const;
  const nodes::Node<_NodeData> get_node() const;

  bool operator<(const NodeDistancePair& other) const;
  bool operator>(const NodeDistancePair& other) const;
  bool operator==(const NodeDistancePair& other) const;
  bool operator>=(const NodeDistancePair& other) const;
  bool operator<=(const NodeDistancePair& other) const;
};

template<class _NodeData>
NodeDistancePair<_NodeData>::NodeDistancePair() { }

template<class _NodeData>
NodeDistancePair<_NodeData>::NodeDistancePair(const nodes::Node<_NodeData> node,
  double distance) : node_(node), distance_(distance) { }

template<class _NodeData>
NodeDistancePair<_NodeData>::~NodeDistancePair() { }

template<class _NodeData>
double NodeDistancePair<_NodeData>::get_distance() const {
  return distance_;
}

template<class _NodeData>
const nodes::Node<_NodeData> NodeDistancePair<_NodeData>::get_node() const {
  return node_;
}

template<class _NodeData>
bool NodeDistancePair<_NodeData>::operator<(const NodeDistancePair& other) const
{
  return (distance_ < other.get_distance());
}

template<class _NodeData>
bool NodeDistancePair<_NodeData>::operator>(const NodeDistancePair& other) const
{
  return (distance_ > other.get_distance());
}

template<class _NodeData>
bool NodeDistancePair<_NodeData>::operator==(const NodeDistancePair& other) const
{
  return (distance_ == other.get_distance());
}

template<class _NodeData>
bool NodeDistancePair<_NodeData>::operator>=(const NodeDistancePair& other) const
{
  return ((*this > other) || (*this == other));
}

template<class _NodeData>
bool NodeDistancePair<_NodeData>::operator<=(const NodeDistancePair& other) const
{
  return ((*this < other) || (*this == other));
}

} // namespace wrappers

#endif // NODE_DISTANCE_PAIR_H