#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>
#include <exception>

#include "tree_similarity.h"

// TODO: tobi: put everything in a method e.g. called get_sbs_fs(node* t1, node* t2)
//
int main (int argc, char* argv[]) {
  if(argc != 3 && argc != 4 && argc != 5 && argc != 6) {
    std::cout << "TODO print a help message" << std::endl;
    return 0;
  }
  if(argv[1] == std::string("--apted")){ // option to test apted
    // variables needed for parser functions to create trees
    parser::LabelIDMap hashtable_label_to_id;
    int node_id_counter = 1;

    // root nodes of the test trees (arguments 4 & 5 in bracket notation)
    nodes::Node<nodes::StringNodeData>* test_tree1 = parser::create_tree_from_string(argv[3],
      hashtable_label_to_id, node_id_counter);
    nodes::Node<nodes::StringNodeData>* test_tree2 = parser::create_tree_from_string(argv[4],
      hashtable_label_to_id, node_id_counter);

    if(argv[2] == std::string("size")){ //Test rted::get_tree_size function
      try {
        std::cout << rted::get_tree_size(test_tree1) << ";" << rted::get_tree_size(test_tree2) << std::endl;
      } catch(char const *e) { //catches manually thrwon undefined exception
        std::cerr << "Exception caught: " << e << std::endl;
      } catch(std::exception &e) { //catches all std exceptions
        std::cout << e.what() << std::endl;
      }

    } else if(argv[2] == std::string("preorder")){ //Test of rted::gather_tree_info
      try {
        int tree1_size = rted::get_tree_size(test_tree1);
        rted::NodeInfo<nodes::StringNodeData>* tree_info_array_preorder[tree1_size]; //array of NodeInfo structs FIXME on the stack - allocate in heap
        for(int i = 0; i < tree1_size; ++i) { // allocation of memory for every struct in array
          tree_info_array_preorder[0] = (rted::NodeInfo<nodes::StringNodeData>*) malloc(sizeof(rted::NodeInfo<nodes::StringNodeData>*));
        }
        nodes::Node<nodes::StringNodeData>* tree_array_preorder[tree1_size]; //FIXME on the stack - allocate in heap

        //gathering info - storing in params tree_array_preorder, tree_info_array_preorder
        rted::gather_tree_info(test_tree1, tree_array_preorder, tree_info_array_preorder);

        //printing
        std::cout << "\nposition:label" << std::endl;
        for(int i = 0; i < tree1_size; i++) { //printing preordered nodes array in form position:label
          std::cout <<  i << ":" << tree_array_preorder[i]->get_data()->get_label() << "|";
        }
        std::cout << std::endl;

        std::cout << "\nposition:subtree_size" << std::endl;
        for(int i = 0; i < tree1_size; i++) { //printing tree info array in form position:subtree_size
          std::cout <<  i << ":" << tree_info_array_preorder[i]->subtree_size << "|";
        }
        std::cout << std::endl;

        std::cout << "\nposition:parent_id" << std::endl;
        for(int i = 0; i < tree1_size; i++) { //printing tree info array in form position:parent_id
          std::cout <<  i << ":" << tree_info_array_preorder[i]->parent_id << "|";
        }
        std::cout << std::endl;

      } catch(char const *e) { //catches manually thrwon undefined exception
        std::cerr << "Exception caught: " << e << std::endl;
      } catch(std::exception &e) { //catches all std exceptions
        std::cout << e.what() << std::endl;
      }
    }
    std::cout << "\nbefore return 0" << std::endl;
    return 0; //terminate programm
  }

  std::cout << argv[1] << " " << argv[2] << std::endl;
  // TODO replace hashtable with a custom node class that sup. strings as labels
  parser::LabelIDMap hashtable_label_to_id;
  common::IDLabelMap hashtable_id_to_label;
  int node_id_counter = 1;
  nodes::Node<nodes::StringNodeData>* tree1 = parser::create_tree_from_string(argv[1], hashtable_label_to_id,
    node_id_counter
  );
  nodes::Node<nodes::StringNodeData>* tree2 = parser::create_tree_from_string(argv[2], hashtable_label_to_id,
    node_id_counter
  );

  for ( auto it = hashtable_label_to_id.begin();
        it != hashtable_label_to_id.end(); ++it )
  {
    hashtable_id_to_label.emplace(it->second, it->first);
  }

  std::vector<nodes::Node<nodes::StringNodeData>*>* tree1_postorder = common::generate_postorder(tree1);
  std::vector<nodes::Node<nodes::StringNodeData>*>* tree2_postorder = common::generate_postorder(tree2);

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter

  /*
  nodes::Node<nodes::StringNodeData>* a1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("a"));
  nodes::Node<nodes::StringNodeData>* r1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("r"));
  nodes::Node<nodes::StringNodeData>* d1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("d"));
  nodes::Node<nodes::StringNodeData>* e1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("d"));
  nodes::Node<nodes::StringNodeData>* i1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("i"));
  nodes::Node<nodes::StringNodeData>* l1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("l"));
  nodes::Node<nodes::StringNodeData>* t1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("t"));
  nodes::Node<nodes::StringNodeData>* k1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("k"));
  nodes::Node<nodes::StringNodeData>* g1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("g"));
  nodes::Node<nodes::StringNodeData>* h1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("h"));

  nodes::Node<nodes::StringNodeData>* stree1 = a1;
  stree1->add_child(r1);
  stree1->add_child(d1);
  stree1->add_child(e1);
  e1->add_child(i1);
  e1->add_child(l1);
  e1->add_child(t1);
  t1->add_child(k1);
  t1->add_child(g1);
  t1->add_child(h1);

  nodes::Node<nodes::StringNodeData>* x2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("x"));
  nodes::Node<nodes::StringNodeData>* stree2 = x2;

  // distance between stree1 and stree2 should be 10 (using StringCosts)
  */

  std::cout
    << "Distance (string-labeled tree, string-labeled cost model, Zhang Shasha):\t"
    << zhang_shasha::compute_zhang_shasha<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(tree1, tree2)
    << std::endl;

  std::cout
    << "Distance (string-labeled tree, string-labeled cost model, RTED):\t"
    << rted::compute_rted<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(tree1, tree2)
    << std::endl;
/*
  // TOBIAS PART - TO BE RESOLVED/REMOVE/CLEANED BY TOBIAS
  //
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
*/

  delete tree1;
  delete tree2;
  delete tree1_postorder;
  delete tree2_postorder;

/*
  // TEST CASES OF DANIEL, DO NOT TOUCH THESE! ;)
  //
  nodes::Node<nodes::StringNodeData>* d1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("John"));
  nodes::Node<nodes::StringNodeData>* d2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("auth"));
  nodes::Node<nodes::StringNodeData>* d3 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("X1"));
  nodes::Node<nodes::StringNodeData>* d4 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* d5 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("article"));
  nodes::Node<nodes::StringNodeData>* d6 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("VLDB"));
  nodes::Node<nodes::StringNodeData>* d7 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("conf"));
  nodes::Node<nodes::StringNodeData>* d8 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Peter"));
  nodes::Node<nodes::StringNodeData>* d9 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("auth"));
  nodes::Node<nodes::StringNodeData>* d10 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("X3"));
  nodes::Node<nodes::StringNodeData>* d11 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* d12 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("article"));
  nodes::Node<nodes::StringNodeData>* d13 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Mike"));
  nodes::Node<nodes::StringNodeData>* d14 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("auth"));
  nodes::Node<nodes::StringNodeData>* d15 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("X4"));
  nodes::Node<nodes::StringNodeData>* d16 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* d17 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("article"));
  nodes::Node<nodes::StringNodeData>* d18 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("proceedings"));
  nodes::Node<nodes::StringNodeData>* d19 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("X2"));
  nodes::Node<nodes::StringNodeData>* d20 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* d21 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("book"));
  nodes::Node<nodes::StringNodeData>* d22 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("dblp"));

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

  nodes::Node<nodes::StringNodeData>* query = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("book"));
  nodes::Node<nodes::StringNodeData>* title = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* x2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("X2"));

  query->add_child(title);
  title->add_child(x2);

  std::queue<nodes::Node<nodes::StringNodeData>> postorder_queue;
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


  std::vector<nodes::Node<nodes::StringNodeData>> result = tasm::prb_pruning<nodes::StringNodeData>(postorder_queue, 6);

  std::cout << "prb-pruning result:" << std::endl;
  for (nodes::Node<nodes::StringNodeData>& s: result) {
    std::cout << s.get_data()->get_label() << std::endl;
  }

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNodeData>> ranking =
    tasm::naive<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(*query, *d22, 6);
  std::cout << "ranking: ";
  std::cout << ranking.max_size() << "-Heap: | ";
  for (auto it = ranking.begin(); it != ranking.end(); ++it) {
    std::cout << it->get_node().get_data()->get_label() << ", " << it->get_distance() <<" | ";
  }
  std::cout << std::endl;

  nodes::Node<nodes::StringNodeData>* g3 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("a"));
  nodes::Node<nodes::StringNodeData>* g2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("c"));
  nodes::Node<nodes::StringNodeData>* g1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("b"));

  g3->add_child(g1);
  g3->add_child(g2);

  nodes::Node<nodes::StringNodeData>* h7 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("x"));
  nodes::Node<nodes::StringNodeData>* h6 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("a"));
  nodes::Node<nodes::StringNodeData>* h5 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("c"));
  nodes::Node<nodes::StringNodeData>* h4 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("b"));
  nodes::Node<nodes::StringNodeData>* h3 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("a"));
  nodes::Node<nodes::StringNodeData>* h2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("d"));
  nodes::Node<nodes::StringNodeData>* h1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("b"));

  h7->add_child(h3);
  h7->add_child(h6);
  h3->add_child(h1);
  h3->add_child(h2);
  h6->add_child(h4);
  h6->add_child(h5);

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNodeData>> heap2(2);
  std::cout << "ranking: " << heap2.size() << " / " << heap2.max_size() << std::endl;
  tasm::tasm_dynamic<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(*g3, *h7, 2, heap2, false);
  std::cout << "ranking: " << heap2.size() << " / " << heap2.max_size() << std::endl;
  std::cout << heap2.max_size() << "-Heap: | ";
  for (auto rit = heap2.rbegin(); rit != heap2.rend(); --rit) {
    std::cout << rit->get_node().get_data()->get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;

  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNodeData>> heap6(6);
  std::cout << "ranking: " << heap6.size() << " / " << heap6.max_size() << std::endl;
  tasm::tasm_dynamic<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(*query, *d22, 6, heap6, false);
  std::cout << "ranking: " << heap6.size() << " / " << heap6.max_size() << std::endl;
  std::cout << heap6.max_size() << "-Heap: | ";
  for (auto rit = heap6.rbegin(); rit != heap6.rend(); --rit) {
    std::cout << rit->get_node().get_data()->get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;

  std::vector<nodes::Node<nodes::StringNodeData>*>* postorder =
    common::generate_postorder<nodes::StringNodeData>(d22);
  // TODO: do not traverse postorder again to create postorder queue
  std::queue<nodes::Node<nodes::StringNodeData>> po_queue;
  for (nodes::Node<nodes::StringNodeData>*& item: *postorder) {
    po_queue.push(*item);
  }

  auto heap = tasm::tasm_postorder<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(*query, po_queue, 6);
  std::cout << "ranking: " << heap.size() << " / " << heap.max_size() << std::endl;
  std::cout << heap.max_size() << "-Heap: | ";
  for (auto rit = heap.rbegin(); rit != heap.rend(); --rit) {
    std::cout << rit->get_node().get_data()->get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;
*/

  return 0;
}
