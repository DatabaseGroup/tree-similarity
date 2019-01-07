// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik.
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

/// \file ted_ub/cted_ub_impl.h
///
/// \details
/// Contains the implementation of the Constrained Tree Edit Distance algorithm.

#ifndef TREE_SIMILARITY_TED_UB_CTED_UB_H_IMPL_H
#define TREE_SIMILARITY_TED_UB_CTED_UB_H_IMPL_H

template <typename Label, typename CostModel>
ConstrainedUB<Label, CostModel>::ConstrainedUB() : c_() {}



template <typename Label, typename CostModel>
double ConstrainedUB<Label, CostModel>::cted_ub_ted(const node::Node<Label>& t1,
                                                     const node::Node<Label>& t2) {

  using data_structures::Matrix;

  init(t1, t2);
  
  std::vector<double> dt2(t1_input_size_ + 1);
  std::vector<double> df2(t2_input_size_ + 1);
  
  double a = -1;
  double b = -1;
  double c = -1;
  // double min = 0.0;
  // double value = 0.0;
  
  double costDel = 1.0;
  double costIns = 1.0;
  double costMatch = 1.0;
  
  dt_.at(0, 0) = 0;
  df_.at(0, 0) = 0;
  for (int i = 1; i <= t1_input_size_; ++i) {
    df_.at(i, 0) = 0;
    for (unsigned int k = 1; k <= t1_children_[i-1].size(); ++k) {
      df_.at(i, 0) += dt_.at(t1_children_[i-1][k-1] + 1, 0);
    }
    dt_.at(i, 0) = df_.at(i, 0) + costDel;
  }
  for (int j = 1; j <= t2_input_size_; ++j) {
    df_.at(0, j) = 0;
    for (unsigned int k = 1; k <= t2_children_[j-1].size(); ++k) {
      df_.at(0, j) += dt_.at(0, t2_children_[j-1][k-1] + 1);
    }
    dt_.at(0, j) = df_.at(0, j) + costIns;
  }
  
  for (int i = 1; i <= t1_input_size_; ++i) {
    std::fill(dt2.begin(), dt2.end(), std::numeric_limits<double>::infinity());
    std::fill(df2.begin(), df2.end(), std::numeric_limits<double>::infinity());
    
    for (int j = 1; j <= t2_input_size_; ++j) {
      e_.at(0, 0) = 0;
      for (unsigned int s = 1; s <= t1_children_[i-1].size(); ++s) {
        e_.at(s, 0) = e_.at(s-1, 0) + dt_.at(t1_children_[i-1][s-1] + 1, 0); // here we access d for subtree rooted in the s'th child of node i AND s starts with 1 but the postorder of the first child of node i is children1[i][s-1]
      }
      for (unsigned int t = 1; t <= t2_children_[j-1].size(); ++t) {
        e_.at(0, t) = e_.at(0, t-1) + dt_.at(0, t2_children_[j-1][t-1] + 1);
      }
      
      // TODO: but we already went over each pair of children
      for (unsigned int s = 1; s <= t1_children_[i-1].size(); ++s) {
        for (unsigned int t = 1; t <= t2_children_[j-1].size(); ++t) {
          a = e_.at(s, t-1) + dt_.at(0, t2_children_[j-1][t-1] + 1);
          b = e_.at(s-1, t) + dt_.at(t1_children_[i-1][s-1] + 1, 0);
          c = e_.at(s-1, t-1) + dt_.at(t1_children_[i-1][s-1] + 1, t2_children_[j-1][t-1] + 1); // TODO: This is a problematic part to reduce the memory. This requires to store distance for each pair of children of i and j.
          e_.at(s, t) = a >= b ? b >= c ? c : b : a >= c ? c : a;
        }
      }
      
      a = df_.at(0, j) + df2[j];
      b = df_.at(i, 0) + df_.at(i, j);
      c = e_.at(t1_children_[i-1].size(), t2_children_[j-1].size());
      
      df_.at(i, j) = a >= b ? b >= c ? c : b : a >= c ? c : a;
      
      a = dt_.at(0, j) + dt2[j];
      b = dt_.at(i, 0) + dt_.at(i, j);
      c = df_.at(i, j) + (t1_label_[i-1] == t2_label_[j-1] ? 0 : costMatch);
      
      dt_.at(i, j) = a >= b ? b >= c ? c : b : a >= c ? c : a;
      
      if (t2_parent_[j-1] > -1) {
        if (df_.at(i, j) - df_.at(0, j) < df2[t2_parent_[j-1]+1]) {
          df2[t2_parent_[j-1]+1] = df_.at(i, j) - df_.at(0, j);
        }
        if (dt_.at(i, j) - dt_.at(0, j) < dt2[t2_parent_[j-1]+1]) {
          dt2[t2_parent_[j-1]+1] = dt_.at(i, j) - dt_.at(0, j);
        }
      }
      if (t1_parent_[i-1] > -1) {
        if (df_.at(i, j) - df_.at(i, 0) < df_.at(t1_parent_[i-1]+1, j)) {
          df_.at(t1_parent_[i-1]+1, j) = df_.at(i, j) - df_.at(i, 0);
        }
        if (dt_.at(i, j) - dt_.at(i, 0) < dt_.at(t1_parent_[i-1]+1, j)) {
          dt_.at(t1_parent_[i-1]+1, j) = dt_.at(i, j) - dt_.at(i, 0);
        }
      }
    }
  }
  
  return dt_.at(t1_input_size_, t2_input_size_);
}

