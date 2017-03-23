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
// other
int _POSITION_NOT_FOUND = -1;
int _HIGHEST_ID_HYBRID = 0;
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

template<class _NodeData = nodes::StringNodeData>
nodes::Node<_NodeData>* copy_tree_for_hybrid (
  nodes::Node<_NodeData>* parent, nodes::Node<_NodeData>* node,
  NodeToInfo<_NodeData>* nodeToInfo, 
  NodeToNode<_NodeData, _NodeData>* nodeToNode, int& id_counter, 
  int level, char colour = char(0))
{
    NodeInfo<_NodeData>* nodeInfo = new NodeInfo<_NodeData>();

    // set the level
    nodeInfo->level = level;
    // get the label of the "original" node
    std::string label = node->get_data()->get_label();
    // check if a parent is set
    if(parent!=nullptr)
    {
        // set the parent
        nodeInfo->parents = new std::vector<nodes::Node<_NodeData>*>();
        nodeInfo->parents->push_back(parent);
    } 
    else
    {
        // set the parent to nullptr --> no parent
        nodeInfo->parents = nullptr;
    }

    // create the new node with the label of the "original" node
    nodes::Node<_NodeData>* newNode = 
        new nodes::Node<_NodeData>(new _NodeData(label));
    // set the childEdges to nullptr --> no coloured edges
    nodeInfo->childEdges = nullptr;

    // copy each child node
    for(int i = 0; i < node->get_children_number(); i++){
        nodes::Node<_NodeData>* tmp = (copy_tree_for_hybrid(newNode, 
            node->get_child(i), nodeToInfo, nodeToNode, id_counter, level+1, colour));
        newNode->add_child(tmp);
    }

    // if a colour is set, colour the node and the edges
    if(colour != char(0)) {
        nodeInfo->childEdges = new std::vector<char>();
        nodeInfo->colour = colour;
        for(int i = 0; i < newNode->get_children_number(); i++){
            nodeInfo->childEdges->push_back(colour);
        }
    }
    // postorder id
    nodeInfo->id = id_counter;
    ++id_counter;

    // set the mapping for the "old"/"original" node and the new node
    nodeToInfo->emplace(newNode, nodeInfo);
    nodeToNode->emplace(node, newNode);

    return (newNode);
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
template<class _NodeData = nodes::StringNodeData>
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

// is_copy_of_tree
// checks if the trees are identical to structure, label and colour,
//      starts at tree2's mapped node 
// params:  tree1 - the first tree (node) 
//              ! tree1 should be COLOURED COPY OF TREE2 !
//          tree2 - the second tree (node) 
//          nodeToInfo - where the additional info is stored in
//          nodeToNode - the mapping of the nodes (tree1<->tree2)
//          colour - (opt) the colour the nodes/edges are allowed to have
//          
// returns (bool) whether the trees are the same (structure + label)
template<class _NodeData = nodes::StringNodeData>
bool is_copy_of_tree(nodes::Node<_NodeData>* node_t1,
    nodes::Node<_NodeData>* node_t2, NodeToInfo<_NodeData>* nodeToInfo,
    NodeToNode<_NodeData, _NodeData>* nodeToNode, char colour = char(0))
{
    nodes::Node<_NodeData>* mappedNode = nodeToNode->at(node_t2);
    // check if the mapping is ok
    if(node_t1 != mappedNode)
        { return false; }
    
    // check label
    if(node_t1->get_data()->get_label() != node_t2->get_data()->get_label())
        {return false; }

    // check the colour
    if(nodeToInfo->at(node_t1)->colour != colour)
        { return false; }
    
    // check the number of children
    if(node_t1->get_children_number() != node_t2->get_children_number())
        { return false; }
    
    if(node_t1->get_children_number() > 0)
    {
        // size of the child edge array has to equal the children number
        if(node_t1->get_children_number() != nodeToInfo->at(node_t1)->childEdges->size())
           {return false; }
        bool result = true;
        // check edges + child nodes
        for(int i = 0; i < node_t1->get_children_number(); i++)
        {
            // check edge colour
            if(nodeToInfo->at(node_t1)->childEdges->at(i) != colour) { return false; }
            // check it for every node
            result = is_copy_of_tree(node_t1->get_child(i), node_t2->get_child(i),
                nodeToInfo, nodeToNode, colour);
            if(!result) { return false; }
        }
        
    }
    
    return true;
}

template<class _NodeData = nodes::StringNodeData>
bool same_labels(nodes::Node<_NodeData>* n1, nodes::Node<_NodeData>* n2)
{
    return n1->get_data()->get_label() == n2->get_data()->get_label();
}

template<class _NodeData = nodes::StringNodeData>
int get_child_position(nodes::Node<_NodeData>* parent,
  nodes::Node<_NodeData>* child)
{
    int position = _POSITION_NOT_FOUND;
    for(int i = 0; i < parent->get_children_number(); i++)
    {
        if(child == parent->get_child(i))
        {
            return i;
        }
    }
    return position;
}

template<class _NodeData = nodes::StringNodeData>
void colour_edge_from_to(nodes::Node<_NodeData>* parent,
    nodes::Node<_NodeData>* child, NodeToInfo<_NodeData>* nodeToInfo,
    char colour)
{
    int position = get_child_position(parent, child);
    if(position != _POSITION_NOT_FOUND)
    {
        nodeToInfo->at(parent)->childEdges->at(position) = colour;
    }
    else
    {
        std::cout << "something went wrong, couldnt find position" << std::endl;
    }
}

template<class _NodeData = nodes::StringNodeData>
int compute_position(nodes::Node<_NodeData>* parent,
    nodes::Node<_NodeData>* child, NodeToInfo<_NodeData>* nodeToInfo)
{
    int position = get_child_position(parent, child);
    if(position==0) { return 0; }
    else if (position == _POSITION_NOT_FOUND)
    { 
        std::cout << "error, couldnt find child, compute pos" << std::endl;
        return 0;
    }
    
    nodes::Node<_NodeData>* left_sibling = parent->get_child(position-1);
    nodes::Node<_NodeData>* sibling_mapped = nodeToInfo->at(left_sibling)->mappedNode;
    nodes::Node<_NodeData>* parent_mapped = nodeToInfo->at(parent)->mappedNode;
    
    int mappedPos = get_child_position(parent_mapped, sibling_mapped);
    if(mappedPos == _POSITION_NOT_FOUND)
    {
        std::cout << "error, couldnt find mapped sibling, compute pos" << std::endl;
        return 0;
    }
    return mappedPos;
}

template<class _NodeData = nodes::StringNodeData>
void insert_edge_from_to(nodes::Node<_NodeData>* parent,
    nodes::Node<_NodeData>* child, NodeToInfo<_NodeData>* nodeToInfo,
    int position, char colour)
{
    // insert child
    std::vector<nodes::Node<_NodeData>*> children = parent->children_;
    if(position > parent->get_children_number())
    {
        position = parent->get_children_number();
    }
    children.insert(children.begin()+position, child);

    // insert edge
    std::vector<char>* edges = nodeToInfo->at(parent)->childEdges;
    if(edges == nullptr)
    {
        nodeToInfo->at(parent)->childEdges = new std::vector<char>();
        edges = nodeToInfo->at(parent)->childEdges;
    }
    edges->insert(edges->begin()+position, colour);

    // insert the parent
    std::vector<nodes::Node<_NodeData>*>* parents = nodeToInfo->at(child)->parents;
    if(parents == nullptr)
    {
        nodeToInfo->at(child)->parents = new std::vector<nodes::Node<_NodeData>*>();
        parents = nodeToInfo->at(child)->parents;
    }
    parents->push_back(parent);
}

template<class _NodeData = nodes::StringNodeData>
void remove_edge_from_to(nodes::Node<_NodeData>* parent,
    nodes::Node<_NodeData>* child, NodeToInfo<_NodeData>* nodeToInfo)
{
    // get the position
    int position = get_child_position(parent, child);

    if(position==_POSITION_NOT_FOUND)
    {
        std::cout << "error, couldnt find child, shouldnt happen" << std::endl;
        return;
    }

    // remove child
    std::vector<nodes::Node<_NodeData>*> children = parent->children_;
    children.erase(children.begin()+position);

    // remove edge
    std::vector<char>* edges = nodeToInfo->at(parent)->childEdges;
    if(edges != nullptr)
    {
        edges->erase(edges->begin()+position);
        if(edges->size()==0)
        {
            nodeToInfo->at(parent)->childEdges = nullptr;
        }
    }

    // remove the parent
    std::vector<nodes::Node<_NodeData>*>* parents = nodeToInfo->at(child)->parents;
    if(parents != nullptr)
    {
        parents = nodeToInfo->at(child)->parents;
        parents->erase(find(parents->begin(), parents->end(), parent));
        if(parents->size()==0)
        {
            nodeToInfo->at(child)->parents = nullptr;
        }
    }
}

template<class _NodeData = nodes::StringNodeData>
void create_hybrid_graph(nodes::Node<_NodeData>* hybrid,
    nodes::Node<_NodeData>* node_t2, NodeToInfo<_NodeData>* nodeToInfo)
{
    // get the mapped node of node_t2 and check if it is mapped
    nodes::Node<_NodeData>* node_mapped = nodeToInfo->at(node_t2)->mappedNode;
    if(node_mapped != nullptr)
    {
        // node is mapped
        // colour the node to mapped
        nodeToInfo->at(node_mapped)->colour = _COLOUR_MAPPED;
        // check if the labels differ
        if(! same_labels(node_mapped, node_t2))
        {
            // labels differ, colour the node + "rename"
            nodeToInfo->at(node_mapped)->colour = _COLOUR_RENAME;
            nodeToInfo->at(node_mapped)->label2 = node_t2->get_data()->get_label();
        }
        // check if the node_t2 has a parent, else it is the root of T2
        if(nodeToInfo->at(node_t2)->parents!=nullptr)
        {
            // get the parent of node_t2
            nodes::Node<_NodeData>* parent_t2 = nodeToInfo->at(node_t2)->parents->front(); 
            // get the mapped node of parent_t2 and check if it is mapped
            nodes::Node<_NodeData>* parent_mapped = nodeToInfo->at(parent_t2)->mappedNode;
            if(nodeToInfo->at(node_mapped)->parents != nullptr)
            {
                nodes::Node<_NodeData>* node_mapped_parent = nodeToInfo->at(node_mapped)->parents->front();
                if(node_mapped_parent==parent_mapped)
                {
                    // colour the edge from node_mapped_parent to node_mapped _COLOUR_MAPPED
                    colour_edge_from_to(node_mapped_parent, node_mapped, nodeToInfo, _COLOUR_MAPPED);
                }
                else
                {
                    // leave the edge from node_mapped_parent to node_mapped _COLOUR_DELETE
                    int position = compute_position(parent_t2, node_t2, nodeToInfo);
                    insert_edge_from_to(parent_mapped, node_mapped, nodeToInfo, position, _COLOUR_INSERT);
                }
            }
            else
            {
                // root of T1 is mapped to a node other than the root of T2
                // TODO position --> from t2
                remove_edge_from_to(hybrid, node_mapped, nodeToInfo);
            }
        }
    }
    else
    {
        // node is not mapped, but inserted
        nodes::Node<_NodeData>* inserted = new nodes::Node<_NodeData>(new _NodeData(node_t2->get_data()->get_label()));
        NodeInfo<_NodeData>* nodeInfo = new NodeInfo<_NodeData>();
        // set the parents to nullptr --> no parent
        nodeInfo->parents = nullptr;
        nodeInfo->childEdges = nullptr;
        nodeInfo->mappedNode = node_t2;
        nodeToInfo->at(node_t2)->mappedNode = inserted;

        ++_HIGHEST_ID_HYBRID;
        nodeInfo->id = _HIGHEST_ID_HYBRID;
        nodeToInfo->emplace(inserted, nodeInfo);

        nodes::Node<_NodeData>* parent_mapped = nullptr;
        nodes::Node<_NodeData>* parent_t2 = nullptr;
        if(nodeToInfo->at(node_t2)->parents != nullptr)
        {
            // get the parent of node_t2
            parent_t2 = nodeToInfo->at(node_t2)->parents->front(); 
            // get the mapped node of parent_t2
            parent_mapped = nodeToInfo->at(parent_t2)->mappedNode;
        }
        if(parent_mapped!=nullptr)
        {
            // the parent of n is mapped to a node in T1
            int position = compute_position(parent_t2, node_t2, nodeToInfo);
            insert_edge_from_to(parent_mapped, inserted, nodeToInfo, position, _COLOUR_INSERT);
            nodeToInfo->at(inserted)->parents = new std::vector<nodes::Node<_NodeData>*>();
            nodeToInfo->at(inserted)->parents->push_back(parent_mapped); 
        }
        else
        {
            // n is the new root of the hybrid graph
            insert_edge_from_to(inserted, hybrid, nodeToInfo, 0, _COLOUR_INSERT);
            hybrid = inserted;
        }
    }

    // preorder - tree2
    for(int i = 0; i < node_t2->get_children_number(); i++)
    {
        create_hybrid_graph(hybrid, node_t2->get_child(i), nodeToInfo);
    }
}

// visualise
// creates the visualisation for the two trees string depending on the type
// params:  type - visualisation type (string)
//          tree1 - the input tree (node)
//          tree2   - the output tree (node)
//
// returns the visualisation string (string)
template<class _NodeData = nodes::StringNodeData>
std::string visualise(char* type, nodes::Node<_NodeData>* tree1,
    nodes::Node<_NodeData>* tree2, 
    std::vector<std::array<nodes::Node<_NodeData>*, 2> > edm)
{
    // TODO remove string "end" and
    std::string output = "end";
    NodeToInfo<_NodeData>* nodeToInfo = new NodeToInfo<_NodeData>();

    if(std::string(type) == _type_hybrid)
    {
        int id_counter = 0;
        // mapping of old to new
        NodeToNode<_NodeData, _NodeData>* nodeToNode = 
            new NodeToNode<_NodeData, _NodeData>();
        // copy tree1, colour delete
        nodes::Node<_NodeData>* hybrid = 
            copy_tree_for_hybrid(static_cast<nodes::Node<_NodeData>*>(nullptr),
            tree1, nodeToInfo, nodeToNode, id_counter, 0, _COLOUR_DELETE);
        // copy tree2, colour insert
        nodes::Node<_NodeData>* output_tree = 
            copy_tree_for_hybrid(static_cast<nodes::Node<_NodeData>*>(nullptr),
            tree2, nodeToInfo, nodeToNode, id_counter, 0, _COLOUR_INSERT);
        // fill the mappedNodes
        fill_mappedNodes_nodeToInfo(edm, nodeToInfo, nodeToNode);
        
        // DEBUG TODO REMOVE - checks if the copy is correct
        bool c1 = is_copy_of_tree(hybrid, tree1, nodeToInfo, nodeToNode, _COLOUR_DELETE);
        bool c2 = is_copy_of_tree(output_tree, tree2, nodeToInfo, nodeToNode, _COLOUR_INSERT);
        if(!(c1 && c2))
        {
            std::cout << "error copying: " << c1 << ", " << c2 << std::endl;
        }
        _HIGHEST_ID_HYBRID = nodeToInfo->at(hybrid)->id;
        create_hybrid_graph(hybrid, output_tree, nodeToInfo);
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
    
    // compute the edm
    std::vector<std::array<nodes::Node<_NodeData>*, 2> > edm =
        zhang_shasha::compute_edit_mapping<_NodeData,
        nodes::StringCosts<_NodeData>>(tree1, tree2);

    std::string output;
    output = visualise(type, tree1, tree2, edm);

    return output;
}

} // Namespace: visualise

#endif //VISUALISATION_H