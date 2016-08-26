#ifndef ARRAY_2D_H
#define ARRAY_2D_H

#include <memory>
#include <algorithm>

/**
 * Encapsulates a two dimensional array of a given type (generic).
 * Main reason: it gives better performance than the standard _type** approach
 * due to its allocation pattern. In a _type** array each row in the matrix is
 * a separately allocated dynamic array on the heap - O(n) in general. Same goes
 * for the deallocation because one has to iterate through all elements again.
 * Moreover, separate allocations tend to live in discontinuous chunks of memory.
 * As a result, lots of cache misses happen, in general. This slows down the
 * actual access.
 */

namespace data_structures {

template<typename Type>
class Array2D {
private:
  // consecutive allocated memory: rows * columns
  size_t rows_;
  size_t columns_;
  // consecutive allocated row containing the array elements
  Type* data_;

public:
  // Constructor(s)
  Array2D(size_t rows, size_t columns);
  Array2D(const Array2D& other);

  // Destructor
  ~Array2D();

  // Get number of rows
  size_t get_rows() const;

  // Get number of columns
  size_t get_columns() const;

  // Enable access as usual using the [][] notation.
  //
  // Params:  row the row to be accessed
  //
  // Return:  A pointer to the beginning of the row to be accessed
  Type* operator[](size_t row);
};

template<typename Type>
Array2D<Type>::Array2D(size_t rows, size_t columns)
  : rows_(rows), columns_(columns), data_(nullptr)
{
  // allocate array and initialize to zero
  data_ = new Type[rows_ * columns_] { };
}

template<typename Type>
Array2D<Type>::Array2D(const Array2D& other)
  : Array2D(other.get_rows(), other.get_columns())
{
  std::copy(other.data, other.data + (rows_ * columns_), data_);
}

template<typename Type>
Array2D<Type>::~Array2D() {
  // deallocate array
  delete[] data_;
}

template<typename Type>
size_t Array2D<Type>::get_rows() const {
  return rows_;
}

template<typename Type>
size_t Array2D<Type>::get_columns() const {
  return columns_;
}

template<typename Type>
Type* Array2D<Type>::operator[](size_t row) {
  return data_ + row * columns_;
}

} // namespace data_structures

#endif // ARRAY_2D_H