# General

This section summarizes some general rules for this repository. We encourage
everybody who works on the tree similarity framework to stick to these rules.

* This `README.md` contains every piece of important information regarding the
tree similarity framework, e.g., the building process, the coding style, et cetera.
* If an issue is fixed, please include some sort of issue description in the
commit message, e.g., **Fix issue #3.2**, and mark the corresponding item in the
issue as completed (see the `Issues` section on the left-hand navigation).
* If you find any bugs/performance bottlenecks/..., either open a completely new
issue or add an item to an existing issue (see the `Issues` section on the
left-hand navigation). In either case, please add a meaningful description. The
description should contain enough information so that any person is able to
resolve it (and mark it as completed).

# Building Process

The following commands are meant to be executed from the root directory of this
repository, i.e. `tree_similarity/`.

## Prerequisites

* A C++ compiler
* `CMake` (version >= 2.8)

## Compilation

The following commands perform an out-of-source building (see the
[CMake FAQs](https://cmake.org/Wiki/CMake_FAQ#Out-of-source_build_trees)).

~~~
mkdir build && cd build/ && cmake ../src && make
~~~

## Execution

The following target can be used to execute the binary using a prespecified test
input.

~~~
cd build/ && make execute
~~~

Another option is to execute the binary directly from the command-line. The trees
have to be supplied using the brackets notation.

~~~
cd build/ && ./tree_similarity "<first-tree>" "<second-tree>"
~~~

# Coding Style

See [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
as well as the exceptions in the following section.

## Exceptions to the Google C++ Style Guide

* We declare AND define all functions in the `.h` files. This is due to the fact
that template functions/classes are used in almost every `.h` file.
