# Prerequisites

* A C++ compiler
* `CMake` (version >= 2.8)

# Building Process

~~~
cmake .
make
~~~

# Execution

Of course, the project first has to be built.

The following target can be used to execute the binary using
a prespecified test input.

~~~
make execute
~~~

Another option is to execute the binary directly from the command-line. The trees
have to be supplied using the brackets notation.

~~~
./tree_similarity "<first-tree>" "<second-tree>"
~~~