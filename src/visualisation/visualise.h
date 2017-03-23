#ifndef VISUALISE_H
#define VISUALISE_H

#include <iostream>
#include <random>
#include <unordered_map>
#include <fstream>
#include <exception>

#include "../parser/parser.h"
#include "../nodes/node.h"
#include "../zhang_shasha/zhang_shasha.h"

// TODO remove visualisation/visualisation.h

namespace visualise {
////////////////////////////////////////////////////////////////////////////////
// DEFINES
// visualisation types
std::string _type_hybrid = std::string("--hybrid");
std::string _type_sbs_fs = std::string("--sbs_fs");

// colours - operations
char _COLOUR_MAPPED = 'm';
char _COLOUR_RENAME = 'r';
char _COLOUR_DELETE = 'd';
char _COLOUR_INSERT = 'i';
////////////////////////////////////////////////////////////////////////////////

// NodeInfo
// stores the additional information needed to visualise
template<class _NodeData = nodes::StringNodeData>
struct NodeInfo {
  // id - initally postorder
  int id;
  // level of the node
  int level;
  // colour of the node
  char colour;
  // if the node is renamed, the "output" label
  std::string label2;
  // the parents of the node, NOTE: multiple parents possible (graph)
  std::vector<nodes::Node<_NodeData>*>* parents;
  // the node to which the node is mapped to
  nodes::Node<_NodeData>* mappedNode;
  // the colour of the edges to the children
  std::vector<char>* childEdges;
};

// NodeToInfo
// used to store additional information needed to create the visualisation
// note that the key is the pointer and the value is the a struct NodeInfo
template<class _NodeData = nodes::StringNodeData>
using NodeToInfo = 
  std::unordered_map<nodes::Node<_NodeData>*, NodeInfo<_NodeData>*>;

// NodeToNode
// is the mapping: node --> node
template<class _NodeData = nodes::StringNodeData, 
  class _NodeData2 = nodes::StringNodeData>
using NodeToNode = 
  std::unordered_map<nodes::Node<_NodeData>*, nodes::Node<_NodeData2>*>;


template<class _NodeData = nodes::StringNodeData>
void fill_map_nodeInfo(nodes::Node<_NodeData>* tree, int& id_counter,
    NodeToInfo<_NodeData>* nodeToInfo)
{
    NodeInfo<_NodeData>* nodeInfo = new NodeInfo<_NodeData>();
    // since this method is called for the root node of the tree,
    // set the level to 0
    nodeInfo->level = 0;

    for(int i = 0; i < tree->get_children_number(); i++){
        //fill_map_nodeInfo(tree, tree->get_child(i), id_counter, nodeToInfo, 1);
    }

    // post order: root node: highest id / largest number
    nodeInfo->id = id_counter;
    nodeToInfo->emplace(tree, nodeInfo);
}


// visualise
// creates the visualisation for the two trees string depending on the type
// params:  type - visualisation type (string)
//          tree1 - the input tree (node)
//          tree2   - the output tree (node)
//
// returns the visualisation string (string)
template<class _NodeData = nodes::StringNodeData, 
    class _costs = nodes::Costs<_NodeData>>
std::string visualise(char* type, nodes::Node<_NodeData>* tree1,
    nodes::Node<_NodeData>* tree2)
{
    // TODO remove string "end" and
    std::string output = "end";
    NodeToInfo<_NodeData>* nodeToInfo = new NodeToInfo<_NodeData>();
    std::vector<std::array<nodes::Node<_NodeData>*, 2> > edm =
        zhang_shasha::compute_edit_mapping<_NodeData, 
        nodes::StringCosts<_NodeData>>(tree1, tree2);

    if(std::string(type) == _type_hybrid)
    {
        // create nodeToInfo
        // create_hybrid
        // get output for hybrid
    } 
    else if (std::string(type) == _type_sbs_fs)
    {
        // id counter since the initial node has no id anymore
        int id_counter = 0;
        // nodeInfo for tree 1 (input tree)
        fill_map_nodeInfo(tree1, id_counter, nodeToInfo);
        // reset the id_counter
        id_counter = 0; 
        // nodeInfo for tree 2 (output tree)
        fill_map_nodeInfo(tree2, id_counter, nodeToInfo);
        
        // get output for sbs_fs
    }

    return output;
}

// visualise_string
// creates the visualisation string depending on the two trees (string)
// params:  type - visualisation type (string)
//          str_t1 - the input tree (string)
//          str_t2 - the output tree (string)
//
// returns the visualisation string (string)
template<class _NodeData = nodes::StringNodeData, 
    class _costs = nodes::Costs<_NodeData>>
std::string visualise_string (char* type, char* str_t1, char* str_t2)
{
    // create the two trees
    // if parser is updated and outdates this, then remove the next line
    parser::LabelIDMap hashtable_label_to_id;
    int node_id = 0;
    nodes::Node<_NodeData>* tree1 = 
        parser::create_tree_from_string<_NodeData>(str_t1, 
        hashtable_label_to_id, node_id);
    nodes::Node<_NodeData>* tree2 = 
        parser::create_tree_from_string<_NodeData>(str_t2, 
        hashtable_label_to_id, node_id);
    
    std::string output;
    output = visualise(type, tree1, tree2);

    return output;
}

} // Namespace: visualise

#endif //VISUALISATION_H