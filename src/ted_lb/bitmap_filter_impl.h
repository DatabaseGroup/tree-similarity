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
  std::vector<bool> bitmap_t1(bitmap_size);
  std::vector<bool> bitmap_t2(bitmap_size);

  // Iterate through one tree and create the bitmap.
  for (unsigned int i = 0; i < t1.tree_size_; i++) {
    bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size] = true;
  }

  // Iterate through the other tree and create the bitmap.
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size] = true;
  }

  // Popcount of bitmap_t1 XOR bitmap_t2.
  unsigned long int int_ub = 0;
  for (unsigned int b = 0; b < bitmap_size; b++) {
    if ((bitmap_t1[b] == true && bitmap_t2[b] == false) ||
        (bitmap_t1[b] == false && bitmap_t2[b] == true)) {
      int_ub++;
    }
  }

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - std::floor((t1.tree_size_ + t2.tree_size_ - int_ub)/2);
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted3(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // The given total size is devided by half since two bitmaps are used.
  unsigned long int bitmap_size = std::ceil(size / 2);
  
  // Bitmap for keys.
  std::vector<bool> key_bitmap_t1(bitmap_size);
  std::vector<bool> key_bitmap_t2(bitmap_size);
  // Bitmap for literals.
  std::vector<bool> literal_bitmap_t1(bitmap_size);
  std::vector<bool> literal_bitmap_t2(bitmap_size);
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
      key_bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size] = true;
    } else if (t1.postl_to_type_[i] == 3) { // Literal.
      literal_bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size] = true;
    }
  }

  // Iterate through one tree and create the bitmaps.
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    if (t2.postl_to_type_[j] == 0) { // Object.
      object_count_t2++;
    } else if (t2.postl_to_type_[j] == 1) { // Array.
      array_count_t2++;
    } else if (t2.postl_to_type_[j] == 2) { // Key.
      key_bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size] = true;
    } else if (t2.postl_to_type_[j] == 3) { // Literal.
      literal_bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size] = true;
    }
  }
  
  // Popcount of bitmap_t1 XOR bitmap_t2.
  unsigned long int int_ub = 0;
  for (unsigned int b = 0; b < bitmap_size; b++) {
    if ((key_bitmap_t1[b] == true && key_bitmap_t2[b] == false) ||
        (key_bitmap_t1[b] == false && key_bitmap_t2[b] == true)) {
      int_ub++;
    }
    if ((literal_bitmap_t1[b] == true && literal_bitmap_t2[b] == false) ||
        (literal_bitmap_t1[b] == false && literal_bitmap_t2[b] == true)) {
      int_ub++;
    }
  }
  // Add costs from objects and arrays.
  int_ub += std::abs(object_count_t1 - object_count_t2) + std::abs(array_count_t1 - array_count_t2);

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - std::floor((t1.tree_size_ + t2.tree_size_ - int_ub)/2);
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted4(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // Size of the bitmap.
  unsigned long int bitmap_size = size;
  
  // Bitmap for all nodes.
  std::vector<bool> bitmap_t1(bitmap_size);
  std::vector<bool> bitmap_t2(bitmap_size);

  // Iterate through one tree and create the bitmap.
  for (unsigned int i = 0; i < t1.tree_size_; i++) {
    bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size] = !bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size];
  }

  // Iterate through the other tree and create the bitmap.
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size] = !bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size];
  }

  // Popcount of bitmap_t1 XOR bitmap_t2.
  unsigned long int int_ub = 0;
  for (unsigned int b = 0; b < bitmap_size; b++) {
    if ((bitmap_t1[b] == true && bitmap_t2[b] == false) ||
        (bitmap_t1[b] == false && bitmap_t2[b] == true)) {
      int_ub++;
    }
  }

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - std::floor((t1.tree_size_ + t2.tree_size_ - int_ub)/2);
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted5(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // The given total size is devided by half since two bitmaps are used.
  unsigned long int bitmap_size = std::ceil(size / 2);
  
  // Bitmap for keys.
  std::vector<bool> key_bitmap_t1(bitmap_size);
  std::vector<bool> key_bitmap_t2(bitmap_size);
  // Bitmap for literals.
  std::vector<bool> literal_bitmap_t1(bitmap_size);
  std::vector<bool> literal_bitmap_t2(bitmap_size);
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
      key_bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size] = !key_bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size];
    } else if (t1.postl_to_type_[i] == 3) { // Literal.
      literal_bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size] = !literal_bitmap_t1[t1.postl_to_label_id_[i]%bitmap_size];
    }
  }

  // Iterate through one tree and create the bitmaps.
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    if (t2.postl_to_type_[j] == 0) { // Object.
      object_count_t2++;
    } else if (t2.postl_to_type_[j] == 1) { // Array.
      array_count_t2++;
    } else if (t2.postl_to_type_[j] == 2) { // Key.
      key_bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size] = !key_bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size];
    } else if (t2.postl_to_type_[j] == 3) { // Literal.
      literal_bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size] = !literal_bitmap_t2[t2.postl_to_label_id_[j]%bitmap_size];
    }
  }
  
  // Popcount of bitmap_t1 XOR bitmap_t2.
  unsigned long int int_ub = 0;
  for (unsigned int b = 0; b < bitmap_size; b++) {
    if ((key_bitmap_t1[b] == true && key_bitmap_t2[b] == false) ||
        (key_bitmap_t1[b] == false && key_bitmap_t2[b] == true)) {
      int_ub++;
    }
    if ((literal_bitmap_t1[b] == true && literal_bitmap_t2[b] == false) ||
        (literal_bitmap_t1[b] == false && literal_bitmap_t2[b] == true)) {
      int_ub++;
    }
  }
  // Add costs from objects and arrays.
  int_ub += std::abs(object_count_t1 - object_count_t2) + std::abs(array_count_t1 - array_count_t2);

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - std::floor((t1.tree_size_ + t2.tree_size_ - int_ub)/2);
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted6(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // Size of the bitmap.
  unsigned long int bitmap_size = size;
  int bitmap_pos = 0;
  int cnt = 0;
  
  // Bitmap for all nodes.
  std::vector<bool> bitmap_t1(bitmap_size);
  std::vector<bool> bitmap_t2(bitmap_size);

  // Iterate through one tree and create the bitmaps.
  for (unsigned int i = 0; i < t1.tree_size_; i++) {
    // Set the next free position in the bitmap.
    bitmap_pos = t1.postl_to_label_id_[i] % bitmap_size;
    cnt = 0;
    while (bitmap_t1[bitmap_pos] == true && cnt < bitmap_size) {
      bitmap_pos = (bitmap_pos + 1) % bitmap_size;
      cnt++;
    }
    if (cnt != bitmap_size) {
      bitmap_t1[bitmap_pos] = true;
    }
  }

  // Iterate through one tree and create the bitmaps.
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    // Set the next free position in the bitmap.
    bitmap_pos = t2.postl_to_label_id_[j] % bitmap_size;
    cnt = 0;
    while (bitmap_t2[bitmap_pos] == true && cnt < bitmap_size) {
      bitmap_pos = (bitmap_pos + 1) % bitmap_size;
      cnt++;
    }
    if (cnt != bitmap_size) {
      bitmap_t2[bitmap_pos] = true;
    }
  }

  // Popcount of bitmap_t1 XOR bitmap_t2.
  unsigned long int int_ub = 0;
  for (unsigned int b = 0; b < bitmap_size; b++) {
    if ((bitmap_t1[b] == true && bitmap_t2[b] == false) ||
        (bitmap_t1[b] == false && bitmap_t2[b] == true)) {
      int_ub++;
    }
  }

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - std::floor((t1.tree_size_ + t2.tree_size_ - int_ub)/2);
}

