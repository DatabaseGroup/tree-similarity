#ifndef PARSER_H
#define PARSER_H

namespace parser {

typedef std::unordered_map<std::string, int> LabelIDMap;

Node* create_tree_from_string (char* str, LabelIDMap& hashtable,
  int& labelid)
{ 
  int length = std::strlen(str);
  int scope = -1;
   
  std::vector<Node*> scopeParentList;
  std::vector<Node*>::const_iterator it;
  std::string label = "";
    
  Node* root = new Node(1, 1);
  for (int i = 0; i < length; i++) {
    if (i != 0 && scope <= -1) {
      break;
    }

    if (str[i] == '{') {
      if (label.length() != 0) {
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }
        Node* tmpnode = new Node(hashtable[label]);
        scopeParentList.push_back(tmpnode);
        if (scope > 0) {
          it = scopeParentList.begin() + scopeParentList.size() - 2;
          Node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << " scope: " << scope << std::endl;
          tmp->add_child(tmpnode);
        } else {
          delete root;
          root = tmpnode;
          //std::cout << label << ".parent_id:/" << " scope: " << scope << std::endl;
        }
        label = "";
      }
      scope++;
    } else if (str[i] == '}') {
      if(label.length() != 0){
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }
        Node* tmpnode = new Node(hashtable[label]);
        scopeParentList.push_back(tmpnode);
        if (scope > 0 && scopeParentList.size() > 1) {
          it = scopeParentList.begin() + scopeParentList.size() - 2;
          Node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << " scope: " << scope << std::endl;
          tmp->add_child(tmpnode);
        } else {
          delete root;
          root = tmpnode;
          //std::cout << label << ".parent_id:/" << " scope: " << scope << std::endl;
        }
        label = "";
      }
      scopeParentList.resize(scope);
      --scope;
    } else {
      label += str[i];
    }
  }
  return root;
}

};

#endif // PARSER_H
