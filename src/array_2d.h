#ifndef ARRAY_2D_H
#define ARRAY_2D_H

// size_t
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
template<class _type>
class Array2D {
private:
  // consecutive allocated memory: rows * columns
  size_t rows;
  size_t columns;
  // consecutive allocated row containing the array elements
  _type* data;

public:
  // Constructor
  Array2D (size_t rows, size_t columns)
    : rows(rows), columns(columns), data(nullptr)
  {
    // allocate array
    data = new double[rows * columns];
  }

  // Copy constructor
  Array2D (const Array2D& other)
    : Array2D(other.get_rows(), other.get_columns)
  {
    std::copy(other.data, other.data + (rows * columns), data);
  }

  // Destructor
  ~Array2D () {
    // deallocate array
    delete[] data;
  }

  // Get number of rows
  size_t get_rows () const {
    return rows;
  }

  // Get number of columns
  size_t get_columns () const {
    return columns;
  }

  // Enable access as usual using the [][] notation.
  //
  // Params:  row the row to be accessed
  //
  // Return:  A pointer to the beginning of the row to be accessed
  _type* operator[] (size_t row) {
    return data + row * columns;
  }
};

#endif // ARRAY_2D_H
