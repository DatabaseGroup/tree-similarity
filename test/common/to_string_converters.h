#include <string>

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

/// Convert vector of vectors of values to its string representation.
/**
 * \param vv Vector of vectors of values.
 * \return String representation of vvv.
 */
template <typename T>
const std::string vector_of_vectors_to_string(const std::vector<std::vector<T>>& vv) {
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
