# ParallelBottomUpOctreeBuilder

What it does.
Implements a modified version of the <Name> algorithm described in [Reference](JournalLink).

## Dependencies

Requires gcc 4.7+ or clang 3.1+, cmake 2.8.12+, [googletest](https://github.com/google/googletest) for building the tests (optional),
[hayai](https://github.com/nickbruun/hayai) for building the benchmarks (optional) and doxygen for generating the documentatíon (also optional).

## How to build

In the project root run:
~~~~~~~~~~~~~{.txt}
mkdir build
cd build
cmake ..
make
~~~~~~~~~~~~~
To install the library at the location defined by the `CMAKE_INSTALL_PREFIX` variable run `make install` inside the `build` directory.

Set `BUILD_SHARED_LIBS=ON` to build a shared library (static by default).

Run `make doc` to generate the documentation (doxygen must be installed and and found by cmake).

### Build and run the tests

Clone [googletest (master)](https://github.com/google/googletest) into `<project-root>/googletest` and set the cmake variable `BUILD_TESTS=ON`:

~~~~~~~~~~~~~{.txt}
git clone https://github.com/google/googletest.git
cd build
cmake .. -DBUILD_TESTS=ON
./octreebuilder/tests/octreebuilder-tests
~~~~~~~~~~~~~

### Build and run the benchmarks

Install [hayai](https://github.com/nickbruun/hayai), set the cmake variable `BUILD_BENCHMARKS=ON` then run the benchmark executable.

## How to use

~~~~~~~~~~~~~{.cpp}

int main(int, char**) {

    // To be written

    return 0;

}

~~~~~~~~~~~~~

Output:
~~~~~~~~~~~~~{.txt}

~~~~~~~~~~~~~
