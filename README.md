This file is supposed to summarize all important information about the
`Tree Similarity` project.

At least to some extend, this is a **`MUST-READ`** for
anybody using the framework (e.g., the [building process](#building-process)) or
actively developing it (e.g., the [coding style](#coding-style)). The time reading
this document is definitely well spent since it reduces the effort for everybody
involved (just imagine one would have to reformat for instance 2'000 poorly
written lines of code - *nobody wants to do this*). Hence, we encourage everyone
to write code to the best of his/her knowledge (regarding this document).

# Table of Contents

- [General](#general)
- [Building Process](#building-process)
    - [Prerequisites](#prerequisites)
        - [Remarks](#remarks)
            - [Installing `cppcheck` from Source](#installing-cppcheck-from-source)
    - [Compilation](#compilation)
    - [Execution](#execution)
- [Coding Style](#coding-style)
    - [Naming](#naming)
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

# Building Process

The following commands are meant to be executed from the root directory of this
repository, i.e. `tree_similarity/`.

## Prerequisites

* A C++ compiler (preferably [`clang`](http://clang.llvm.org/) in version >=
3.4.0)
* [`CMake`](https://cmake.org/) in version >= 2.8
* [`valgrind`](http://valgrind.org/) in version >= 3.10.1 (to be able to use
`make check-dynamic` or `make check`)
* [`cppcheck`](https://github.com/danmar/cppcheck) in version >= 1.74 (to be able
to use `make check-static` or `make check`)

### Remarks

#### Installing [`cppcheck`](https://github.com/danmar/cppcheck) from Source

~~~
git clone git@github.com:danmar/cppcheck.git && cd cppcheck
make SRCDIR=build CFGDIR=/usr/bin/cfg HAVE_RULES=yes
sudo make install CFGDIR=/usr/bin/cfg
~~~

The path supplied to `CFGDIR` can be adjusted to a custom path (e.g.,
`~/cppcheck-cfg`). However, the path has to be consistent for `make` and
`make install`.

## Compilation

The following commands perform an out-of-source building (see the
[CMake FAQs](https://cmake.org/Wiki/CMake_FAQ#Out-of-source_build_trees)).

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
[Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)).
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

## Programming Style

Regarding the programming style (i.e., how to use C++11 features), we do not fully
comply to
[Google's C++ Style Guide](https://google.github.io/styleguide/cppguide.html). This
is due to its restrictiveness in some aspects (e.g., disallowing exceptions
completely). We summarize some aspects regarding the programming style in the
following listing. Also here it holds that the rules in this listing **overrules**
conflicting/inconsistent rules from Google's style guide. However, if any rules
is inconsistent/unclear, please just ask and we will discuss it.

1. We **declare AND define** all functions/classes in the `.h` files. This is due
to the fact that template functions/classes are used in almost every `.h` file.
2. We **always** use the
[`at`](http://www.cplusplus.com/reference/vector/vector/at/) member of the
[`std::vector`](http://www.cplusplus.com/reference/vector/) container when we
want to access an element. As a consequence, we try to avoid the
[`[]`](http://www.cplusplus.com/reference/vector/vector/operator%5B%5D/) operator.

    The main reason is that
[`at`](http://www.cplusplus.com/reference/vector/vector/at/) performs a range
check, whereas the 
[`[]`](http://www.cplusplus.com/reference/vector/vector/operator%5B%5D/) operator
does not.

# References

* [C++ Reference [1]](http://www.cplusplus.com/reference/)
* [C++ Reference [2]](http://en.cppreference.com/w/cpp)