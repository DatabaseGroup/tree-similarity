// The MIT License (MIT)
// Copyright (c) 2017 Daniel Kocher, Mateusz Pawlik, and Nikolaus Augsten
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

/// \file node/string_node_data.h
///
/// \details
/// Contains the declaration of the StringNodeData class (represents string data
/// which is associated with a Node). For example, if one wants a tree with
/// strings as labels, one could use this class as a template argument for the
/// Node class to accomplish this.

#ifndef TREE_SIMILARITY_LABEL_STRING_LABEL_H
#define TREE_SIMILARITY_LABEL_STRING_LABEL_H

#include <string>

namespace label {

/// \class StringNodeData
///
/// \details
/// Represents the (satellite) data a node can hold. This class only serves as
/// encapsulation of this data and all operations on this data, nothing else.
/// 
/// \remark
/// A user of this framework may want to create a specialized data class to
/// suffice his/her application area, e.g., holding a sequence of sequences of
/// <needed-type> to encode their data properly. One needs at least to implement
/// the following member functions such that it does not break the framework:
///     - Ctor taking no arguments (or one with default arguments).
///     - inline bool operator<(...) const; // aka the 'less' operator.
///     - inline bool operator==(...) const; // aka the 'equals' operator.
/// The member function to get the member are optional but may be necessary if
/// one wants to use a specialized cost model.
/// For an example of another data class, please see custom_node_data.h and
/// custom_node_data_impl.h, respectively.
class StringLabel {
public:
    //StringLabel(const std::string& label);
    StringLabel(std::string&& label);

    /// Gets members.
    /// @{
    const std::string& label() const;
    /// @}

    /// Operator overloadings.
    /// @{
    bool operator==(const StringLabel& other) const;
    /// @}

private:
    /// The label to be associated with a node.
    std::string label_{};
};

} // namespace label

#endif // TREE_SIMILARITY_LABEL_STRING_LABEL_H