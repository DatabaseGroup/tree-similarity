#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>
#include <exception>
#include <random>

#include "tree_similarity.h"

void run_zhang_shasha(char* tree_string_1, char* tree_string_2);
void test_tasm();
void test_ss_naive();

// TODO: tobi: put everything in a method e.g. called get_sbs_fs(node* t1, node* t2)
//
int main (int argc, char* argv[]) {
  //if(argc != 3 && argc != 4 && argc != 5 && argc != 6) {
  if (argc <= 1) {
    std::cout << "TODO print a help message" << std::endl;
    return 0;
  }
  // passing too big trees from command line could exceed the stack size and cause
  // a stack overflow because argv and argc are passed like normal function
  // parameters via the stack - get the maximum argument length under linux via
  // $ getconf ARG_MAX
  if (argv[1] == std::string("--apted")) { // option to test apted
    // variables needed for parser functions to create trees
    parser::LabelIDMap hashtable_label_to_id;
    int node_id_counter = 1;
    // root nodes of the test trees (arguments 3 & 4 in bracket notation)
    nodes::Node<nodes::StringNodeData>* test_tree1 = parser::create_tree_from_string(argv[2],
      hashtable_label_to_id, node_id_counter);
    nodes::Node<nodes::StringNodeData>* test_tree2 = parser::create_tree_from_string(argv[3],
      hashtable_label_to_id, node_id_counter);

    try {
      double rted_output = rted::compute_rted(test_tree1, test_tree2); //TODO test with costs
      std::cout << "ted: " << rted_output << std::endl;

    } catch(char const *e) { //catches manually thrwon undefined exception
      std::cerr << "Exception caught: " << e << std::endl;
    } catch(std::exception &e) { //catches all std exceptions
      std::cerr << e.what() << std::endl;
    }

    delete test_tree1;
    delete test_tree2;

    return 0; //terminate programm
  } else if (argv[1] == std::string("--zzted")) {
    run_zhang_shasha(argv[1], argv[2]);
    return 0;
  } else if (argv[1] == std::string("--tasm-test")) {
    test_tasm();
    return 0;
  } else if (argv[1] == std::string("--ss-naive-test")) {
    test_ss_naive();
    return 0;
  }

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

  delete tree1;
  delete tree2;
  delete tree1_postorder;
  delete tree2_postorder;
*/

  return 0;
}

void run_zhang_shasha(char* tree_string_1, char* tree_string_2) {
  // TODO replace hashtable with a custom node class that sup. strings as labels
  parser::LabelIDMap hashtable_label_to_id;
  common::IDLabelMap hashtable_id_to_label;
  int node_id_counter = 1;
  nodes::Node<nodes::StringNodeData>* tree1 = parser::create_tree_from_string(
    tree_string_1, hashtable_label_to_id, node_id_counter
  );
  nodes::Node<nodes::StringNodeData>* tree2 = parser::create_tree_from_string(
    tree_string_2, hashtable_label_to_id, node_id_counter
  );

  for ( auto it = hashtable_label_to_id.begin();
        it != hashtable_label_to_id.end(); ++it )
  {
    hashtable_id_to_label.emplace(it->second, it->first);
  }

  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter

  // distance between stree1 and stree2 should be 10 (using StringCosts)

  std::cout
    << "Distance (string-labeled tree, string-labeled cost model, Zhang Shasha):\t"
    << zhang_shasha::compute_zhang_shasha<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(tree1, tree2)
    << std::endl;
}

void test_tasm() {
  // TASM TEST CASES (DANIEL), DO NOT TOUCH THESE! ;)
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
}

