#pragma once

#include <string>
#include <limits>
#include <cmath>
#include <unordered_map>
#include "matrix.h"
#include "join_result_element.h"

namespace common {

/// Convert vector of ResultElement to its string representation.
/**
 * Sorts the vector in case the result elements don't come in the input order.
 * \param v Vector of JoinResultElement.
 * \return String representation of v.
 */
std::string vector_to_string(std::vector<join::JoinResultElement>& v) {
  // Sort the vector using lambda function implemented here.
  std::sort(v.begin(), v.end(), [](const join::JoinResultElement& a,
                                   const join::JoinResultElement& b){
    if (a.tree_id_1 < b.tree_id_1) {
      return true;
    } else if (a.tree_id_1 == b.tree_id_1) {
      if (a.tree_id_2 < b.tree_id_2) {
        return true;
      } else if (a.tree_id_2 == b.tree_id_2) {
        if (a.ted_value < b.ted_value) {
          return true;
        }
      }
    }
    return false;
  });
  // Convert vector to string.
  std::string s;
  for (auto e : v) {
    s += "{" + std::to_string(e.tree_id_1) + "," + std::to_string(e.tree_id_2)
        + "," + std::to_string(std::lround(e.ted_value)) + "},";
  }
  s.pop_back(); // Delete the last comma.
  return s;
}

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

/// Convert vector of pairs of int values to its string representation.
/**
 * Specialized vector_to_string.
 * \param v Vector of values.
 * \return String representation of v.
 */
const std::string vector_to_string(const std::vector<std::pair<int, int>>& v) {
  std::string s("{");
  for (auto e : v) {
    s += "(" + std::to_string(e.first) + "," + std::to_string(e.second) + "),";
  }
  if (s.size() > 1) s.pop_back();
  s += "}";
  return s;
}

/// Convert vector of vectors of values to its string representation.
/**
 * Specialized vector_to_string.
 * \param vv Vector of vectors of values.
 * \return String representation of vv.
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

/// Convert a matrix of double values to its string representation.
/**
 * \param m Matrix of double values.
 * \return String representation of m.
 */
const std::string matrix_to_string(const data_structures::Matrix<double>& m) {
  std::string s("");
  for (unsigned int x = 0; x < m.get_rows(); ++x){
    for (unsigned int y = 0; y < m.get_columns(); ++y){
      double e = m.read_at(x, y);
      if (e == std::numeric_limits<double>::infinity()) {
        s += "@";
      } else {
        std::string e_string = std::to_string(e);
        std::string e_string_int = e_string.substr(0, e_string.find("."));
        s += e_string_int;
      }
    }
    s += "\n";
  }
  s.pop_back();
  return s;
}

/// Convert a matrix of double values to its string representation.
/**
 * \param m Matrix of double values.
 * \return String representation of m.
 */
const std::string format_matrix_to_string(const data_structures::Matrix<double>& m) {
  std::string s("");
  for (unsigned int x = 0; x < m.get_rows(); ++x){
    for (unsigned int y = 0; y < m.get_columns(); ++y){
      double e = m.read_at(x, y);
      if (e == std::numeric_limits<double>::infinity()) {
        s += "  @";
      } else if (std::isnan(e)) {
        s += "  -";
      } else {
        std::string e_string = std::to_string(e);
        std::string e_string_int = e_string.substr(0, e_string.find("."));
        if (e_string_int.length() == 1) {
          s += "  " + e_string_int;
        }
        else if (e_string_int.length() == 2) {
          s += " " + e_string_int;
        } else {
          s += e_string_int;
        }
      }
    }
    s += "\n";
  }
  s.pop_back();
  return s;
}

/// Convert inverted list of label ids to its string representation.
/**
 * The string representation lists the elements in ascending order of the keys.
 *
 * \param m A hash map of (int, std::vector<int>) pairs.
 * \return String representation of m.
 */
const std::string map_to_string(const std::unordered_map<int, std::vector<int>>& m) {
  // Sort the elements on keys.
  std::vector<std::pair<int, std::vector<int>>> sorted_m(m.begin(), m.end());
  std::sort(sorted_m.begin(), sorted_m.end());
  // Compose the string.
  std::string s("{");
  for (auto& p : sorted_m) {
    s += "{" + std::to_string(p.first) + ":";
    for (auto e : p.second) {
      s += std::to_string(e) + ",";
    }
    s.pop_back();
    s += "}";
  }
  s += "}";
  return s;
}

}
