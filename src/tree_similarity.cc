#include <iostream>
#include <random>

#include "node.h"
#include "upper_bound.h"
#include "string_edit_distance.h"
#include "zhang_shasha.h"

int main () {
	// create test-trees for simplyUpperBound
  node* t1 = new node(1, 5);
  node* middle_child = new node(3);

  t1->add_child(new node(1));
	middle_child->add_child(new node(2));
	t1->add_child(middle_child);
	t1->add_child(new node(4));

	std::cout << "size tree 1: " << t1->get_subtree_size() << std::endl;

  node* t2 = new node(13, 5);
	node* left_child = new node(4);

  left_child->add_child(new node(1));
  left_child->add_child(new node(2));
  left_child->add_child(new node(3));
  t2->add_child(left_child);

  upper_bound* ub = new upper_bound();
  double sup = ub->compute_simple_upper_bound(t1, t2);
  std::cout << "UpperBound t1->t2: " << sup << std::endl;

  delete ub;

//String-Edit-Distance
  int sed_result;
  //Creaet Line-Trees (Strings)
  node* string1 = new node(3, 13);
  node* char2 = new node(15);
  node* char3 = new node(14);
  char3->add_child(new node(4));
  char2->add_child(char3);
  string1->add_child(char2);

  node* string2 = new node(4, 13);
  node* char22 = new node(15);
  node* char33 = new node(15);
  char33->add_child(new node(14));
  char22->add_child(char33);
  string2->add_child(char22);

  //Compute SED (
  sed_result =
    sed::compute_string_edit_distance<node, costs<node>>(string1, string2);
  std::cout << "String Edit Distance: " << sed_result << std::endl;

  delete string1;
  delete string2;
  // Zhang and Shasha cost = 1 (insert), 1 (delete), 1 (rename)
  // compute distance using basic nodes and basic cost model
  // no need to generate basic cost model since it is set as default template
  // parameter
  std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha<node, costs2<node>>(t1, t2)
    << std::endl;

    delete t1;
    delete t2;


    //TEST of Zhang & Shasha
    node* tree1 = new node(100);
    generate_full_tree(tree1, 4, 4);

    //deep copy
    node* tree2 = new node(100);
    copy_tree(tree1, tree2);

    std::vector<node*>* post_t1 = generate_postorder(tree1);
    std::vector<node*>* post_t2 = generate_postorder(tree2);

    std::default_random_engine generator;
    std::uniform_int_distribution<int> distribution(0,post_t1->size()-1);

    int zufall = 0;
    int tree_size = tree1->get_subtree_size();
    for(int i = 0; i < tree_size; i++){
  //  std::cout << "Schleifendurchlauf: " << i << std::endl;
        //zufall = rand() % post_t2.size(); //Random-Number betwenn 0 and number of all nodes
        std::cout << "post_t1" <<  std::endl;
        for(unsigned int i = 0; i < post_t1->size(); i++){
            std:: cout << post_t1->at(i)->get_label_id() << ", ";
        }
        std::cout << std::endl;

         std::cout << "post_t2" <<  std::endl;
        for(unsigned int i = 0; i < post_t2->size(); i++){
            std:: cout << post_t2->at(i)->get_label_id() << ", ";
        }
        std::cout << std::endl;

        //std::cout << "zufall = " << zufall << std::endl;
        while(post_t1->at(zufall)->get_label_id() == 500){
          //  std::cout << "in while" << std::endl;
            //std::cout << "label id" << post_t2->at(zufall)->get_label_id() << std::endl;
            zufall = distribution(generator);
        }
        //std::cout << "nach while (vor set label-id)" << std::endl;
        //set node to -1
        post_t1->at(zufall)->set_label_id(500);
        //std::cout <<"nach set_label_id; label = " << post_t2->at(zufall)->get_label_id() << std::endl;


        std::cout << "Distance (basic tree, basic cost model, Zhang Shasha): " << "\t"
    << zs::compute_zhang_shasha(tree1, tree2)
    << std::endl;

    }

    delete tree1;
    delete tree2;


	return 0;
}
