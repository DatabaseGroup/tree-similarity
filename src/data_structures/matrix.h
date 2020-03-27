// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik
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

/// \file zhang_shasha/zhang_shasha.h
///
/// \details
/// Encapsulates a two dimensional array of a given type (generic).
/// Main reason: it gives better performance than the standard _type** approach
/// due to its allocation pattern. In a _type** array each row in the matrix is
/// a separately allocated dynamic array on the heap - O(n) in general. Same
/// for the deallocation because one has to iterate through all elements again.
/// Moreover, separate allocations tend to live in discontinuous chunks of
/// memory. As a result, lots of cache misses happen, in general. This slows
/// down the actual access.


#pragma once

#include <memory>
#include <vector>
#include <iostream>
#include <string>
#include <algorithm>
#include <stdexcept>

namespace data_structures {

template<typename ElementType>
class Matrix {
// Member variables.
private:
  /// Number of rows in the matrix.
  size_t rows_;
  /// Number of columns in the matrix.
  size_t columns_;
  /// Consecutive-allocated long vector containing the matrix elements.
  std::vector<ElementType> data_;
// Member functions.
public:
  /// Constructor(s).
  Matrix() = default; // Used while constructing ZS-Algorithm.
  Matrix(size_t rows, size_t columns);
  /// Returns the number of rows.
  size_t get_rows() const;
  /// Returns the number of columns.
  size_t get_columns() const;
  /// Accesses the element of specific row and column.
  ///
  /// \param row The row to be accessed.
  /// \param col The column to be accessed.
  ///
  /// \return Reference to the specified element.
  ElementType& at(size_t row, size_t col);
  /// Reads the element at specific row and column.
  /// Using this method the element's value cannot be modified.
  ///
  /// \param row The row to be accessed.
  /// \param col The column to be accessed.
  ///
  /// \return Reference to the specified element.
  const ElementType& read_at(size_t row, size_t col) const;
  void fill_with(ElementType value);
};

/// A specialised matrix, where only the elements on the diagonal band matter.
/// The elements on that band in the BandMatrix are shifted left for reducing
/// memory usage. In order to access the correct element of a BandMatrix, the
/// column coordinate has to be translated.
///
/// One can use BandMatrix as it was a regular rectangular matrix and the index
/// translations are transparent. One has to ensure that all accessed elements
/// are within the band.
template<typename ElementType>
class BandMatrix : public Matrix<ElementType> {
// Member variables.
private:
  /// The parameter specifying the width of the diagonal bend which is
  /// 2*band_width_+1.
  size_t band_width_;
// Member functions.
public:
  /// Constructor(s).
  BandMatrix() = default;
  // TODO: Take original rows and original columns and band_width.
  // TODO: Not every original cell is in the BandMatrix but each of them should
  //       be possible to call. Throw exception if the called cell is not
  //       addressed in the BandMatrix.
  BandMatrix(size_t rows, size_t band_width);
  /// Returns the band width parameter.
  size_t get_band_width() const;
  /// Overwrites the access methods of the rectangular matrix such that the
  /// column coordinate is correctly translated to the shifted band.
  ElementType& at(size_t row, size_t col);
  const ElementType& read_at(size_t row, size_t col) const;
};

template<typename ElementType>
Matrix<ElementType>::Matrix(size_t rows, size_t columns)
  : rows_(rows), columns_(columns)
{
  data_.resize(rows_ * columns_);
}

template<typename ElementType>
size_t Matrix<ElementType>::get_rows() const {
  return rows_;
}

template<typename ElementType>
size_t Matrix<ElementType>::get_columns() const {
  return columns_;
}

template<typename ElementType>
ElementType& Matrix<ElementType>::at(size_t row, size_t col) {
  // NOTE: Using at() for checking bounds.
  // TODO: There is a max original x-coordinate that can be addressed - maybe
  //       should be verified here.
  if (col >= columns_ || col < 0) {
    throw std::out_of_range ("Matrix<ElementType>::at() : col is out of range, col accessed = " + std::to_string(col));
  }
  if (row >= rows_) {
    throw std::out_of_range ("Matrix<ElementType>::at() : row is out of range, row accessed = " + std::to_string(row));
  }
  return data_[row * columns_ + col];
}

template<typename ElementType>
const ElementType& Matrix<ElementType>::read_at(size_t row, size_t col) const {
  if (col >= columns_ || col < 0) {
    throw std::out_of_range ("Matrix<ElementType>::read_at() : col is out of range, col accessed = " + std::to_string(col));
  }
  if (row >= rows_) {
    throw std::out_of_range ("Matrix<ElementType>::read_at() : row is out of range, row accessed = " + std::to_string(row));
  }
  // if (std::isnan(data_[row * columns_ + col])) {
  //   std::cout << "Matrix<ElementType>::read_at() : reading NaN." << std::endl;
  //   // throw std::invalid_argument ("Matrix<ElementType>::read_at() : reading NaN.");
  // }
  return data_[row * columns_ + col];
}

template<typename ElementType>
void Matrix<ElementType>::fill_with(ElementType value) {
  std::fill(data_.begin(), data_.end(), value);
}

template<typename ElementType>
BandMatrix<ElementType>::BandMatrix(size_t rows, size_t band_width)
  : Matrix<ElementType>::Matrix(rows, 2 * band_width + 1), band_width_(band_width) {}

// TODO: The current implementation of the following two methods causes two
//       method calls for accessing the cells.
//       IDEA: Implement access directly without calling methods of the base
//             class. Or read about inlining.

template<typename ElementType>
size_t BandMatrix<ElementType>::get_band_width() const {
  return band_width_;
}

template<typename ElementType>
ElementType& BandMatrix<ElementType>::at(size_t row, size_t col) {
  return Matrix<ElementType>::at(row, col + band_width_ - row);
}

template<typename ElementType>
const ElementType& BandMatrix<ElementType>::read_at(size_t row, size_t col) const {
  return Matrix<ElementType>::read_at(row, col + band_width_ - row);
}

} // namespace data_structures

