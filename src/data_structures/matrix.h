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


#ifndef MATRIX_H
#define MATRIX_H

#include <memory>
#include <vector>

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
  // TODO: Bounds are not checked, nor exception is caught.
  return data_[row * columns_ + col];
}

} // namespace data_structures

#endif // MATRIX_H
