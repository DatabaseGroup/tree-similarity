# clean-doc-msc-kocher

This branch is supposed to replace the master branch at some point.

## Purpose

1. Clean up the repository (from the state of the `xml_parser` branch, i.e., the
final state of msc-kocher).
2. Add a meaningful documentation to all parts of the codebase in the [https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html](Doxygen format).
3. Refactor parts of the codebase if necessary.
4. Enforce (and probably extend) the coding policy/style described on the `master`
branch and create a [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
configuration to simplify enforcing the coding style for future collaborators.
5. Every pushed code is reviewed by a second person, or there is a discussion about it beforehand.

This branch will **not** include the work of bsc-auinger as Tobias currently tries
to reimplement his part of the codebase (see `visualization` branch). This part
of the code will be added/merged when it is finalized.

## User-Workflow

1. If necessary, implement custom Label class and/or custom CostModel class.
2. If necessary, define the grammar to be used in the parsing process and generate Parser class using [http://www.antlr.org/](ANTLR).
3. Modify the types of `Parser`, `Label`, and `CostModel` inside `tree_similarity.cc` if you do not want to use the default types.
4. Call `make` to (re-)build the framework.
