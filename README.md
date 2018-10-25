# Tree Similarity Library

**We actively develop this library and things change continuously.**

The goal of this repository is to be the home of all code corresponding to tree similarity. This code base is a starting point for new research ideas and students project. Implementation of new features should be consistent with this code base.

**Currently this README serves everyone, users and contributors.** It is disorganised with an idea that having disorganised and correct content is better than no content. It should be expanded on a regular basis and possibly split into ``README.md`` and ``README_CONTRIBUTOR.md``.

## Licence

All code in this repository is currently under the [MIT licence](https://opensource.org/licenses/MIT). Each source file must contain the licence and the list of copyright holders (contributors to the source).

## Building process

Install [CMake](https://cmake.org/) for building and testing.

Clone the repository. To build the project, execute the following from the root directory (this will build all targets).
```bash
mkdir build
cd build
cmake ..
make
```
Execute ``make test`` to run all tests (currently there are only correctness tests).

In the ``build`` directory you find the binary ``ted`` that executes the algorithms from command line. **Currently there is no help or documentation for this binary.**

## Generating documentation

Install [Doxygen](http://www.stack.nl/~dimitri/doxygen/) for generating the documentation.

Execute the following (in the project's root directory) to generate the documentation.
```bash
doxygen doxygen.config
```
Then, open ``doc/html/index.html`` in your browser.

## Notes on branches (old stuff)

[**TODO:** Tidy up. Preferably modify to describe the current state. See TODOs.]

This branch [**TODO:** Which branch is meant?] will **not** include the work of bsc-auinger as Tobias currently tries
to reimplement his part of the codebase (see `visualization` branch). This part
of the code will be added/merged when it is finalized.

## Notes on user workflow (old stuff)

[**TODO:** Tidy up. Preferably modify to describe the current state. See TODOs.]

1. If necessary, implement custom Label class and/or custom CostModel class.
2. If necessary, define the grammar to be used in the parsing process and generate Parser class using [http://www.antlr.org/](ANTLR).
3. Modify the types of `Parser`, `Label`, and `CostModel` inside `tree_similarity.cc` if you do not want to use the default types.
4. Call `make` to (re-)build the framework.

## TODOs

This is a list of of all TODOs split into a few categories.

### Features.
- **Command line:** make nicer and document.
- **APTED:** Implement. There is some C++ code written by Maxi. Java implementation available on https://github.com/DatabaseGroup/apted.
- **Edit mapping:** Implement with subtree-pairs distance matrix as input such that not only the matrix of Zhang and Shasha can be input. There is C++ code on ``visualisation`` branch by Tobias (review and tests required).

### README.md
- Describe functionalities of the library.
- Explain the input format. Point to format converters.
- Explain how to use the tool: command line interface and output.
- Explain (with examples) how to extend the library with custom labels and cost models.
- Explain how to use the library in an external project (API).
- State how to cite the library.

### README_CONTRIBUTOR.md
- Expand the details of including licence and copyright into source files.
- Describe directory structure and file naming.
- Describe C++ coding guidelines. Point to:
  - C++ Core Guidelines (https://github.com/isocpp/CppCoreGuidelines)
  - Google C++ Style Guide (http://google.github.io/styleguide/cppguide.html)
- List documentation requirements.
- Describe requirements for correctness testing of new algorithms.
  - Include test naming convention.
- Explain how to extend labels and cost models.
- Explain how to extend input formats (create nodes-labels structure from raw input, parsing raw input).
- Describe **Git** workflows (be explicit with the git commands, especially for educational purposes):
  - **Branching.** Collaborators (e.g. PhD and Master students that we trust an that implement specific functionalities). Include info about using private branches, especially for sensitive code (that should not be public once the library goes online; point to (https://24ways.org/2013/keeping-parts-of-your-codebase-private-on-github/ and https://git-scm.com/book/en/v2/Git-Basics-Working-with-Remotes)).
  - **Forking.** Non-collaborators (e.g. Master and Bachelor students that do prototyping).
  - **Committing.** Write requirements for commit messages.
  - **Merging.** Explain the steps, using rebase, and fast-forwarding.

### Old TODOs:
[**TODO:** Tidy up or at least move to other TODO categories.]
- Clean up the repository (from the state of the `xml_parser` branch, i.e., the
final state of msc-kocher).
- Add a meaningful documentation to all parts of the codebase in the [Doxygen format](https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html).
- Refactor parts of the codebase if necessary.
- Enforce (and probably extend) the coding policy/style described on the `master`
branch and create a [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
configuration to simplify enforcing the coding style for future collaborators.
- Every pushed code is reviewed by a second person, or there is a discussion about it beforehand.
