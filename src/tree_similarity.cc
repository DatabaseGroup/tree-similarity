#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>

#include "node.h"
#include "string_node.h"
#include "common.h"
#include "parser.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"
#include "k_heap.h"

#include "tasm/tasm.h"

int main (int argc, char* argv[]) {
  StringNode* d1 = new StringNode("John");
  StringNode* d2 = new StringNode("auth");
  StringNode* d3 = new StringNode("X1");
  StringNode* d4 = new StringNode("title");
  StringNode* d5 = new StringNode("article");
  StringNode* d6 = new StringNode("VLDB");
  StringNode* d7 = new StringNode("conf");
  StringNode* d8 = new StringNode("Peter");
  StringNode* d9 = new StringNode("auth");
  StringNode* d10 = new StringNode("X3");
  StringNode* d11 = new StringNode("title");
  StringNode* d12 = new StringNode("article");
  StringNode* d13 = new StringNode("Mike");
  StringNode* d14 = new StringNode("auth");
  StringNode* d15 = new StringNode("X4");
  StringNode* d16 = new StringNode("title");
  StringNode* d17 = new StringNode("article");
  StringNode* d18 = new StringNode("proceedings");
  StringNode* d19 = new StringNode("X2");
  StringNode* d20 = new StringNode("title");
  StringNode* d21 = new StringNode("book");
  StringNode* d22 = new StringNode("dblp");
  
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

  StringNode* query = new StringNode("book");
  StringNode* title = new StringNode("title");
  StringNode* x2 = new StringNode("X2");

  query->add_child(title);
  title->add_child(x2);

  std::queue<StringNode> postorder_queue;
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

  std::vector<StringNode> result = tasm::prb_pruning<StringNode>(postorder_queue, 6);

  std::cout << "prb-pruning result:" << std::endl;
  for (StringNode& s: result) {
    std::cout << s.get_label() << std::endl;
  }

  KHeap<NodeDistancePair<StringNode>> ranking =
    tasm::naive<StringNode, StringCosts<StringNode>>(*query, *d22, 6);
  std::cout << "ranking:" << std::endl;
  ranking.print_array();

  while (!ranking.empty()) {
    const NodeDistancePair<StringNode>& p = ranking.front();
    ranking.erase_front();
    std::cout << "[" << p.get_node().get_label() << ", " << p.get_distance() << "]" << std::endl;
  }

  /*
  KHeap<int> maxheap(6);

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
  */

	return 0;
}
