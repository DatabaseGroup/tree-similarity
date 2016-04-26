# General

This section summarizes some general rules for this repository. We encourage
everybody who works on the tree similarity framework to stick to these rules.

* This `README.md` contains every piece of important information regarding the
tree similarity framework, e.g., the building process, the coding style, current
issues, et cetera.
* All issues/todos are summarized at the end of this `README.md`. If one fixes an
issue, please include the issue number in the commit message, i.e.,
**(issue x.y)**, and remove the corresponding issue/todo from this `README.md`.

# Coding Style

See [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)

## Exceptions to the Google C++ Style Guide

* We declare AND define all functions in the `.h` files. This is due to the fact
that template functions/classes are used in almost every `.h` file.

# Issues / TODO

## All files
1.1. Would a directory structure be useful? (I - Daniel - don't think so, since we
only keep .h files).
1.2. CMake instead of current hand-crafted Makefile.
1.3. Check reference/pointer usage (avoid copying as much as possible).
1.4. Check if memory leaks are resolved (and if not, resolve them).
1.5. Include check target in Makefile, to automatically invoke
valgrind/cppcheck/splint.
1.6. Apply Google's C++ style guide rules.

## `node.h`
2.1. Check if node class can be shortened (should be as short as possible to minimize
the effort of users when creating.

## `zhang_shasha.h`
3.1. Documentation (check for completeness).

## `string_edit_distance.h`
4.1. Documentation (check for completeness).

## `upper_bound.h`
5.1. Template function instead of a class? (like in zhang_shasha.h).