template <typename Label, typename CostModel>
double ConstrainedUB<Label, CostModel>::verify(const node::Node<Label>& t1, const node::Node<Label>& t2, double similarity_threshold){
  double ted_value = zhang_shasha_ted(t1, t2);
  if (ted_value <= similarity_threshold) {
    return ted_value;
  }
  return std::numeric_limits<double>::infinity();
}

template <typename Label, typename CostModel>
int ConstrainedUB<Label, CostModel>::index_nodes_recursion(
    const node::Node<Label>& node,
    std::vector<int>& parent,
    std::vector<std::vector<int>>& children,
    std::vector<int>& label,
    int& start_postorder) {  
  // Stores number of descendants of this node. Incrementally computed while
  // traversing the children.
  int desc_sum = 0;

  // To store postorder ids of this node's children.
  std::vector<int> nodes_children;
  
  auto children_start_it = std::begin(node.get_children());
  auto children_end_it = std::end(node.get_children());
  while (children_start_it != children_end_it) {
    desc_sum += index_nodes_recursion(*children_start_it, parent, children,
                                      label, start_postorder);
    // Here, start_postorder-1 is the postorder of the current child.
    // Collect children ids.
    nodes_children.push_back(start_postorder-1);
    // Continue to consecutive children.
    ++children_start_it;
  }
  
  // Here, start_postorder holds this node's postorder id here.

  // Parent - dummy element to grow the vector by this node.
  parent.push_back(-1);
  // Set this node's postorder value as parent for all its children.
  for (const auto& child_id : nodes_children) {
    parent.at(child_id) = start_postorder;
  }
  
  // Children.
  children.push_back(nodes_children);
  
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
void ConstrainedUB<Label, CostModel>::index_nodes(
    const node::Node<Label>& root,
    std::vector<int>& parent,
    std::vector<std::vector<int>>& children,
    std::vector<int>& label) {
  // Orders start with '0'.
  int start_postorder = 0;
  index_nodes_recursion(root, parent, children, label, start_postorder);
  // Here, start_postorder and start_preorder store the size of tree minus 1.
};

template <typename Label, typename CostModel>
void ConstrainedUB<Label, CostModel>::init(const node::Node<Label>& t1,
                                    const node::Node<Label>& t2) {
  t1_parent_.clear();
  t2_parent_.clear();
  t1_children_.clear();
  t2_children_.clear();
  dict_.clear();
  t1_label_.clear();
  t2_label_.clear();
  
  // TODO: Do not call get_tree_size() that causes an additional tree traversal.
  //       Index subtree sizes instead - they'll be used anyways.
  // TODO: Getting tree size first makes things easier while indexing nodes.
  //       If the size is known upfront it could be immediately assigned.
  t1_input_size_ = t1.get_tree_size();
  t2_input_size_ = t2.get_tree_size();
  
  dt_ = data_structures::Matrix<double>(t1_input_size_+1, t2_input_size_+1);
  df_ = data_structures::Matrix<double>(t1_input_size_+1, t2_input_size_+1);
  e_ = data_structures::Matrix<double>(t1_input_size_+1, t2_input_size_+1);
  
  // Fill the matrices with inf.
  dt_.fill_with(std::numeric_limits<double>::infinity());
  df_.fill_with(std::numeric_limits<double>::infinity());
  e_.fill_with(std::numeric_limits<double>::infinity());
  
  index_nodes(t1, t1_parent_, t1_children_, t1_label_);
  index_nodes(t2, t2_parent_, t2_children_, t2_label_);
};

#endif // TREE_SIMILARITY_TED_UB_CTED_UB_H_IMPL_H
