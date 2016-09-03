#ifndef NODE_DISTANCE_PAIR_H
#define NODE_DISTANCE_PAIR_H

#include "../nodes/node.h"

namespace wrappers {

// TODO: Make node_ of type _Node and adapt get_node/constr. accordingly

template<class _Node = nodes::StringNode>
class NodeDistancePair {
private:
  nodes::Node<_Node> node_;
  double distance_;

public:
  NodeDistancePair();
  NodeDistancePair(const nodes::Node<_Node> node, double distance);
  ~NodeDistancePair();

  double get_distance() const;
  const nodes::Node<_Node> get_node() const;

  bool operator<(const NodeDistancePair& other) const;
  bool operator>(const NodeDistancePair& other) const;
  bool operator==(const NodeDistancePair& other) const;
  bool operator>=(const NodeDistancePair& other) const;
  bool operator<=(const NodeDistancePair& other) const;
};

template<class _Node>
NodeDistancePair<_Node>::NodeDistancePair() { }

template<class _Node>
NodeDistancePair<_Node>::NodeDistancePair(const nodes::Node<_Node> node,
  double distance) : node_(node), distance_(distance) { }

template<class _Node>
NodeDistancePair<_Node>::~NodeDistancePair() { }

template<class _Node>
double NodeDistancePair<_Node>::get_distance() const {
  return distance_;
}

template<class _Node>
const nodes::Node<_Node> NodeDistancePair<_Node>::get_node() const {
  return node_;
}

template<class _Node>
bool NodeDistancePair<_Node>::operator<(const NodeDistancePair& other) const
{
  return (distance_ < other.get_distance());
}

template<class _Node>
bool NodeDistancePair<_Node>::operator>(const NodeDistancePair& other) const
{
  return (distance_ > other.get_distance());
}

template<class _Node>
bool NodeDistancePair<_Node>::operator==(const NodeDistancePair& other) const
{
  return (distance_ == other.get_distance());
}

template<class _Node>
bool NodeDistancePair<_Node>::operator>=(const NodeDistancePair& other) const
{
  return ((*this > other) || (*this == other));
}

template<class _Node>
bool NodeDistancePair<_Node>::operator<=(const NodeDistancePair& other) const
{
  return ((*this < other) || (*this == other));
}

} // namespace wrappers

#endif // NODE_DISTANCE_PAIR_H