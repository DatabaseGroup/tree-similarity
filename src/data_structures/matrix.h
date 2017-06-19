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

namespace data_structures {

template<typename ElementType>
class Matrix {
// Member variables.
private:
  // consecutive allocated memory: rows * columns
  size_t rows_;
  size_t columns_;
  // consecutive allocated row containing the array elements
  ElementType* data_;
// Member functions.
public:
  /// Constructor(s).
  Matrix(size_t rows, size_t columns);
  Matrix(const Matrix& other);
  /// Destructor.
  ~Matrix();
  /// Returns the number of rows.
  size_t get_rows() const;
  /// Returns the number of columns.
  size_t get_columns() const;
  /// Enable access as usual using the [][] notation.
  ///
  /// \param row The row to be accessed.
  ///
  /// \return A pointer to the beginning of the row to be accessed.
  ElementType* operator[](size_t row);
};

template<typename ElementType>
Matrix<ElementType>::Matrix(size_t rows, size_t columns)
  : rows_(rows), columns_(columns), data_(nullptr)
{
  // allocate array and initialize to zero
  data_ = new ElementType[rows_ * columns_] { };
}

// template<typename ElementType>
// Matrix<ElementType>::Matrix(const Matrix& other)
//   : Matrix(other.get_rows(), other.get_columns())
// {
//   std::copy(other.data, other.data + (rows_ * columns_), data_);
// }

template<typename ElementType>
Matrix<ElementType>::~Matrix() {
  delete[] data_;
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
ElementType* Matrix<ElementType>::operator[](size_t row) {
  return data_ + row * columns_;
}

} // namespace data_structures

#endif // MATRIX_H
