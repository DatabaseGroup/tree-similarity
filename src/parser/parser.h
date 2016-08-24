#ifndef PARSER_H
#define PARSER_H

namespace parser {

typedef std::unordered_map<std::string, int> LabelIDMap;

nodes::Node* create_tree_from_string(char* str, LabelIDMap& hashtable,
  int& labelid)
{ 
  int length = std::strlen(str);
  int scope = -1;
   
  std::vector<nodes::Node*> scope_parent_list;
  std::vector<nodes::Node*>::const_iterator it;
  std::string label = "";
    
  nodes::Node* root = new nodes::Node(1, 1);
  for (int i = 0; i < length; i++) {
    if (i != 0 && scope <= -1) {
      break;
    }

    if (str[i] == '{') {
      if (label.length() != 0) {
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }
        
        nodes::Node* tmp_node = new nodes::Node(hashtable[label]);
        scope_parent_list.push_back(tmp_node);
        
        if (scope > 0) {
          it = scope_parent_list.begin() + scope_parent_list.size() - 2;
          //nodes::Node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << " scope: " << scope << std::endl;
          //tmp->add_child(tmp_node);
          (*it)->add_child(tmp_node);
        } else {
          delete root;
          root = tmp_node;
          //std::cout << label << ".parent_id:/" << " scope: " << scope << std::endl;
        }
        
        label = "";
      }
      ++scope;
    } else if (str[i] == '}') {
      if (label.length() != 0) {
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }

        nodes::Node* tmp_node = new nodes::Node(hashtable[label]);
        scope_parent_list.push_back(tmp_node);
        
        if (scope > 0 && scope_parent_list.size() > 1) {
          it = scope_parent_list.begin() + scope_parent_list.size() - 2;
          //nodes::Node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << " scope: " << scope << std::endl;
          //tmp->add_child(tmpnode);
          (*it)->add_child(tmp_node);
        } else {
          delete root;
          root = tmp_node;
          //std::cout << label << ".parent_id:/" << " scope: " << scope << std::endl;
        }
        
        label = "";
      }
      scope_parent_list.resize(scope);
      --scope;
    } else {
      label += str[i];
    }
  }
  return root;
}

} // namespace parser

#endif // PARSER_H