void test_ss_naive() {
  // STRUCTURE SEARCH TEST CASES (DANIEL), DO NOT TOUCH THESE! ;)
  //
  nodes::Node<nodes::StringNodeData>* t1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("dblp"));
  nodes::Node<nodes::StringNodeData>* t2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("inproceedings"));
  nodes::Node<nodes::StringNodeData>* t3 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("inproceedings"));
  nodes::Node<nodes::StringNodeData>* t4 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("inproceedings"));
  nodes::Node<nodes::StringNodeData>* t5 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("inproceedings"));
  nodes::Node<nodes::StringNodeData>* t6 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("author"));
  nodes::Node<nodes::StringNodeData>* t7 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("author"));
  nodes::Node<nodes::StringNodeData>* t8 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("author"));
  nodes::Node<nodes::StringNodeData>* t9 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("author"));
  nodes::Node<nodes::StringNodeData>* t10 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("author"));
  nodes::Node<nodes::StringNodeData>* t11 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* t12 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* t13 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* t14 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* t15 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Hanrahn"));
  nodes::Node<nodes::StringNodeData>* t16 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Analytic database ..."));
  nodes::Node<nodes::StringNodeData>* t17 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Vahdat"));
  nodes::Node<nodes::StringNodeData>* t18 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Symbiosis in scale out ..."));
  nodes::Node<nodes::StringNodeData>* t19 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Hamilton"));
  nodes::Node<nodes::StringNodeData>* t20 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Internet scale ..."));
  nodes::Node<nodes::StringNodeData>* t21 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Meliou"));
  nodes::Node<nodes::StringNodeData>* t22 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Suciu"));
  nodes::Node<nodes::StringNodeData>* t23 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Tiresias ..."));

  nodes::Node<nodes::StringNodeData>* q1 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("inproceedings"));
  nodes::Node<nodes::StringNodeData>* q2 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("author"));
  nodes::Node<nodes::StringNodeData>* q3 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("title"));
  nodes::Node<nodes::StringNodeData>* q4 = new nodes::Node<nodes::StringNodeData>(new nodes::StringNodeData("Hanrahn"));

  t1->add_child(t2); t1->add_child(t3); t1->add_child(t4); t1->add_child(t5);
  t2->add_child(t6); t2->add_child(t11);
  t3->add_child(t7); t3->add_child(t12);
  t4->add_child(t8); t4->add_child(t13);
  t5->add_child(t9); t5->add_child(t10); t5->add_child(t14);
  t6->add_child(t15);
  t7->add_child(t17);
  t8->add_child(t19);
  t9->add_child(t21);
  t10->add_child(t22);
  t11->add_child(t16);
  t12->add_child(t18);
  t13->add_child(t20);
  t14->add_child(t23);

  q1->add_child(q2); q1->add_child(q3);
  q2->add_child(q4);

  std::vector<nodes::Node<nodes::StringNodeData>*> trees = { t1 };

  data_structures::BTree<nodes::StringNodeData, std::list<data_structures::DeweyIdentifier>> label_index;
  data_structures::BTree<data_structures::DeweyIdentifier, wrappers::NodeIndexValue<nodes::StringNodeData>> node_index;
  
  data_structures::DeweyIdentifier dblp(1);
  data_structures::DeweyIdentifier inproceedings1(dblp, 0);
  data_structures::DeweyIdentifier inproceedings2(dblp, 1);
  data_structures::DeweyIdentifier inproceedings3(dblp, 2);
  data_structures::DeweyIdentifier inproceedings4(dblp, 3);
  data_structures::DeweyIdentifier author1(inproceedings1, 0);
  data_structures::DeweyIdentifier author2(inproceedings2, 0);
  data_structures::DeweyIdentifier author3(inproceedings3, 0);
  data_structures::DeweyIdentifier author4(inproceedings4, 0);
  data_structures::DeweyIdentifier author5(inproceedings4, 1);
  data_structures::DeweyIdentifier title1(inproceedings1, 1);
  data_structures::DeweyIdentifier title2(inproceedings2, 1);
  data_structures::DeweyIdentifier title3(inproceedings3, 1);
  data_structures::DeweyIdentifier title4(inproceedings4, 2);
  data_structures::DeweyIdentifier hanrahn(author1, 0);
  data_structures::DeweyIdentifier analytic_db(title1, 0);
  data_structures::DeweyIdentifier vahdat(author2, 0);
  data_structures::DeweyIdentifier symbiosis_in_scale_out(title2, 0);
  data_structures::DeweyIdentifier hamilton(author3, 0);
  data_structures::DeweyIdentifier internet_scale(title3, 0);
  data_structures::DeweyIdentifier meliou(author4, 0);
  data_structures::DeweyIdentifier suciu(author5, 0);
  data_structures::DeweyIdentifier tiresias(title4, 0);

  label_index.insert(nodes::StringNodeData("dblp"), { dblp });
  label_index.insert(nodes::StringNodeData("inproceedings"), { inproceedings1,
    inproceedings2, inproceedings3, inproceedings4  }
  );
  label_index.insert(nodes::StringNodeData("author"), { author1, author2, author3, author4, author5 });
  label_index.insert(nodes::StringNodeData("title"), { title1, title2, title3, title4 });
  label_index.insert(nodes::StringNodeData("Hanrahn"), { hanrahn });
  label_index.insert(nodes::StringNodeData("Analytic database ..."), { analytic_db });
  label_index.insert(nodes::StringNodeData("Vahdat"), { vahdat });
  label_index.insert(nodes::StringNodeData("Symbiosis in scale out ..."), { symbiosis_in_scale_out });
  label_index.insert(nodes::StringNodeData("Hamilton"), { hamilton });
  label_index.insert(nodes::StringNodeData("Internet scale ..."), { internet_scale });
  label_index.insert(nodes::StringNodeData("Meliou"), { meliou });
  label_index.insert(nodes::StringNodeData("Suciu"), { suciu });
  label_index.insert(nodes::StringNodeData("Tiresias ..."), { tiresias });

  node_index.insert(dblp, wrappers::NodeIndexValue<nodes::StringNodeData>(23, 4, nodes::StringNodeData("dblp"), t1));
  node_index.insert(inproceedings1, wrappers::NodeIndexValue<nodes::StringNodeData>(5, 3, nodes::StringNodeData("inproceedings"), t2));
  node_index.insert(inproceedings2, wrappers::NodeIndexValue<nodes::StringNodeData>(5, 3, nodes::StringNodeData("inproceedings"), t3));
  node_index.insert(inproceedings3, wrappers::NodeIndexValue<nodes::StringNodeData>(5, 3, nodes::StringNodeData("inproceedings"), t4));
  node_index.insert(inproceedings4, wrappers::NodeIndexValue<nodes::StringNodeData>(7, 3, nodes::StringNodeData("inproceedings"), t5));
  node_index.insert(author1, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("author"), t6));
  node_index.insert(author2, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("author"), t7));
  node_index.insert(author3, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("author"), t8));
  node_index.insert(author4, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("author"), t9));
  node_index.insert(author5, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("author"), t10));
  node_index.insert(title1, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("title"), t11));
  node_index.insert(title2, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("title"), t12));
  node_index.insert(title3, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("title"), t13));
  node_index.insert(title4, wrappers::NodeIndexValue<nodes::StringNodeData>(2, 2, nodes::StringNodeData("title"), t14));
  node_index.insert(hanrahn, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("hanrahn"), t15));
  node_index.insert(analytic_db, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("analytic_db"), t16));
  node_index.insert(vahdat, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("vahdat"), t17));
  node_index.insert(symbiosis_in_scale_out, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("symbiosis_in_scale_out"), t18));
  node_index.insert(hamilton, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("hamilton"), t19));
  node_index.insert(internet_scale, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("internet_scale"), t20));
  node_index.insert(meliou, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("meliou"), t21));
  node_index.insert(suciu, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("suciu"), t22));
  node_index.insert(tiresias, wrappers::NodeIndexValue<nodes::StringNodeData>(1, 1, nodes::StringNodeData("tiresias"), t23));

  std::multiset<nodes::StringNodeData> labels_query = {
    nodes::StringNodeData("inproceedings"), nodes::StringNodeData("author"),
    nodes::StringNodeData("title"), nodes::StringNodeData("Hanrahn")
  };
  
  int m = 4;
  const int k = 6;
  
  data_structures::KHeap<wrappers::NodeDistancePair<nodes::StringNodeData>> ranking =
    structure_search::naive_search<nodes::StringNodeData>(q1, trees, m, k,
      label_index, node_index, labels_query
    );

  std::cout << "ranking: " << ranking.size() << " / " << ranking.max_size() << std::endl;
  std::cout << ranking.max_size() << "-Heap: | ";
  for (auto rit = ranking.rbegin(); rit != ranking.rend(); --rit) {
    std::cout << rit->get_node().get_data()->get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;

  std::cout << std::endl << std::endl;

  std::queue<nodes::Node<nodes::StringNodeData>> po_queue;
  po_queue.push(*t15);
  po_queue.push(*t6);
  po_queue.push(*t16);
  po_queue.push(*t11);
  po_queue.push(*t2);
  po_queue.push(*t17);
  po_queue.push(*t7);
  po_queue.push(*t18);
  po_queue.push(*t12);
  po_queue.push(*t3);
  po_queue.push(*t19);
  po_queue.push(*t8);
  po_queue.push(*t20);
  po_queue.push(*t13);
  po_queue.push(*t4);
  po_queue.push(*t21);
  po_queue.push(*t9);
  po_queue.push(*t22);
  po_queue.push(*t10);
  po_queue.push(*t23);
  po_queue.push(*t14);
  po_queue.push(*t5);
  po_queue.push(*t1);

  auto heap = tasm::tasm_postorder<nodes::StringNodeData, nodes::StringCosts<nodes::StringNodeData>>(*q1, po_queue, k);
  std::cout << "ranking: " << heap.size() << " / " << heap.max_size() << std::endl;
  std::cout << heap.max_size() << "-Heap: | ";
  for (auto rit = heap.rbegin(); rit != heap.rend(); --rit) {
    std::cout << rit->get_node().get_data()->get_label() << ", " << rit->get_distance() <<" | ";
  }
  std::cout << std::endl;
}