template <typename CostModel, typename TreeIndex>
double BitmapFilter<CostModel, TreeIndex>::ted7(
    const TreeIndex& t1, const TreeIndex& t2, const unsigned int size) {
  
  // The given total size is devided by half since two bitmaps are used.
  unsigned long int bitmap_size = std::ceil(size / 2);
  int bitmap_pos = 0;
  int cnt = 0;
  
  // Bitmap for keys.
  std::vector<bool> key_bitmap_t1(bitmap_size);
  std::vector<bool> key_bitmap_t2(bitmap_size);
  // Bitmap for literals.
  std::vector<bool> literal_bitmap_t1(bitmap_size);
  std::vector<bool> literal_bitmap_t2(bitmap_size);
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
      bitmap_pos = t1.postl_to_label_id_[i] % bitmap_size;
      cnt = 0;
      while (key_bitmap_t1[bitmap_pos] == true && cnt < bitmap_size) {
        bitmap_pos = (bitmap_pos + 1) % bitmap_size;
        cnt++;
      }
      if (cnt != bitmap_size) {
        key_bitmap_t1[bitmap_pos] = true;
      }
    } else if (t1.postl_to_type_[i] == 3) { // Literal.
      bitmap_pos = t1.postl_to_label_id_[i]%bitmap_size;
      cnt = 0;
      while (literal_bitmap_t1[bitmap_pos] == true && cnt < bitmap_size) {
        bitmap_pos = (bitmap_pos + 1) % bitmap_size;
        cnt++;
      }
      if (cnt != bitmap_size) {
        literal_bitmap_t1[bitmap_pos] = true;
      }
    }
  }

  // Iterate through the other tree and create the bitmaps.
  for (unsigned int j = 0; j < t2.tree_size_; j++) {
    if (t2.postl_to_type_[j] == 0) { // Object.
      object_count_t2++;
    } else if (t2.postl_to_type_[j] == 1) { // Array.
      array_count_t2++;
    } else if (t2.postl_to_type_[j] == 2) { // Key.
      bitmap_pos = t2.postl_to_label_id_[j]%bitmap_size;
      cnt = 0;
      while (key_bitmap_t2[bitmap_pos] == true && cnt < bitmap_size) {
        bitmap_pos = (bitmap_pos + 1) % bitmap_size;
        cnt++;
      }
      if (cnt != bitmap_size) {
        key_bitmap_t2[bitmap_pos] = true;
      }
    } else if (t2.postl_to_type_[j] == 3) { // Literal.
      bitmap_pos = t2.postl_to_label_id_[j]%bitmap_size;
      cnt = 0;
      while (literal_bitmap_t2[bitmap_pos] == true && cnt < bitmap_size) {
        bitmap_pos = (bitmap_pos + 1) % bitmap_size;
        cnt++;
      }
      if (cnt != bitmap_size) {
        literal_bitmap_t2[bitmap_pos] = true;
      }
    }
  }

  // Popcount of bitmap_t1 XOR bitmap_t2.
  unsigned long int int_ub = 0;
  for (unsigned int b = 0; b < bitmap_size; b++) {
    if ((key_bitmap_t1[b] == true && key_bitmap_t2[b] == false) ||
        (key_bitmap_t1[b] == false && key_bitmap_t2[b] == true)) {
      int_ub++;
    }
    if ((literal_bitmap_t1[b] == true && literal_bitmap_t2[b] == false) ||
        (literal_bitmap_t1[b] == false && literal_bitmap_t2[b] == true)) {
      int_ub++;
    }
  }
  // Add costs from objects and arrays.
  int_ub += std::abs(object_count_t1 - object_count_t2) + std::abs(array_count_t1 - array_count_t2);

  // Return the overlaping bitmap positions which is the size of 
  // the larger tree minus the number of nodes in common.
  return std::max(t1.tree_size_, t2.tree_size_) - std::floor((t1.tree_size_ + t2.tree_size_ - int_ub)/2);
}
