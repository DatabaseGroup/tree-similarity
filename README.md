# General

This section summarizes some general rules for this repository. We encourage
everybody who works on the tree similarity framework to stick to these rules.

* This `README.md` contains every piece of important information regarding the
tree similarity framework, e.g., the building process, the coding style, current
issues, et cetera.
* If an issue is fixed, please include some sort of issue description in the
commit message, i.e., **Fix issue #3**, and mark the corresponding issue as
resolved (see the `Issues` section on the left-hand navigation).

# Coding Style

See [Google C++ Style Guide](https://google.github.io/styleguide/cppguide.html)
as well as the exceptions in the following section.

## Exceptions to the Google C++ Style Guide

* We declare AND define all functions in the `.h` files. This is due to the fact
that template functions/classes are used in almost every `.h` file.