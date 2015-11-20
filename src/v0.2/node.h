#ifndef NODE_H
#define NODE_H

#include <vector>
#include <string>

class node {
private:
  std::string label;
  std::vector<node>* children;

public:
  node( std::string label = std::string(),
        std::vector<node>* children = new std::vector<node>())
    : label(label), children(children)
  {  }
  
  std::string get_label() const { return label; }
  std::vector<node>* get_children() const { return children; }
};

template<class _node>
struct costs {
  int ren(_node n1, _node n2) { return 1; }
  int del(_node n) { return 2; }
  int ins(_node n) { return 3; }
};

/***********/

class custom_node {
private:
  std::vector<std::string> label;
  std::vector<custom_node>* children;

public:
  custom_node( std::vector<std::string> label = std::vector<std::string>(),
        std::vector<custom_node>* children = new std::vector<custom_node>())
    : label(label), children(children)
  { }

  std::vector<std::string> get_label() const { return label; }
  std::vector<custom_node>* get_children() const { return children; }
};

template<class _node>
struct custom_costs {
  int ren(_node n1, _node n2) { return 11; }
  int del(_node n) { return 12; }
  int ins(_node n) { return 13; }
};

#endif // NODE_H
