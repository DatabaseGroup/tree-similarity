#ifndef NODE_DISTANCE_PAIR_H
#define NODE_DISTANCE_PAIR_H

#include "node.h"

template<class NodeType = Node>
class NodeDistancePair {
private:
  NodeType node_;
  double distance_;

public:
  NodeDistancePair ();
  NodeDistancePair (const NodeType node, double distance);
  ~NodeDistancePair ();

  double get_distance () const;
  const NodeType get_node () const;

  bool operator< (const NodeDistancePair& other) const;
  bool operator> (const NodeDistancePair& other) const;
  bool operator== (const NodeDistancePair& other) const;
  bool operator>= (const NodeDistancePair& other) const;
  bool operator<= (const NodeDistancePair& other) const;
};

template<class NodeType>
NodeDistancePair<NodeType>::NodeDistancePair () { }

template<class NodeType>
NodeDistancePair<NodeType>::NodeDistancePair (const NodeType node,
  double distance) : node_(node), distance_(distance) { }

template<class NodeType>
NodeDistancePair<NodeType>::~NodeDistancePair () { }

template<class NodeType>
double NodeDistancePair<NodeType>::get_distance () const {
  return distance_;
}

template<class NodeType>
const NodeType NodeDistancePair<NodeType>::get_node () const {
  return node_;
}

template<class NodeType>
bool NodeDistancePair<NodeType>::operator< (const NodeDistancePair& other) const
{
  return (distance_ < other.get_distance());
}

template<class NodeType>
bool NodeDistancePair<NodeType>::operator> (const NodeDistancePair& other) const
{
  return (distance_ > other.get_distance());
}

template<class NodeType>
bool NodeDistancePair<NodeType>::operator== (const NodeDistancePair& other) const
{
  return (distance_ == other.get_distance());
}

template<class NodeType>
bool NodeDistancePair<NodeType>::operator>= (const NodeDistancePair& other) const
{
  return ((*this > other) || (*this == other));
}

template<class NodeType>
bool NodeDistancePair<NodeType>::operator<= (const NodeDistancePair& other) const
{
  return ((*this < other) || (*this == other));
}

#endif // NODE_DISTANCE_PAIR_H