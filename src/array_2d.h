#ifndef ARRAY_2D_H
#define ARRAY_2D_H

#include <memory>

/**
 * Encapsulates a two dimensional array of a given type (generic).
 * Mainly because it gives better performance than the standard _type** approach
 * due to its allocation pattern. In a _type** array each row in the matrix is
 * a separately allocated dynamic array on the heap - O(n) in general. Same goes
 * for the deallocation because one has to iterate through all elements again.
 * Moreover, separate allocations tend to live in discontinuous chunks of memory.
 * As a result, lots of cache misses happen, in general. This slows down the
 * actual access.
 */
template<class _type>
class array_2d {
private:
  size_t rows;
  size_t columns;
  _type* data;

public:
  array_2d (size_t rows, size_t columns) : rows(rows), columns(columns),
    data(nullptr)
  {
    data = new _type[rows * columns];
  }

  ~array_2d () { delete data; }

  size_t get_rows () const { return rows; }
  size_t get_columns () const { return columns; }

  _type* operator[] (size_t row) { return row * columns + data; }
};

#endif // ARRAY_2D_H
