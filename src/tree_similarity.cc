#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>

#include "tree_similarity.h"

int main (int argc, char* argv[]) {
  nodes::StringNode* d1 = new nodes::StringNode("John");
  nodes::StringNode* d2 = new nodes::StringNode("auth");
  nodes::StringNode* d3 = new nodes::StringNode("X1");
  nodes::StringNode* d4 = new nodes::StringNode("title");
  nodes::StringNode* d5 = new nodes::StringNode("article");
  nodes::StringNode* d6 = new nodes::StringNode("VLDB");
  nodes::StringNode* d7 = new nodes::StringNode("conf");
  nodes::StringNode* d8 = new nodes::StringNode("Peter");
  nodes::StringNode* d9 = new nodes::StringNode("auth");
  nodes::StringNode* d10 = new nodes::StringNode("X3");
  nodes::StringNode* d11 = new nodes::StringNode("title");
  nodes::StringNode* d12 = new nodes::StringNode("article");
  nodes::StringNode* d13 = new nodes::StringNode("Mike");
  nodes::StringNode* d14 = new nodes::StringNode("auth");
  nodes::StringNode* d15 = new nodes::StringNode("X4");
  nodes::StringNode* d16 = new nodes::StringNode("title");
  nodes::StringNode* d17 = new nodes::StringNode("article");
  nodes::StringNode* d18 = new nodes::StringNode("proceedings");
  nodes::StringNode* d19 = new nodes::StringNode("X2");
  nodes::StringNode* d20 = new nodes::StringNode("title");
  nodes::StringNode* d21 = new nodes::StringNode("book");
  nodes::StringNode* d22 = new nodes::StringNode("dblp");
  
  d22->add_child(d5);
  d22->add_child(d18);
  d22->add_child(d21);
  d5->add_child(d2);
  d5->add_child(d4);
  d18->add_child(d7);
  d18->add_child(d12);
  d18->add_child(d17);
  d21->add_child(d20);
  d2->add_child(d1);
  d4->add_child(d3);
  d7->add_child(d6);
  d12->add_child(d9);
  d12->add_child(d11);
  d17->add_child(d14);
  d17->add_child(d16);
  d20->add_child(d19);
  d9->add_child(d8);
  d11->add_child(d10);
  d14->add_child(d13);
  d16->add_child(d15);

  nodes::StringNode* query = new nodes::StringNode("book");
  nodes::StringNode* title = new nodes::StringNode("title");
  nodes::StringNode* x2 = new nodes::StringNode("X2");

  query->add_child(title);
  title->add_child(x2);

  std::queue<nodes::StringNode> postorder_queue;
  postorder_queue.push(*d1);
  postorder_queue.push(*d2);
  postorder_queue.push(*d3);
  postorder_queue.push(*d4);
  postorder_queue.push(*d5);
  postorder_queue.push(*d6);
  postorder_queue.push(*d7);
  postorder_queue.push(*d8);
  postorder_queue.push(*d9);
  postorder_queue.push(*d10);
  postorder_queue.push(*d11);
  postorder_queue.push(*d12);
  postorder_queue.push(*d13);
  postorder_queue.push(*d14);
  postorder_queue.push(*d15);
  postorder_queue.push(*d16);
  postorder_queue.push(*d17);
  postorder_queue.push(*d18);
  postorder_queue.push(*d19);
  postorder_queue.push(*d20);
  postorder_queue.push(*d21);
  postorder_queue.push(*d22);

  std::vector<nodes::StringNode> result = tasm::prb_pruning<nodes::StringNode>(postorder_queue, 6);

  /*std::cout << "prb-pruning result:" << std::endl;
  for (nodes::StringNode& s: result) {
    std::cout << s.get_label() << std::endl;
  }

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNode>> ranking =
    tasm::naive<nodes::StringNode, StringCosts<StringNode>>(*query, *d22, 6);
  std::cout << "ranking:" << std::endl;
  ranking.print_array();

  while (!ranking.empty()) {
    const wrappers::NodeDistancePair<nodes::StringNode>& p = ranking.front();
    ranking.erase_front();
    std::cout << "[" << p.get_node().get_label() << ", " << p.get_distance() << "]" << std::endl;
  }*/

  data_structures::KHeap<int> maxheap(6);

  maxheap.insert(16);
  maxheap.insert(17);
  maxheap.insert(18);
  maxheap.insert(13);
  maxheap.insert(15);
  maxheap.insert(19);

  std::cout << "MAX-HEAP:" << std::endl;
  maxheap.print_array();

  maxheap.replace_front(6);

  std::cout << "MAX-HEAP:" << std::endl;
  maxheap.print_array();

	return 0;
}
