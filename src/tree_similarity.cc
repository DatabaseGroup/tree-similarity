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

/// \file tree_similarity.cc
///
/// \details
/// Entry point of the tree similarity command-line interface.

#include "tree_similarity.h"

#include <iostream>

template<class _Label> using CostModel = cost_model::UnitCostModel<_Label>;

int main(int argc, char** argv) {
    // TODO: using Parser = ;
    using Label = label::StringLabel;
    

    node::Node<Label> root(Label(std::string("abc")));
    node::Node<Label> root2(Label(std::string("abcd")));
    node::Node<Label> root3(Label(std::string("abc")));

    CostModel<Label> cost_model;

    std::cout << cost_model.ren(root, root2) << std::endl;
    std::cout << cost_model.ren(root, root3) << std::endl;
    std::cout << cost_model.ren(root2, root) << std::endl;
    std::cout << cost_model.ren(root2, root3) << std::endl;
    std::cout << cost_model.ren(root3, root) << std::endl;
    std::cout << cost_model.ren(root3, root2) << std::endl;

    return 0;
}