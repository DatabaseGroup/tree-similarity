#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>

#include "tree_similarity.h"

// TODO: tobi: put everything in a method e.g. called get_sbs_fs(node* t1, node* t2)
//
int main (int argc, char* argv[]) {
  /*
  if(argc != 3 && argc != 4 && argc != 6) {
    std::cout << "TODO print a help message" << std::endl;
    return 0;
  }
  std::cout << argv[1] << " " << argv[2] << std::endl;
  // TODO replace hashtable with a custom node class that sup. strings as labels
  parser::LabelIDMap hashtable_label_to_id;
  common::IDLabelMap hashtable_id_to_label;
  int node_id_counter = 1;
  nodes::Node* tree1 = parser::create_tree_from_string(argv[1], hashtable_label_to_id,
    node_id_counter
  );
  nodes::Node* tree2 = parser::create_tree_from_string(argv[2], hashtable_label_to_id,
    node_id_counter
  );

  for ( auto it = hashtable_label_to_id.begin();
        it != hashtable_label_to_id.end(); ++it )
  {
    hashtable_id_to_label.emplace(it->second, it->first);
  }

  std::vector<nodes::Node*>* tree1_postorder = common::generate_postorder(tree1);
  std::vector<nodes::Node*>* tree2_postorder = common::generate_postorder(tree2);

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  
  nodes::StringNode* a1 = new nodes::StringNode("a");
  nodes::StringNode* r1 = new nodes::StringNode("r");
  nodes::StringNode* d1 = new nodes::StringNode("d");
  nodes::StringNode* e1 = new nodes::StringNode("d");
  nodes::StringNode* i1 = new nodes::StringNode("i");
  nodes::StringNode* l1 = new nodes::StringNode("l");
  nodes::StringNode* t1 = new nodes::StringNode("t");
  nodes::StringNode* k1 = new nodes::StringNode("k");
  nodes::StringNode* g1 = new nodes::StringNode("g");
  nodes::StringNode* h1 = new nodes::StringNode("h");
  
  nodes::StringNode* stree1 = a1;
  stree1->add_child(r1);
  stree1->add_child(d1);
  stree1->add_child(e1);
  e1->add_child(i1);
  e1->add_child(l1);
  e1->add_child(t1);
  t1->add_child(k1);
  t1->add_child(g1);
  t1->add_child(h1);

  nodes::StringNode* x2 = new nodes::StringNode("x");
  nodes::StringNode* stree2 = x2;

  // distance between stree1 and stree2 should be 10 (using StringCosts)
  std::cout
    << "Distance (string-labeled tree, string-labeled cost model, Zhang Shasha):\t"
    << zhang_shasha::compute_zhang_shasha<nodes::StringNode, nodes::StringCosts<nodes::StringNode>>(stree1, stree2)
    << std::endl;

  std::vector<std::array<nodes::Node*, 2> > edit_mapping =
    zs::compute_edit_mapping<nodes::Node, Costs<nodes::Node>>(tree1, tree2);
  if(argc > 3) {
    std::string output;
    //std::cout << "'" << argv[3] << "'" << std::endl;
    if(argv[3] == std::string("-hybrid")){
      std::vector<int> edm;
      nodes::Node* hybrid = common::create_hybrid_graph(tree1, tree2, edit_mapping, hashtable_id_to_label);
      output = common::get_json_hybrid_graph_tree(hybrid, hashtable_id_to_label);
      //delete hybrid;
    } else if(strcmp(argv[3],"-sbs_fs")==0){
      std::cout << "sbs" << std::endl;
      int *edit_mapping_int_array[2];
      int max_nodes = std::max(tree1->get_subtree_size(), tree2->get_subtree_size());
     
      edit_mapping_int_array[0] = new int[max_nodes + 1];
      edit_mapping_int_array[1] = new int[max_nodes + 1];
      for (int i = 0; i < 2; ++i) {
        for (int j = 0; j <= max_nodes; ++j) {
          edit_mapping_int_array[i][j] = 0;
        }
      }
      zs::get_edit_mapping_int_array<nodes::Node>(edit_mapping, edit_mapping_int_array);
      
      output = common::get_json_side_by_side(tree1, tree2, 
        hashtable_id_to_label, edit_mapping_int_array);
        for (int i = 0; i < 2; ++i){
          delete[] edit_mapping_int_array[i];
        }
    }
    std::cout << output << std::endl;
    //std::cout << "output check" << std::endl;
    if(argc>=6 && (strcmp(argv[4],"-f") == 0)){
      std::fstream out;
      std::string filename = argv[5];
      filename += ".tree";
      out.open(filename, std::fstream::in | std::fstream::out | std::fstream::trunc);
      if (out.is_open()){
        out << output << std::endl;
        std::cout << "writing to the file: " << filename << std::endl;
      }
      out.close();
      output.clear();
    }
  }
  
  delete tree1;
  delete tree2;
  delete tree1_postorder;
  delete tree2_postorder;
  */

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

  std::cout << "prb-pruning result:" << std::endl;
  for (nodes::StringNode& s: result) {
    std::cout << s.get_label() << std::endl;
  }

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNode>> ranking =
    tasm::naive<nodes::StringNode, nodes::StringCosts<nodes::StringNode>>(*query, *d22, 6);
  std::cout << "ranking: ";
  std::cout << ranking.max_size() << "-Heap: | ";
  for (auto it = ranking.begin(); it != ranking.end(); ++it) {
    std::cout << it->get_node().get_label() << ", " << it->get_distance() <<" | ";
  }
  std::cout << std::endl;

  nodes::StringNode* g3 = new nodes::StringNode("a");
  nodes::StringNode* g2 = new nodes::StringNode("c");
  nodes::StringNode* g1 = new nodes::StringNode("b");

  g3->add_child(g1);
  g3->add_child(g2);

  nodes::StringNode* h7 = new nodes::StringNode("x");
  nodes::StringNode* h6 = new nodes::StringNode("a");
  nodes::StringNode* h5 = new nodes::StringNode("c");
  nodes::StringNode* h4 = new nodes::StringNode("b");
  nodes::StringNode* h3 = new nodes::StringNode("a");
  nodes::StringNode* h2 = new nodes::StringNode("d");
  nodes::StringNode* h1 = new nodes::StringNode("b");

  h7->add_child(h3);
  h7->add_child(h6);
  h3->add_child(h1);
  h3->add_child(h2);
  h6->add_child(h4);
  h6->add_child(h5);

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNode>> heap2(2);
  std::cout << "ranking: " << heap2.size() << " / " << heap2.max_size() << std::endl;
  tasm::tasm_dynamic<nodes::StringNode, nodes::StringCosts<nodes::StringNode>>(*g3, *h7, 2, heap2, false);
  std::cout << "ranking: " << heap2.size() << " / " << heap2.max_size() << std::endl;
  std::cout << heap2.max_size() << "-Heap: | ";
  for (auto rit = heap2.rbegin(); rit != heap2.rend(); --rit) {
    std::cout << rit->get_node().get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNode>> heap6(6);
  std::cout << "ranking: " << heap6.size() << " / " << heap6.max_size() << std::endl;
  tasm::tasm_dynamic<nodes::StringNode, nodes::StringCosts<nodes::StringNode>>(*query, *d22, 6, heap6, false);
  std::cout << "ranking: " << heap6.size() << " / " << heap6.max_size() << std::endl;
  std::cout << heap6.max_size() << "-Heap: | ";
  for (auto rit = heap6.rbegin(); rit != heap6.rend(); --rit) {
    std::cout << rit->get_node().get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;

  std::vector<nodes::StringNode*>* postorder =
    common::generate_postorder<nodes::StringNode>(d22);
  // TODO: do not traverse postorder again to create postorder queue
  std::queue<nodes::StringNode> po_queue;
  for (nodes::StringNode*& item: *postorder) {
    po_queue.push(*item);
  }
  
  auto heap = tasm::tasm_postorder<nodes::StringNode, nodes::StringCosts<nodes::StringNode>>(*query, po_queue, 6);
  std::cout << "ranking: " << heap.size() << " / " << heap.max_size() << std::endl;
  std::cout << heap.max_size() << "-Heap: | ";
  for (auto rit = heap.rbegin(); rit != heap.rend(); --rit) {
    std::cout << rit->get_node().get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;

  return 0;
}
