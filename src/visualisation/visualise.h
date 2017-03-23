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

// fill_map_nodeInfo (child nodes)
// fills the nodeToInfo nodeInfo for each node
// params:  parent - the parent of the node (node)
//          node - node of the tree (node)
//          nodeToInfo - nodeToInfo, map which values are the nodeInfo (nodeToInfo)
//          id_counter - the ids for the nodes (int)
template<class _NodeData = nodes::StringNodeData>
void fill_map_nodeInfo(nodes::Node<_NodeData>* parent,
  nodes::Node<_NodeData>* node, int& id_counter, 
  NodeToInfo<_NodeData>* nodeToInfo, int level)
{
    NodeInfo<_NodeData>* nodeInfo = new NodeInfo<_NodeData>();
    // set the level
    nodeInfo->level = level;
    // set the parent
    nodeInfo->parents = new std::vector<nodes::Node<_NodeData>*>();    
    nodeInfo->parents->push_back(parent);

    // fill the nodeInfo for each child node (rec.)
    for(int i = 0; i < node->get_children_number(); i++){
        fill_map_nodeInfo(node, node->get_child(i), id_counter, nodeToInfo,level+1);
    }

    // postorder id !
    nodeInfo->id = id_counter;
    ++id_counter;
    nodeToInfo->emplace(node, nodeInfo);
}

// fill_map_nodeInfo (root node)
// fills the nodeToInfo nodeInfo for each node
// params:  root - root node of the tree (node)
//          nodeToInfo - nodeToInfo, map which values are the nodeInfo (nodeToInfo)
//          id_counter - the ids for the nodes (int)
template<class _NodeData = nodes::StringNodeData>
void fill_map_nodeInfo(nodes::Node<_NodeData>* root, int& id_counter,
    NodeToInfo<_NodeData>* nodeToInfo)
{
    NodeInfo<_NodeData>* nodeInfo = new NodeInfo<_NodeData>();
    // since this method is called for the root node of the tree,
    // set the level to 0
    nodeInfo->level = 0;
    // set the mappedNode to nullptr = no mapping (yet)
    nodeInfo->mappedNode = nullptr;
    // set the parents of the root node to nullptr, since it has none
    nodeInfo->parents = nullptr;
    
    // fill the nodeInfo for each child node
    for(int i = 0; i < root->get_children_number(); i++){
        fill_map_nodeInfo(root, root->get_child(i), id_counter, nodeToInfo, 1);
    }

    // post order: root node: highest id / largest number
    nodeInfo->id = id_counter;
    nodeToInfo->emplace(root, nodeInfo);
}

// fill_mappedNodes_nodeToInfo (hybrid)
// fills the nodeToInfo mapping
// params:  edm - edit mapping (vector of array of nodes)
//          nodeToInfo - nodeToInfo, where we store the mapping (nodeToInfo)
//          nodeToNode - stores the mapping of newNode - old Node (nodeToNode)
template<class _NodeData = nodes::StringNodeData>
void fill_mappedNodes_nodeToInfo(
    std::vector<std::array<nodes::Node<_NodeData>*, 2> > edm,
    NodeToInfo<_NodeData>* nodeToInfo, 
    NodeToNode<_NodeData, _NodeData>* nodeToNode)
{
    typename std::vector<std::array<nodes::Node<_NodeData>*, 2> >::iterator it;
    for(it = edm.begin(); it != edm.end(); ++it){
    std::array<nodes::Node<_NodeData>*, 2> ar = *it;
        if(ar[0] != 0 && ar[1] != 0){
            nodeToInfo->at(nodeToNode->at(ar[0]))->mappedNode = nodeToNode->at(ar[1]);
            nodeToInfo->at(nodeToNode->at(ar[1]))->mappedNode = nodeToNode->at(ar[0]);
        }
    }
}

// fill_mappedNodes_nodeToInfo
// fills the nodeToInfo mapping
// params:  edm - edit mapping (vector of array of nodes)
//          nodeToInfo - nodeToInfo, where we store the mapping (nodeToInfo)
template<class _NodeData = nodes::StringNodeData>
void fill_mappedNodes_nodeToInfo(
  std::vector<std::array<nodes::Node<_NodeData>*, 2> > edm,
  NodeToInfo<_NodeData>* nodeToInfo)
{
    typename std::vector<std::array<nodes::Node<_NodeData>*, 2> >::iterator it;
    for(it = edm.begin(); it != edm.end(); ++it){
    std::array<nodes::Node<_NodeData>*, 2> ar = *it;
        if(ar[0] != 0 && ar[1] != 0){
            nodeToInfo->at(ar[0])->mappedNode = ar[1];
            nodeToInfo->at(ar[1])->mappedNode = ar[0];
        }
    }
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
        // fill the mapped node for each node
        fill_mappedNodes_nodeToInfo(edm, nodeToInfo);
        // get output for sbs_fs
        output = get_side_by_side_indented(tree1,tree2,nodeToInfo);
        delete nodeToInfo;
    }

    return output;
}

// get_json_string_side_by_side_indented
// params:  tree - the current node we need to acquire information about
//          nodeToInfo - the map in which additional info is stored
//          t - (integer) which tree the node belongs to
//          id_offset - since we need unique ids, the offset is used
//                      so we can differentiate between nodes solely with ids
//                      --> unique ids
//
// is called by get_side_by_side_indented
// returns the json string used for visualising sbs_fs
template<class _NodeData = nodes::StringNodeData>
std::string get_json_string_side_by_side_indented(nodes::Node<_NodeData>* tree,
  NodeToInfo<_NodeData>* nodeToInfo, int t, int id_offset)
{
    std::stringstream str;
    if(! tree){ return ""; }
    str << "{\"scope\":" << nodeToInfo->at(tree)->level;
    str << ",\"label\":\"" << tree->get_data()->get_label() << "\"";
    str << ",\"id\":" << (nodeToInfo->at(tree)->id + id_offset);
    str << ",\"tree\":" << t << "";
    str << ",\"mappedTo\":";
    if(nodeToInfo->at(tree)->mappedNode){
        str << nodeToInfo->at(nodeToInfo->at(tree)->mappedNode)->id;
    } else {
        str << "0";
    }
    str << ",\"children\":";
    if(tree->get_children_number() > 0){
        str << "[";
        for(int i = 0; i < tree->get_children_number(); i++){
            str << get_json_string_side_by_side_indented(tree->get_child(i),
            nodeToInfo, t, id_offset);
            if ((i + 1) < tree->get_children_number()) {
                str << ",";
            }
        }
        str << "]";
    } else {
        str << " null ";
    }
    str << "}";

    return str.str();
}

// get_side_by_side_indented (sbs_fs)
// descr
// params:  a
//
// returns the json string for the visualisation type sbs_fs
template<class _NodeData = nodes::StringNodeData, 
  class _costs = nodes::Costs<_NodeData>>
std::string get_side_by_side_indented(nodes::Node<_NodeData>* tree1,
  nodes::Node<_NodeData>* tree2, NodeToInfo<_NodeData>* nodeToInfo)
{
    std::string res = "[";
    res += get_json_string_side_by_side_indented(tree1, nodeToInfo, 1, 0);
    res += ",";
    res += get_json_string_side_by_side_indented(tree2, nodeToInfo, 2,
            nodeToInfo->at(tree1)->id);
    res += "]";
    return res;
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