// The MIT License (MIT)
// Copyright (c) 2020 Thomas Huetter.
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

/// Implements the bitmap filter lower bound for trees.

#pragma once

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted(
    const TreeIndex& t1, const TreeIndex& t2) {
  // Call bitmap filter with a total bitmap size of 1024.
  return ted2(t1, t2, 1024);
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted2(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // Size of the bitmap.
  unsigned long int bitmap_size = size;
  
  // Bitmap for all nodes.
  std::vector<bool> bitmap_(bitmap_size);

  // Iterate through one tree and create the bitmaps.
  for (unsigned int i = 0; i < t1.tree_size_; i++) {
    bitmap_[t1.postl_to_label_id_[i]%bitmap_size] = true;
  }

  // Iterate through the other tree and count overlap.
  unsigned long int overlap = 0;
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    if (bitmap_[t2.postl_to_label_id_[j]%bitmap_size] == true) {
      // Found an overlap. Reset bitmap position.
      overlap++;
      bitmap_[t2.postl_to_label_id_[j]%bitmap_size] = false;
    }
  }

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - overlap;
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted3(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // The given total size is devided by half since two bitmaps are used.
  unsigned long int bitmap_size = std::ceil(size / 2);
  
  // Bitmap for keys.
  std::vector<bool> key_bitmap(bitmap_size);
  // Bitmap for literals.
  std::vector<bool> literal_bitmap(bitmap_size);
  // Number of objects.
  long int object_count_t1 = 0;
  long int object_count_t2 = 0;
  // Number of arrays.
  long int array_count_t1 = 0;
  long int array_count_t2 = 0;

  // Iterate through one tree and create the bitmaps.
  for (unsigned int i = 0; i < t1.tree_size_; i++) {
    if (t1.postl_to_type_[i] == 0) { // Object.
      object_count_t1++;
    } else if (t1.postl_to_type_[i] == 1) { // Array.
      array_count_t1++;
    } else if (t1.postl_to_type_[i] == 2) { // Key.
      key_bitmap[t1.postl_to_label_id_[i]%bitmap_size] = true;
    } else if (t1.postl_to_type_[i] == 3) { // Literal.
      literal_bitmap[t1.postl_to_label_id_[i]%bitmap_size] = true;
    }
  }

  // Iterate through the other tree and count overlap.
  unsigned long int overlap = 0;
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    if (t2.postl_to_type_[j] == 0) { // Object.
      object_count_t2++;
    } else if (t2.postl_to_type_[j] == 1) { // Array.
      array_count_t2++;
    } else if (t2.postl_to_type_[j] == 2) { // Key.
      if (key_bitmap[t2.postl_to_label_id_[j]%bitmap_size] == true) {
        // Found an overlap. Reset bitmap position.
        overlap++;
        key_bitmap[t2.postl_to_label_id_[j]%bitmap_size] = false;
      }
    } else if (t2.postl_to_type_[j] == 3) { // Literal.
      if (literal_bitmap[t2.postl_to_label_id_[j]%bitmap_size] == true) {
        // Found an overlap. Reset bitmap position.
        overlap++;
        literal_bitmap[t2.postl_to_label_id_[j]%bitmap_size] = false;
      }
    }
  }
  // Add costs from objects and arrays.
  overlap += std::min(object_count_t1, object_count_t2) + std::min(array_count_t1, array_count_t2);

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - overlap;
}
