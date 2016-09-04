This file is supposed to summarize all important information about the
`Tree Similarity` project.

At least to some extend, this is a **`MUST-READ`** for
anybody using the framework (e.g., the [building process](#building-process)) or
actively developing it (e.g., the [coding style](#coding-style)). The time reading
this document is definitely well spent since it reduces the effort for everybody
involved (just imagine one would have to rewrite for instance 2'000 poorly
written lines of code - *nobody wants to do this*). Hence, we encourage everyone
to write code to the best of his/her knowledge (regarding this document).

# Table of Contents

- [General](#general)
- [Description](#description)
- [Building Process](#building-process)
    - [Prerequisites](#prerequisites)
        - [Remarks](#remarks)
            - [Installing `cppcheck` from Source](#installing-cppcheck-from-source-optional)
    - [Compilation](#compilation)
    - [Execution](#execution)
- [Coding Style](#coding-style)
    - [Naming](#naming)
        - [Exceptions/Additions](#exceptionsadditions)
    - [Programming Style](#programming-style)
- [References](#references)
    

# General

This section summarizes some general rules for this repository. We encourage
everybody who works on the tree similarity framework to stick to these rules.

* This `README.md` contains every piece of important information regarding the
tree similarity framework, e.g., the building process, the coding style, et cetera.
* If an issue is fixed, please include some sort of issue description in the
commit message, e.g., **Fix issue #3.2**, and mark the corresponding item in the
issue as completed (see the
[`Issues`](https://frosch.cosy.sbg.ac.at/mpawlik/tree-similarity/issues) section
on the left-hand navigation).
* If you find any bugs/performance bottlenecks/..., either open a completely new
issue or add an item to an existing issue (see the
[`Issues`](https://frosch.cosy.sbg.ac.at/mpawlik/tree-similarity/issues) section
on the left-hand navigation). In either case, please add a meaningful description.
The description should contain enough information so that any person is able to
resolve it (and mark it as completed).
* In general, write meaningful commit message. One possible rule would be to
complete the following sentence with your commit message:

    `When applied, this commit will: <commit-message>`

    For instance, `When applied, this commit will Add links to CMake FAQs`.
* Use `make check` heavily. It runs several checks (utilizing `valgrind` and
`cppcheck`, as of yet). However, if one of the checks reports some error, resolve
them before continue writing new code (and potentially introducing additional
errors) or make sure that it is not necessary to resolve them due to some
rational reason (e.g., `The scope of the variable 'x' can be reduced` may be
ignored if the variable scope is chosen on purpose).

# Description

This part describes the framework internals. After reading this, one should understand how the framework organizes trees and should also be able to implement additional algorithm within the framework. If anything is unclear, please open an issue so that we can update this part of the README accordingly.

## Trees

A tree is represented in a very traditional way. There exists a `Node` class which provides some basic tree functionality, e.g. get the size of the subtree rooted at the node or get all/a specific children/child of the node (see `src/nodes/node.h`). The `Node` class is templated and takes one template paremeter: the data of the node. The idea is, that the data of a node may be anything as long as this *anything* provides a corresponding cost model. An example is the `StringNodeData` class (see `src/nodes/string_node_data.h`). This class represents a string label, so every node of a tree would, in this case, have a member of type `StringNodeData` inside. Thus, the `Node` class could be seen as kind of wrapper class around the data, providing the tree functionality. The advantage of this approach should now be obvious: if a user wants a node to hold other data, e.g., a vector of vector of strings, the tree functionality is still there and the user does not have to care about this but just his node data class. The other thing the user has to take care of is the cost model. The cost model is based upon the node data class because, typically, it utilizes the node data class to get the costs of renaming/deleting/inserting a node. In the following example, the node data class holds an integer and the corresponding cost model uses the `get_id()` member to compare the node ids (includes are omitted).

```c++
namespace nodes {

class IntNodeData {
private:
  int id_;

public:
  IntNodeData(int id = 0);

  int get_id() const;

  bool operator<(const IntNodeData& other) const;
  bool operator==(const IntNodeData& other) const;
};

IntNodeData::IntNodeData(int id) : id_(id) { }

int IntNodeData::get_id() const {
  return id_;
}

bool IntNodeData::operator<(const IntNodeData& other) const {
  return (id_ < other.get_id());
}

bool IntNodeData::operator==(const IntNodeData& other) const {
  return (id_ == other.get_id());
}

template<class _NodeData = IntNodeData>
struct IntCosts : public Costs<_NodeData> {
  int ren(_NodeData* node_data1, _NodeData* node_data2);
  int del(_NodeData* node_data);
  int ins(_NodeData* node_data);
};

template<class _NodeData>
int StringCosts<_NodeData>::ren(_NodeData* node_data1, _NodeData* node_data2) {
  if (node_data1->get_id() == node_data2->get_id()) {
    return 0;
  }

  return ((del(node_data1) + ins(node_data2)) / 2);
}

template<class _NodeData>
int StringCosts<_NodeData>::del(_NodeData* node_data) {
  return 2;
}

template<class _NodeData>
int StringCosts<_NodeData>::ins(_NodeData* node_data) {
  return 3;
}

}
```

All the algorithms are then called by providing the node data and the costs type accordingly. The following snippet shows the call of the Zhang & Shasha algorithm as an example (the construction of `tree1` and `tree2` are omitted, see `src/tree_similarity.cc` for examples).

```c++
zhang_shasha::compute_zhang_shasha<nodes::IntNodeData, nodes::IntCosts<nodes::IntNodeData>>(tree1, tree2);
```

The above call tells the algorithm to use the `IntNodeData` type as node data and use the `IntCosts` of `IntNodeData` to compute the rename/deletion/insertion costs when they are needed.

Please check out the `Node` class to see all the tree functionality provided (`src/nodes/node.h`).

## HowTo: Implement New Algorithms

The following steps are mandatory if one wants to contribute a new algorithm to the framework:

1. Create a new file (of course).
2. Add the file to the list starting at line 19 of the `src/CMakeLists.txt` file.
3. Add the include in the `src/tree_similarity.h` file.
4. Add the obvious parts to the file (header guard, includes you need).
5. Add the includes of `src/nodes/node.h` and `src/nodes/string_node_data.h` (used a default template parameter).
6. Add an existing or introduce a new namespace in which the function resides.
7. Add the template signature and your algorithms signature.

After these 7 steps, one should be able to compile it using `CMake` (as described afterwards). The file should look similar to the following code snippet (the example function takes a vector of nodes and the cost model as input, and returns a `double` value):
 
```c++
// content of src/my_awesome_algorithm/my_awesome_algorithm.h
#ifndef MY_AWESOME_ALGORITHM_H
#define MY_AWESOME_ALGORITHM_H

#include <vector>

#include "../nodes/node.h"
#include "../nodes/string_node_data.h"

// one could also use an existing namespace here, e.g., zhang_shasha, tasm, ...
namespace my_awesome_algorithm {

// StringNodeData and StringCosts are the defaults if now template parameters are given
template<class _NodeData = nodes::StringNodeData, class _Costs = nodes::Costs<_NodeData>>
double my_awesome_algorithm_func(std::vector<nodes::Node<_NodeData>> trees,
  _Costs costs = new _Costs())
{
  // implement your awesome function here!
  return 0.0;
}

}

#endif // MY_AWESOME_ALGORITHM
```

Now, one can implement the functionality of the function and then call it in the `src/tree_similarity.cc` file (construction of `trees` and instantiation of `costs` omitted):

```c++
// uses the default template parameters
double result = my_awesome_algorithm::my_awesome_algorithm_func(trees, costs);
```

# Building Process

The following commands are meant to be executed from the root directory of this
repository, i.e. `tree_similarity/`.

## Prerequisites

- A C++ compiler (preferably [`clang`](#references) in version >=
3.4.0)
- [`CMake`](#references) in version >= 2.8
- [`valgrind`](#references) in version >= 3.10.1 (**not obligatory** but required if one wants to use
`make check-dynamic`/`make check`)
- [`cppcheck`](#references) in version >= 1.74 (**not obligatory** but required if one wants to use
`make check-static`/`make check`)

### Remarks

#### Installing [`cppcheck`](#references) from Source [optional]

If you have set an SSH key for your GitHub account, start by using

~~~
git clone git@github.com:danmar/cppcheck.git
~~~

If you have not set such an SSH key, start by using

~~~
git clone https://github.com/danmar/cppcheck.git
~~~

The remaining operations are supposed to work independent of whether an SSH key
is set or not.

~~~
cd cppcheck
make SRCDIR=build CFGDIR=/usr/bin/cfg HAVE_RULES=yes
sudo make install CFGDIR=/usr/bin/cfg
~~~

The path supplied to `CFGDIR` can be adjusted to a custom path (e.g.,
`~/cppcheck-cfg`). However, the path has to be consistent for `make` and
`make install`.

## Compilation

The following commands perform an out-of-source building (see the
[CMake FAQs](https://cmake.org/Wiki/CMake_FAQ#Out-of-source_build_trees)).
It has to be issued in the root directory of the repository.

~~~
mkdir build && cd build/ && CXX=clang++ cmake ../src && make
~~~

The resulting directory (`build`) then contains all the files generated by CMake.

## Execution

The following target can be used to execute the binary using a prespecified test
input.

~~~
cd build/ && make execute
~~~

Another option is to execute the binary directly from the command-line. The trees
have to be supplied using the brackets notation.

~~~
cd build/ && ./tree_similarity '<first-tree>' '<second-tree>'
~~~

# Coding Style

Since the whole project is still under development, please be aware that also
the coding style may be extended (by introducing rules overruling one or more
rules in the
[Google C++ Style Guide](#references)).
Therefore, please check if the coding style has been updated since your last
contribution. For the sake of simplicity, it would be very nice to notify all
active developers if the coding style was updated (if done consistently, this
removes the necessity to check the `README.md` regularly).

## Naming

Regarding naming, see
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html#Naming).
If there are some exceptions/additions, they are specified in the following
subsection. However, it is important to mention that these exceptions/additions
**overrule** conflicting/inconsistent rules from Google's C++ style guide.

### Exceptions/Additions

- **Abbreviations**: We consider the following abbreviations *well-known* (hence,
    one is allowed to use them):
    - `temp`, `tmp`, `str`, `it`, `min`, `max`
    - `ren`, `ins`, `del` *(rename, insert, delete)*
    - `ed`, `ted`, `sed` *(edit distance, tree edit distance, string edit distance)*
    - `tasm` *(top-k approximate subtree matching)*
    - All abbreviations used in the [Zhang & Shasha algorithm](#references) (e.g.,
        `lml`, `td`, `fd`, `l1`, `l2`, `kr1`,`kr2`, ...)
- **Function Names**: We use the *lower case with underscore* notation for **all**
    functions/methods, i.e., `this_is_a_function_name` - regardless of their
    scope and cost.

## Programming Style

Regarding the programming style (i.e., how to use C++11 features), we do not fully
comply to
[Google's C++ Style Guide](#references). This
is due to its restrictiveness in some aspects (e.g., disallowing exceptions
completely). We summarize some aspects regarding the programming style in the
following listing. Also here it holds that the rules in this listing **overrules**
conflicting/inconsistent rules from Google's style guide. However, if any rules
is inconsistent/unclear, please just ask and we will discuss it.

1. **We *declare AND define* all functions/classes in the `.h` files.**

    This is due to the fact that template functions/classes are used in almost
    every `.h` file. Nevertheless we separate definition and declaration as
    much as possible. One possible way to do this is shown in the following
    example code (*by now, we do it like this*):

    ```c++    
    // declaration
    template<typename _Type>
    class TestContainer {
    private:
      _Type* data_;
    
    public:
      TestContainer (_Type* data);
      ~TestContainer ();
    
      _Type* get_data () const;
      void set_data (_Type* data);
    };
            
    // definition
    template<typename _Type>
    TestContainer<_Type>::TestContainer (_Type* data) : data_(data) {};
    
    template<typename _Type>
    TestContainer<_Type>::~TestContainer () {
        delete data_;
    }
    
    template<typename _Type>
    _Type* TestContainer<_Type>::get_data () const {
      return data_;
    }
    
    template<typename _Type>
    void TestContainer<_Type>::set_data (_Type* data) {
      data_ = data;
    }
    ```

    Another approach would be to keep declaration and definition in separate
    files, e.g. `my_class.h` and `my_class_impl.h`. Then the last 9 lines of the
    above example would be located in `my_class_impl.h`. The rest would remain
    in `my_class.h`. One then needs to include `my_class_impl.h` after the
    declarative part.

2. For template parameters, we use a `_` (underscore) prefix and then the usual type naming style (e.g. `_MyTemplType` - see the example above).
    
3. **We *always* use the *safer* method if there are multiple methods achieving
the same result.**

    A good example is the
[`std::vector`](http://www.cplusplus.com/reference/vector/) container for which
there are multiple possibilities to access an element (i.e., the
[`at`](http://www.cplusplus.com/reference/vector/vector/at/) member or the
[`[]`](http://www.cplusplus.com/reference/vector/vector/operator%5B%5D/)
operator). In this example, we *always* use the
[`at`](http://www.cplusplus.com/reference/vector/vector/at/) member. The main
reason is that
[`at`](http://www.cplusplus.com/reference/vector/vector/at/) performs a range
check, whereas the 
[`[]`](http://www.cplusplus.com/reference/vector/vector/operator%5B%5D/) operator
does not.

4. **We use exceptions in order to make the library more error prone. Further, we
do not allow any call to a function which terminates the library (e.g., `exit`).**

    This is due to the fact that we cannot anticipate if a termination is desired
in case of, e.g., an exception (the program which uses the library should catch
the exception(s) and decide whether it wants to terminate or not). The only point
in the code where a call to the `exit` function is allowed, is - of course - in
the `tree_similarity.cc`.

5. **Function/Method declaration/definition if they do not fit on a single line:**

    We fit as many arguments into a single line as possible. Further, if there is a
line break in a function/method definition, we put the curly brace into the next
line to emphasize the beginning of the implementational part. The same rule is
applied if there is a line break in loops, if statements, et cetera. In the
following, the example from [Google's C++ Style Guide](#references) is adapted
accordingly.

    ```c++
    ReturnType ClassName::ReallyLongFunctionName(Type par_name1, Type par_name2,
      Type par_name3) // 2 space indentation
    {
      DoSomething();
      ...
    }

    ReturnType LongClassName::ReallyReallyReallyLongFunctionName(
      Type par_name1, Type par_name2, Type par_name3) // 2 space indent
    {
      DoSomething();  // 2 space indent
      ...
    }
    ```

# References

[1] [C++ Reference [1]](http://www.cplusplus.com/reference/)

[2] [C++ Reference [2]](http://en.cppreference.com/w/cpp)

[3] [`clang`](http://clang.llvm.org/)

[4] [`CMake`](https://cmake.org/)

[5] [`valgrind`](http://valgrind.org/)

[6] [`cppcheck`](https://github.com/danmar/cppcheck)

[7] [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

[8] [Zhang & Shasha algorithm](http://www.inf.unibz.it/dis/teaching/SS/ss05-handout-1x1.pdf)
