#include <string>
#include <limits>

namespace common {

/// Convert vector of values to its string representation.
/**
 * \param v Vector of values.
 * \return String representation of v.
 */
template <typename T>
const std::string vector_to_string(const std::vector<T>& v) {
  std::string s("{");
  for (auto e : v) {
    s += std::to_string(e) + ",";
  }
  s.pop_back();
  s += "}";
  return s;
}

/// Convert vector of double values to its string representation.
/**
 * Specialized vector_to_string.
 * \param v Vector of values.
 * \return String representation of v.
 */
const std::string vector_to_string(const std::vector<double>& v) {
  char int_part[2];
  // printf ("[%s] is a string %d chars long\n",buffer,n);
  std::string s("{");
  for (auto e : v) {
    sprintf(int_part, "%.0f", e);
    s += std::string(int_part) + ",";
  }
  s.pop_back();
  s += "}";
  return s;
}

/// Convert vector of vectors of values to its string representation.
/**
 * Specialized vector_to_string.
 * \param vv Vector of vectors of values.
 * \return String representation of vvv.
 */
template <typename T>
const std::string vector_to_string(const std::vector<std::vector<T>>& vv) {
  std::string s("{");
  for (auto v : vv) {
    s += "{";
    for (auto e : v) {
      s += std::to_string(e) + ",";
    }
    if (v.size() > 0) {
      s.pop_back();
    }
    s += "},";
  }
  s.pop_back();
  s += "}";
  return s;
}

}
