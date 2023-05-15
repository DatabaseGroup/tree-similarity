# Tree Similarity Library

**We actively develop this library and things change continuously.**

The goal of this repository is to be the home of all code corresponding to tree similarity. This code base is a starting point for new research ideas and students project. Implementation of new features should be consistent with this code base.

**Currently this README serves everyone, users and contributors.** It is disorganised with an idea that having disorganised and correct content is better than no content. It should be expanded on a regular basis and possibly split into ``README.md`` and ``README_CONTRIBUTOR.md``.

Please find **additional information** at our [project website](http://tree-edit-distance.dbresearch.uni-salzburg.at).

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

## Installing the library with VCPKG

This library has been ported to [VCPKG](https://github.com/microsoft/vcpkg) package manager. To include in your project, install with ``./vcpkg install tree-similarity``, then include the library in your project's CMakeFile

```
find_path(TREE_SIMILARITY_INCLUDE_DIRS "tree-similiarity/CMakeLists.txt")
target_include_directories(main PRIVATE ${TREE_SIMILARITY_INCLUDE_DIRS})
```

## Usage

The ``command_line`` program allows you to measure the Tree Edit Distance between 2 tree structures (using the Bracket Notation Format) using APTED.

The trees can be passed as string arguments:
```
./ted string {x{a}{b}} {x{a}{c}}
```

or read from files:
```
./ted file treeA.txt treeB.txt
```

## Implemented algorithms

The library implements many tree similarity algorithms. We categorize them based on the use case. Note that sometimes the class name is different to the algorithm's name used in a paper. The recommended, state-of-the-art algorithm is marked bold.

### Exact tree edit distance

Given two trees, $T$ and $T'$, compute the exact tree edit distance (TED) between $T$ and $T'$.

| Short name | C++ class | Paper titles and DOIs |
|:-----------|:----------|:----------------------|
| **APTED** | [`apted_tree_index.h`](./src/ted/apted_tree_index.h) | [Tree edit distance: robust and memory-efficient](https://doi.org/10.1016/j.is.2015.08.004) |
| ZhangShasha | [`zhang_shasha_tree_index.h`](./src/ted/zhang_shasha_tree_index.h) | [Simple fast algorithms for the editing distance between trees and related problems](https://doi.org/10.1137/0218082) |

APTED supersedes older exact TED algorithms, like [RTED](https://doi.org/10.1145/2699485), [Demaine](https://doi.org/10.1145/1644015.1644017), and [Klein](https://doi.org/10.1007/3-540-68530-8_8). We evaluated them experimentally before this library was born. If interested, refer to their [old Java implementations](http://tree-edit-distance.dbresearch.uni-salzburg.at/downloads/projects/tree-edit-distance/RTED_v1.2.jar).

### JSON similarity

Given two JSON trees, $T$ and $T'$, compute the JSON edit distance (JEDI) between $T$ and $T'$.

| Short name | C++ class | Paper title and doi |
|:-----------|:----------|:--------------------|
| **QuickJEDI** | [`quickjedi_index.h`](./src/json/quickjedi_index.h) | [JEDI: These aren't the JSON documents you're looking for?](https://doi.org/10.1145/3514221.3517850) |

### Tree similarity self join

Given a collection of trees $\mathcal{T}$ and a tree edit distance threshold $\tau$. The TED join is defined as the set of all distinct tree pairs in $\mathcal{T}$ that are within edit distance $\tau$.

$\mathcal{T}\bowtie_{\tau}\mathcal{T}=\{T,T'\}\in\mathcal{T}\times\mathcal{T}:T\ne T'\wedge\delta(T,T')\le\tau.$

| Short name | C++ class | Paper title and doi |
|:-----------|:----------|:--------------------|
| **TJoin** | [`t_join.ti.h`](./src/join/tjoin/t_join.ti.h) | [Effective filters and linear time verification for tree similarity joins](https://doi.org/10.1109/ICDE.2019.00081) |
| Tang | [`tang_join_ti.h`](./src/join/tang/tang_join_ti.h) | [Scaling similarity joins over tree-structured data](https://doi.org/10.14778/2809974.2809976) |
| Guha | [`guha_join_ti.h`](./src/join/guha/guha_join_ti.h) | [Approximate XML joins](https://doi.org/10.1145/564691.564725) |
| Binary | [`bb_join_ti.h`](./src/join/binary_branches/bb_join_ti.h) | [Similarity evaluation on tree-structured data](https://doi.org/10.1145/1066157.1066243) |
| Histo | [`histo_join_ti.h`](./src/join/histogram/histo_join_ti.h) | [A survey on tree edit distance lower bound estimation techniques for similarity join on XML data](https://doi.org/10.1145/2590989.2590994) |

### Exact tree edit distance with a known upper bound

Given two trees, $T$ and $T'$, and a true TED upper bound $\tau$ between $T$ and $T'$ compute the exact tree edit distance (TED) between $T$ and $T'$. These algorithms are faster than the classic TED algorithms thanks to pruning based on $\tau$.

| Short name | C++ class | Paper title and doi |
|:-----------|:----------|:--------------------|
| **TopDiff** | [`touzet_kr_set_tree_index.h`](./src/ted/touzet_kr_set_tree_index.h) | [Minimal edit-based diffs for large trees](https://doi.org/10.1145/3340531.3412026) |
| TopDiff+ | implemented only in [experiments](https://frosch.cosy.sbg.ac.at/mpawlik/diffs-large-trees-exp) | [Minimal edit-based diffs for large trees](https://doi.org/10.1145/3340531.3412026) |
| Touzet (baseline) | [`touzet_baseline_tree_index.h`](./src/ted/touzet_baseline_tree_index.h) | [Comparing similar ordered trees in linear-time](https://doi.org/10.1016/j.jda.2006.07.002) |
| Touzet (depth pruning) | [`touzet_depth_pruning_truncated_tree_fix_tree_index.h`](./src/ted/touzet_depth_pruning_truncated_tree_fix_tree_index.h) | [Comparing similar ordered trees in linear-time](https://doi.org/10.1016/j.jda.2006.07.002) |

These algorithms can be also executed without a known upper bound by using the `ted()` function from `ted_algorithm_touzet.h`. The details of this technique are explained in [Minimal edit-based diffs for large trees](https://doi.org/10.1145/3340531.3412026).

### Tree edit distance lower bounds

These algorithms have a lower runtime complexity than TED algorithms and return a value less than or equal to the exact TED.

| Short name | C++ class | Paper title and doi |
|:-----------|:----------|:--------------------|
| SED (string edit distance) | [`sed_tree_index.h`](./src/ted_lb/sed_tree_index.h) | [A linear space algorithm for computing maximal common subsequences](https://doi.org/10.1145/360825.360861) |
| Label intersection | [label_intersection.h](./src/ted_lb/label_intersection.h) ||

### Tree edit distance upper bounds

These algorithms have lower runtime complexity than TED algorithms and return a value greater than or equal to the exact TED.

| Short name | C++ class | Paper title and doi |
|:-----------|:----------|:--------------------|
| CTED (constrained TED) | [`cted_tree_index.h`](./src/ted_ub/cted_tree_index.h) | [Algorithms for the constrained editing distance between ordered labeled trees and related problems](https://doi.org/10.1016/0031-3203(94)00109-Y) |
| LGM (label guided mapping) | [`lgm_tree_index.h`](./src/ted_ub/lgm_tree_index.h) | [Effective filters and linear time verification for tree similarity joins](https://doi.org/10.1109/ICDE.2019.00081) |

## Generating documentation

Install [Doxygen](http://www.stack.nl/~dimitri/doxygen/) for generating the documentation.

Execute the following (in the project's root directory) to generate the documentation.
```bash
doxygen doxygen.config
```
Then, open ``doc/html/index.html`` in your browser.

## Notes on user workflow

1. Convert the input data into the Bracket Notation Format or implement a new parser (e.g., [http://www.antlr.org/](ANTLR)).
2. If necessary, implement custom Label class and/or custom CostModel class.
3. Modify the types of `Parser`, `Label`, and `CostModel` inside `tree_similarity.cc` if you do not want to use the default types.
4. Call `make` to (re-)build the framework.

## TODOs

This is a list of of all TODOs split into a few categories.

### Features.
- **Command line:** make nicer and document.
- **Edit mapping:** Implement with subtree-pairs distance matrix as input such that not only the matrix of Zhang and Shasha can be input. There is C++ code [here](https://frosch.cosy.sbg.ac.at/mpawlik/old-tree-similarity/-/blob/visualisation/src/zhang_shasha/zhang_shasha.h) (review and tests required).

### README.md
- Describe functionalities of the library.
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

### Random:
- Add a meaningful documentation to all parts of the codebase in the [Doxygen format](https://www.stack.nl/~dimitri/doxygen/manual/docblocks.html).
- Refactor parts of the codebase if necessary.
- Enforce (and probably extend) the coding policy/style described on the `master`
branch and create a [clang-format](https://clang.llvm.org/docs/ClangFormat.html)
configuration to simplify enforcing the coding style for future collaborators.
- Every pushed code is reviewed by a second person, or there is a discussion about it beforehand.
