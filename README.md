# General

This section summarizes some general rules for this repository. We encourage
everybody who works on the tree similarity framework to stick to these rules.

* This `README.md` contains every piece of important information regarding the
tree similarity framework, e.g., the building process, the coding style, current
issues, et cetera.
* All issues/todos are summarized at the end of this `README.md`. If an issue is
fixed, please include some sort of issue description in the commit message, i.e.,
**Fix issue 3. (All files)**, and remove the corresponding issue/todo from this
`README.md`.

# Coding Style

See [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
as well as the exceptions in the following section.

## Exceptions to the Google C++ Style Guide

* We declare AND define all functions in the `.h` files. This is due to the fact
that template functions/classes are used in almost every `.h` file.

# Issues / TODO

## All files
1. Would a directory structure be useful? (I - Daniel - don't think so, since we
only keep .h files).
2. CMake instead of current hand-crafted Makefile.
3. Check reference/pointer usage (avoid copying as much as possible).
4. Check if memory leaks are resolved (and if not, resolve them).
5. Include check target in Makefile, to automatically invoke
valgrind/cppcheck/splint.
6. Apply Google's C++ style guide rules.

## `node.h`
1. Check if node class can be shortened (should be as short as possible to minimize
the effort of users when creating.

## `zhang_shasha.h`
1. Documentation (check for completeness).

## `string_edit_distance.h`
1. Documentation (check for completeness).

## `upper_bound.h`
1. Template function instead of a class? (like in zhang_shasha.h).