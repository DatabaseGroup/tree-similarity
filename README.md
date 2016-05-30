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
- [Building Process](#building-process)
    - [Prerequisites](#prerequisites)
        - [Remarks](#remarks)
            - [Installing `cppcheck` from Source](#installing-cppcheck-from-source)
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

# Building Process

The following commands are meant to be executed from the root directory of this
repository, i.e. `tree_similarity/`.

## Prerequisites

- A C++ compiler (preferably [`clang`](#references) in version >=
3.4.0)
- [`CMake`](#references) in version >= 2.8
- [`valgrind`](#references) in version >= 3.10.1 (to be able to use
`make check-dynamic` or `make check`)
- [`cppcheck`](#references) in version >= 1.74 (to be able
to use `make check-static` or `make check`)

### Remarks

#### Installing [`cppcheck`](#references) from Source

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
    - `ren` *(rename)*
    - `ins` *(insert)*
    - `del` *(delete)*
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
    every `.h` file.
    
2. **We *always* use the *safer* method if there are multiple methods achieving
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

3. **We use exceptions in order to make the library more error prone. Further, we
do not allow any call to a function which terminates the library (e.g., `exit`).**

    This is due to the fact that we cannot anticipate if a termination is desired
in case of, e.g., an exception (the program which uses the library should catch
the exception(s) and decide whether it wants to terminate or not).

# References

[1] [C++ Reference [1]](http://www.cplusplus.com/reference/)

[2] [C++ Reference [2]](http://en.cppreference.com/w/cpp)

[3] [`clang`](http://clang.llvm.org/)

[4] [`CMake`](https://cmake.org/)

[5] [`valgrind`](http://valgrind.org/)

[6] [`cppcheck`](https://github.com/danmar/cppcheck)

[7] [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

[8] [Zhang & Shasha algorithm](http://www.inf.unibz.it/dis/teaching/SS/ss05-handout-1x1.pdf)