#include "custom_node.h"

custom_node::custom_node (
  std::vector<std::string> label = std::vector<std::string>(),
  std::vector<custom_node*> children = std::vector<custom_node*>()
) : label(label), children(children) { }

std::vector<std::string> custom_node::get_label () const {
  return label;
}

std::vector<custom_node*> custom_node::get_children () const {
  return children;
}
