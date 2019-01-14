// The MIT License (MIT)
// Copyright (c) 2018 Nikolaus Augsten, Mateusz Pawlik.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

/// \file ted_lb/sed_lb_impl.h
///
/// \details
/// Contains the implementation of the string edit distance algorithm.

#ifndef TREE_SIMILARITY_TED_LB_SED_LB_IMPL_H
#define TREE_SIMILARITY_TED_LB_SED_LB_IMPL_H

template <typename Label, typename CostModel>
StringEditDistanceLB<Label, CostModel>::StringEditDistanceLB() : c_() {}

template <typename Label, typename CostModel>
double StringEditDistanceLB<Label, CostModel>::verify(const node::Node<Label>& t1,
    const node::Node<Label>& t2, double similarity_threshold) {
  init(t1, t2);
  int int_threshold = static_cast <int> (std::ceil(similarity_threshold));
  
  // TODO
  
  return std::numeric_limits<double>::infinity();
};

template <typename Label, typename CostModel>
double StringEditDistanceLB<Label, CostModel>::sed_lb_ted_k(const node::Node<Label>& t1,
    const node::Node<Label>& t2, const int k) {
  return sed_lb_ted(t1, t2);
};

template <typename Label, typename CostModel>
double StringEditDistanceLB<Label, CostModel>::sed_lb_ted(const node::Node<Label>& t1,
    const node::Node<Label>& t2) {
  init(t1, t2);
  
  // TODO: Swap input trees for memory optimisation, as below.
  // if (x.length() > y.length()) {
  //   String tmp = x;
  //   x = y;
  //   y = tmp;
  // }
  
  std::vector<double> cp(t2_input_size_ + 1);
  std::vector<double> cc(t2_input_size_ + 1);
  for (int j = 0; j <= t2_input_size_; ++j) {
    cp[j] = j;
  }
  for (int i = 1; i <= t1_input_size_; ++i) {
    cc[0] = i;
    for (int j = 1; j <= t2_input_size_; ++j) {
      ++subproblem_counter;
      unsigned int c1 = t1_label_[i - 1];
      unsigned int c2 = t2_label_[j - 1];
      cc[j] = std::min({
          cp[j - 1] + ((c1 == c2) ? 0 : 1),
          cc[j - 1] + 1,
          cp[j] + 1
          });
    }
    std::swap(cp, cc);
  }
  return cp[t2_input_size_]; // note: cc is asigned to cp, cc is overwritten!
};

template <typename Label, typename CostModel>
int StringEditDistanceLB<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::vector<int>& label,
    int& start_postorder) {
  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  int desc_sum = 0;
  
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it = std::end(node.get_children());
  while (children_start_it != children_end_it) {
    desc_sum += index_nodes_recursion(*children_start_it, label,
                                      start_postorder);
    // Continue to consecutive children.
    ++children_start_it;
  }
  
  // Here, start_postorder holds this node's postorder id here.
  
  // New inverted list.
  unsigned int label_id_in_dict = dict_.insert(node.label());
  label.push_back(label_id_in_dict);
  
  // Increment start_postorder for the consecutive node in postorder have the
  // correct id.
  ++start_postorder;
  
  // Return the number of nodes in the subtree rooted at this node.
  return desc_sum + 1;
};

template <typename Label, typename CostModel>
void StringEditDistanceLB<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::vector<int>& label) {
  // Orders start with '0'.
  int start_postorder = 0;
  index_nodes_recursion(root, label, start_postorder);
  // Here, start_postorder and start_preorder store the size of tree minus 1.
};

template <typename Label, typename CostModel>
void StringEditDistanceLB<Label, CostModel>::init(const node::Node<Label>& t1,
                                    const node::Node<Label>& t2) {
  dict_.clear();
  t1_label_.clear();
  t2_label_.clear();
  
  // TODO: Do not call get_tree_size() that causes an additional tree traversal.
  //       Index subtree sizes instead - they'll be used anyways.
  // TODO: Getting tree size first makes things easier while indexing nodes.
  //       If the size is known upfront it could be immediately assigned.
  t1_input_size_ = t1.get_tree_size();
  t2_input_size_ = t2.get_tree_size();
  
  index_nodes(t1, t1_label_);
  index_nodes(t2, t2_label_);
  
  // Reset subproblem counter.
  subproblem_counter = 0;
};

template <typename Label, typename CostModel>
const unsigned long long int StringEditDistanceLB<Label, CostModel>::get_subproblem_count() const {
  return subproblem_counter;
}

#endif // TREE_SIMILARITY_TED_LB_SED_LB_IMPL_H
