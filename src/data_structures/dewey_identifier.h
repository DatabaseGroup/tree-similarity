#ifndef DATA_STRUCTURES_DEWEY_IDENTIFIER_H
#define DATA_STRUCTURES_DEWEY_IDENTIFIER_H

#include <vector>

namespace data_structures {

class DeweyIdentifier {
public:
  std::vector<size_t> id_;

public:
  DeweyIdentifier();
  DeweyIdentifier(const size_t& value);
  DeweyIdentifier(const DeweyIdentifier& other);
  DeweyIdentifier(const DeweyIdentifier& other, const size_t& last_value);
  ~DeweyIdentifier();

  void clear();
  size_t& at(const size_t& level);
  const size_t& at(const size_t& level) const;
  void set(const size_t& value, const size_t& level);
  bool is_prefix(const DeweyIdentifier& other) const;
  bool is_predecessor(const DeweyIdentifier& other) const;
  size_t length() const;
  bool empty() const;

  bool operator<(const DeweyIdentifier& other) const;
  bool operator==(const DeweyIdentifier& other) const;
};

DeweyIdentifier::DeweyIdentifier() { }

DeweyIdentifier::DeweyIdentifier(const size_t& value) {
  if (id_.size() > 0) {
    clear();
  }

  id_.push_back(value);
}

DeweyIdentifier::DeweyIdentifier(const DeweyIdentifier& other) {
  //id_.resize(other.id_.size());
  //std::copy(other.id_.begin(), other.id_.end(), id_.begin());
  id_.insert(id_.end(), other.id_.begin(), other.id_.end());
}

DeweyIdentifier::DeweyIdentifier(const DeweyIdentifier& other,
  const size_t& last_value) : DeweyIdentifier(other)
{
  id_.push_back(last_value);
}

DeweyIdentifier::~DeweyIdentifier() { }

void DeweyIdentifier::clear() {
  id_.clear();
}

size_t& DeweyIdentifier::at(const size_t& level) {
  try {
    return id_.at(level);
  } catch(std::exception& e) {
    throw;
  }
}

const size_t& DeweyIdentifier::at(const size_t& level) const {
  try {
    return id_.at(level);
  } catch(std::exception& e) {
    throw;
  }
}

void DeweyIdentifier::set(const size_t& value, const size_t& level) {
  try {
    id_.at(level) = value;
  } catch(std::exception& e) {
    throw;
  }
}

bool DeweyIdentifier::is_prefix(const DeweyIdentifier& other) const {
  if (id_.size() >= other.id_.size()) {
    return false;
  }

  // id_.size() < other.id_.size()
  std::vector<size_t>::const_iterator tcit, ocit;
  size_t i = 0;
  for ( tcit = id_.cbegin(), ocit = other.id_.cbegin(); tcit != id_.cend();
        ++tcit, ++ocit)
  {
    //std::cout << *tcit << ", " << *ocit << std::endl;
    if (*tcit != *ocit) {
      //std::cout << *tcit << " != " << *ocit << std::endl;
      return false;
    }
  }

  return true;
}

bool DeweyIdentifier::is_predecessor(const DeweyIdentifier& other) const {
  // other is a descendant of this iff did(this) is a prefix of did(other)
  // as a consequence, this is a predecessor of other if this holds
  return is_prefix(other);
}

size_t DeweyIdentifier::length() const {
  return id_.size();
}

bool DeweyIdentifier::empty() const {
  return id_.empty();
}

bool DeweyIdentifier::operator<(const DeweyIdentifier& other) const {
  return (id_ < other.id_);
}

bool DeweyIdentifier::operator==(const DeweyIdentifier& other) const {
  return (id_ == other.id_);
}

} // namespace data_structures

#endif // DATA_STRUCTURES_DEWEY_IDENTIFIER_H