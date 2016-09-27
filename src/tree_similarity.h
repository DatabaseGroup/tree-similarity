#ifndef TREE_SIMILARITY_H
#define TREE_SIMILARITY_H

#include "common/common.h"

#include "visualisation/visualisation.h"

#include "data_structures/array_2d.h"
#include "data_structures/btree.h"
#include "data_structures/dewey_identifier.h"
#include "data_structures/k_heap.h"
#include "data_structures/posting_list_container.h"

#include "nodes/custom_node_data.h"
#include "nodes/node.h"
#include "nodes/string_node_data.h"
#include "nodes/hybrid_node.h"

#include "parser/parser.h"

#include "rted/rted.h"

#include "sed/string_edit_distance.h"

#include "structure_search/structure_search.h"

#include "tasm/tasm.h"

#include "upper_bound/upper_bound.h"

#include "wrappers/node_distance_pair.h"
#include "wrappers/node_index_value.h"
#include "wrappers/node_prefixes_pair.h"

#include "zhang_shasha/zhang_shasha.h"

#include <chrono>
#include <map>
template<class _T, size_t _M>
void test_btree(std::mt19937& mt) {
  data_structures::BTree<_T, _T, _M> btree;
  std::map<_T, _T> log_map;

  std::uniform_int_distribution<int> dist(1, 100000000);
  int random_number = 0;
  std::vector<int> random_numbers;

  while (random_numbers.size() < 50000) {
    random_number = dist(mt);

    if (std::find(random_numbers.cbegin(), random_numbers.cend(), random_number) == random_numbers.cend()) {
      random_numbers.push_back(random_number);
    }
  }

  std::cout << "Test run: " << random_numbers.size() << " random insertions"
    << std::endl;

  std::chrono::high_resolution_clock::time_point t1 =
    std::chrono::high_resolution_clock::now();

  for (int& random_number: random_numbers) {
    log_map.insert(std::make_pair(random_number, random_number));
  }

  std::chrono::high_resolution_clock::time_point t2 =
    std::chrono::high_resolution_clock::now();

  std::cout << "std::map: "
    << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
    << std::endl;   

  t1 = std::chrono::high_resolution_clock::now();
  
  for (int& random_number: random_numbers) {
    btree.insert(random_number, random_number);
  }

  t2 = std::chrono::high_resolution_clock::now();
 
  std::cout << "data_structures::BTree: "
    << std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count()
    << std::endl;
}

#endif // TREE_SIMILARITY_H
