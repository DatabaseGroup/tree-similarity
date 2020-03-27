// The MIT License (MIT)
// Copyright (c) 2017 Mateusz Pawlik, Nikolaus Augsten, and Daniel Kocher.
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

#pragma once

template <class Label>
int UnitCostModel<Label>::ren(const node::Node<Label>& node1,
                              const node::Node<Label>& node2) const {
  if (node1.label() == node2.label()) {
    return 0;
  }
  return 1;
}

template <class Label>
int UnitCostModel<Label>::del(const node::Node<Label>& node) const {
  return 1;
}

template <class Label>
int UnitCostModel<Label>::ins(const node::Node<Label>& node) const {
  return 1;
}

template <class Label>
UnitCostModelLD<Label>::UnitCostModelLD(label::LabelDictionary<Label>& ld) :
    ld_(ld) {}

template <typename Label>
double UnitCostModelLD<Label>::ren(const int label_id_1,
    const int label_id_2) const {
  if (label_id_1 == label_id_2) {
    return 0.0;
  }
  return 1.0;
}

// Argument's name deleted because not used.
template <typename Label>
double UnitCostModelLD<Label>::del(const int) const {
  return 1.0;
}

// Argument's name deleted because not used.
template <typename Label>
double UnitCostModelLD<Label>::ins(const int) const {
  return 1.0;
}
