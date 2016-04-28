typedef std::unordered_map<std::string, int> ht_ids;
node* create_tree_from_string (char* str, ht_ids& hashtable, int& labelid) { 
  int length = std::strlen(str);
  int scope = -1;
   
  std::vector<node*> scopeParentList;
  std::vector<node*>::const_iterator it;

  std::string label = "";
    
  node* root = new node(1,1);
  for (int i = 0; i < length; i++) {
    if (str[i] == '{') {
      if (label.length() != 0) {
        if (!hashtable.count(label)) {
          hashtable.emplace(label, labelid++);
        }
        node* tmpnode = new node(hashtable[label]);
        scopeParentList.push_back(tmpnode);
        if (scope > 0) {
          it = scopeParentList.begin() + scopeParentList.size() - 2;
          node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << " scope: " << scope << std::endl;
          tmp->add_child(tmpnode);
        } else {
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
        node* tmpnode = new node(hashtable[label]);
        std::vector<node*>::reverse_iterator tmp_it = scopeParentList.rbegin();
        scopeParentList.push_back(tmpnode);
        if (scope > 0 && scopeParentList.size() > 1) {
          it = scopeParentList.begin() + scopeParentList.size() - 2;
          node* tmp = *it;
          //std::cout << label << ".parent_id:" << tmp->get_label_id() << " scope: " << scope << std::endl;
          tmp->add_child(tmpnode);
        } else {
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
  std::cout << "ok" << std::endl;
  return root;
}
