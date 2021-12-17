// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Thomas Huetter.
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

/// \file label/label_dictionary.h
///
/// \details
/// This is an implementation of a dictionary for labels. The purpose is to use
/// it to assign complex labels uniqe integer ids. This makes it more efficient
/// for equality comparison, indexing, and hashing. 

#pragma once

#include <unordered_map>

namespace label {

/// \class LabelDictionary
///
/// \details
/// This is an implementation of a dictionary for labels. The purpose is to use
/// it to assign complex labels uniqe integer ids. This makes it more efficient
/// for equality comparison, indexing, and hashing.
///
/// NOTE: Once using LabelDictionary, one has to be careful about with the
///       cost model.
///
template <class Label>
class LabelDictionary {
public:
  /// Constructor.
  LabelDictionary();
  /// Tries to insert l into the dictionary. If l is already in dictionary, its
  /// id is returned. The old label and its id are not overwritten. If l is not
  /// in dictionary, it is inserted, and its id is returned.
  ///
  /// \param l A label to insert.
  /// \return The id of the given label.
  int insert(const Label& l);
  /// Returns the number of labels.
  ///
  /// \return The number of labels.
  int get_number_of_labels();
  /// Returns the label that is mapped to a certain 
  ///
  /// \param id A label identifier to lookup.
  /// \return The according label to a given label identifier.
  const Label& get(const int id) const;
  /// Removes all elements from the dictionary.
  /// Resets labels_count_ to 0;
  void clear();
  
  int size() const;
private:
  /// A hash map that maps labels to unique integer identifiers.
  /// TODO: Write requirements for Label and update them in string_label.
  std::unordered_map<Label, int> label_to_id_dictionary_;
  /// Maps a unique interger identifier to the according label.
  std::vector<Label> id_to_label_dictionary_;
  /// The number of labels in dictionary. It is also used to assign ids to new
  /// inserted labels.
  int labels_count_;
};

// Implementation details
#include "label_dictionary_impl.h"

} // namespace label
