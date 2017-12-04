#include <string>

namespace common {

/// Convert vector of int values to its string representation.
///
/// \param v Vector of int values.
/// \return String representation of v.
const std::string vector_to_string(const std::vector<int>& v) {
  std::string s("{");
  for (auto e : v) {
    s += std::to_string(e) + ",";
  }
  s.pop_back();
  s += "}";
  return s;
}

}
