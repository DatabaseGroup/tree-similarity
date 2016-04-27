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

# Coding Style

See [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
as well as the exceptions in the following section.

## Exceptions to the Google C++ Style Guide

* We declare AND define all functions in the `.h` files. This is due to the fact
that template functions/classes are used in almost every `.h` file.