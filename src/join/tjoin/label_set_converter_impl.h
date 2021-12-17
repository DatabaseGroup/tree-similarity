// The MIT License (MIT)
// Copyright (c) 2017 Thomas Huetter
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

/// \file join/tjoin/label_set_converter_impl.h
///
/// \details
/// Implements an algorithm that converts a collection of trees into a collection 
/// of label sets. The node labels of a tree are substituted with their inverted 
/// label frequency order number. Each set holds labels and each label holds all 
/// elements with that label.

#pragma once

template<typename Label>
Converter<Label>::Converter() {}

template<typename Label>
void Converter<Label>::assignFrequencyIdentifiers(
    std::vector<node::Node<Label>>& trees_collection,
    std::vector<std::pair<int, std::vector<label_set_converter::LabelSetElement>>>& sets_collection) {
  // token_map = {(token, tokcnt) -> id}
  typename std::unordered_map<Label, int, labelhash> token_map;
  // token_list = [(#occurrences, data_nr)]
  std::vector<std::pair<int, int>> token_count_list;

  // for each tree in the tree collection
  for(const auto& tree: trees_collection) {
    // record = [tokid]
    std::vector<label_set_converter::LabelSetElement> record;
    // {tokid -> 1}
    std::unordered_map<int, label_set_converter::LabelSetElement> record_labels;
    // number of nodes of the tree
    int tree_size = tree.get_tree_size();

    // postorder id for recursive postorder traversal
    int postorder_id = 0;

    // array of records stored in sets_collection
    create_record(tree, postorder_id, tree_size, token_map, record_labels);

    // reset variables that are needed for positional evaluation
    actual_pre_order_number_ = 0;
    actual_depth_ = 0;

    for (const auto& it : record_labels) 
      record.emplace_back(it.second);

    // add to vector of label sets
    sets_collection.emplace_back(tree_size, record);
  }

  // token_count_list = [tokenfrequency, tokenid]
  for(int i = 0; i < next_token_id_; ++i)
    token_count_list.emplace_back(0, i);
  for(const auto& record: sets_collection)
    for(const auto& token: record.second)
      token_count_list[token.id].first += token.weight;

  // sort token_count_list by tokenfrequency
  std::sort(token_count_list.begin(), token_count_list.end(), pairComparator);

  // tokenmaplist[tokenid] = frequencyID
  std::vector<int> tokenmaplist (token_count_list.size());
  for(std::size_t i = 0; i < token_count_list.size(); ++i)
    tokenmaplist[token_count_list[i].second] = i;

  // substitute the tokenIDs with frequencyIDs
  for(auto& record: sets_collection) {
    for(std::size_t i = 0; i < record.second.size(); ++i)
      record.second[i].id = tokenmaplist[record.second[i].id];

    // sort integers of a record ascending
    std::sort(record.second.begin(), record.second.end(), LabelSetElementComparator);

    // weight of an element in the set up to its position
    int weight_sum = 0;
    for(std::size_t i = 0; i < record.second.size(); ++i) {
      weight_sum += record.second[i].weight;
      record.second[i].weight_so_far = weight_sum;
    }
  }

  // sort all label sets by length (number of elements, not labels)
  // TODO: needed for an unordered input, but messes up the order of an ordered 
  //       input file, that should match in case of a tree similarity join
  // std::sort(sets_collection.begin(), sets_collection.end(), vectorComparator);
}

template<typename Label>
int Converter<Label>::create_record(
    const node::Node<Label>& tree_node, int& postorder_id, int tree_size,
    std::unordered_map<Label, int, labelhash>& token_map, 
    std::unordered_map<int, label_set_converter::LabelSetElement>& record_labels) {

  // number of children = subtree_size - 1
  // subtree_size = 1 -> actual node + sum of children
  int subtree_size = 1;

  // increase depth for children
  ++actual_depth_;

  // do recursively for all children
  for (const auto& child: tree_node.get_children()) {
    subtree_size += create_record(child, postorder_id, tree_size, token_map, record_labels);
  }

  // update postorder id
  ++postorder_id;
  // decrease depth for children
  --actual_depth_;
  // raise pre order number with each node
  ++actual_pre_order_number_;

  // string label of the current node of the node
  Label key = tree_node.label();
  std::string label_str = tree_node.label().to_string();

  // lookup key in token_map
  typename std::unordered_map<Label, int, labelhash>::const_iterator 
                              token_in_map = token_map.find(key);

  // if tokenkey not in map
  if(token_in_map == token_map.end()) {
    // store tokenid in global tokenmap
    token_map.emplace(key, next_token_id_);
    // create new set element
    label_set_converter::LabelSetElement se = {next_token_id_, 1};
    // add positional information
    se.struct_vect.emplace_back(postorder_id, actual_pre_order_number_ - subtree_size, 
      tree_size - (actual_pre_order_number_ + actual_depth_), actual_depth_, subtree_size - 1);
    // append to record (id, weight, left, right, ancestor, descendant)
    record_labels.emplace(next_token_id_, se);
    // update next_token_id_
    next_token_id_ += 1;
  } else {
    if(record_labels.find(token_map[key]) == record_labels.end()) {
      label_set_converter::LabelSetElement se = {token_map[key], 1};
      // add positional information
      se.struct_vect.emplace_back(postorder_id, actual_pre_order_number_ - subtree_size, 
        tree_size - (actual_pre_order_number_ + actual_depth_), actual_depth_, subtree_size - 1);
      // append to record (id, weight, left, right, ancestor, descendant)
      record_labels.emplace(token_map[key], se);
    } else {
      // increase weight
      ++record_labels[token_map[key]].weight;
      // push position information to its duplicates
      record_labels[token_map[key]].struct_vect.emplace_back(postorder_id, actual_pre_order_number_ - subtree_size, 
          tree_size - (actual_pre_order_number_ + actual_depth_), actual_depth_, subtree_size - 1);

    }
  }

  return subtree_size;
}

template<typename Label>
int Converter<Label>::get_number_of_labels() const {
  return next_token_id_;
}
