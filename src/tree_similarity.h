#ifndef TREE_SIMILARITY_H
#define TREE_SIMILARITY_H

#include "common/common.h"

#include "visualisation/visualisation.h"

#include "data_structures/k_heap.h"
#include "data_structures/array_2d.h"
#include "data_structures/btree.h"

#include "nodes/custom_node_data.h"
#include "nodes/node.h"
#include "nodes/string_node_data.h"
#include "nodes/hybrid_node.h"

#include "parser/parser.h"

#include "rted/rted.h"

#include "sed/string_edit_distance.h"

#include "tasm/tasm.h"

#include "upper_bound/upper_bound.h"

#include "wrappers/node_distance_pair.h"
#include "wrappers/node_prefixes_pair.h"

#include "zhang_shasha/zhang_shasha.h"

template<class _T, size_t _M>
void test_btree(std::mt19937& mt) {
  data_structures::BTree<_T, _T, _M> btree;
  //std::random_device rnd;
  std::uniform_int_distribution<int> dist(1, 100);
  int random_number = 0;
  std::vector<int> random_numbers;

  for (int i = 0; i < _M * _M * _M; ++i) {
    random_number = dist(mt);
    random_numbers.push_back(random_number);
    try {
      btree.insert(random_number, random_number);
      btree.print();
    } catch (std::exception e) {
      std::cout << ">>> " << e.what() << " when inserting " << random_number << std::endl;
    }
  }

  std::cout << "Insertion order: ";
  for (int& random_number: random_numbers) {
    std::cout << random_number << " ";
  }
  std::cout << std::endl;

  btree.print();
}

#endif // TREE_SIMILARITY_H
