# ParallelBottomUpBalancedOctreeBuilder

[![Build Status](https://travis-ci.org/ClausSteuer/ParallelBottomUpBalancedOctreeBuilder.svg?branch=master)](https://travis-ci.org/ClausSteuer/ParallelBottomUpBalancedOctreeBuilder)

A library for the creation of 2:1 balanced octrees.
Implements a parallel and a serial bottom-up algorithm.
The parallel algorithm is a modified version of
[Bottom-up construction and 2: 1 balance refinement of linear octrees in parallel](http://epubs.siam.org/doi/abs/10.1137/070681727) by Sundar et al.

Its optimized for shared memory systems instead of distributed memory systems, using
OpenMP and Arch D. Robisons [parallel stable sort algorithm for OpenMP](https://software.intel.com/en-us/articles/a-parallel-stable-sort-using-c11-for-tbb-cilk-plus-and-openmp).

## Dependencies

Requires:
 * [gcc](https://gcc.gnu.org/) 4.9+ or [clang](http://clang.llvm.org/) 3.7+
 * [cmake](https://cmake.org/) 2.8.12+

Optional:
 * [tcmalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html) (for better performance)
 * [googletest](https://github.com/google/googletest) master (for building the tests)
 * [hayai](https://github.com/nickbruun/hayai) v1.0.1 (for building the benchmarks)
 * [gperftools](https://github.com/gperftools/gperftools) (for profiling)
 * [doxygen](www.doxygen.org/) (for generating the documentatíon).

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

Run `make doc` to generate the documentation in `<project-root>/doc` (doxygen must be installed and and found by cmake).

### Build and run the tests

Clone [googletest](https://github.com/google/googletest) into `<project-root>/googletest` and set the cmake variable `BUILD_TESTS=ON`:

~~~~~~~~~~~~~{.sh}
git clone https://github.com/google/googletest.git
cd build
cmake .. -DBUILD_TESTS=ON
./octreebuilder/tests/octreebuilder-unittests
./octreebuilder/tests/octreebuilder-integrationtests
~~~~~~~~~~~~~

### Build and run the benchmarks

Install [hayai](https://github.com/nickbruun/hayai), set the cmake variable `BUILD_BENCHMARKS=ON` then run the benchmark executable.

## How to use

~~~~~~~~~~~~~{.cpp}

#include <iostream>
#include <memory>
#include <random>
#include <omp.h>

#include <octreebuilder/paralleloctreebuilder.h>
#include <octreebuilder/sequentialoctreebuilder.h>
#include <octreebuilder/octree.h>

using namespace octreebuilder;

int main(int, char**) {
    const Vector3i maxCoord(1000, 1000, 1000);

    std::unique_ptr<OctreeBuilder> octreeBuilder;

    if (omp_get_max_threads() > 1) {
        octreeBuilder = std::unique_ptr<OctreeBuilder>(new ParallelOctreeBuilder(maxCoord));
    } else {
        octreeBuilder = std::unique_ptr<OctreeBuilder>(new SequentialOctreeBuilder(maxCoord));
    }

    std::default_random_engine generator;
    std::uniform_int_distribution<coord_t> coordinateDistribution(0, 1000);
    auto genCoord = std::bind(coordinateDistribution, generator);

    for (size_t i = 0; i < 5000; i++) {
        octreeBuilder->addLevelZeroLeaf(Vector3i(genCoord(), genCoord(), genCoord()));
    }

    std::unique_ptr<Octree> octree = octreeBuilder->finishBuilding();

    std::cout << "Octree: " << octree << std::endl;
}

~~~~~~~~~~~~~

Output:
~~~~~~~~~~~~~{.txt}
Octree: { depth: 10, maxLevel: 5, maxXYZ: (1023, 1023, 1023), numNodes: 1693581 }
~~~~~~~~~~~~~

## Optimization

### Use TCMalloc
Install [TCMalloc](http://goog-perftools.sourceforge.net/doc/tcmalloc.html) to speed up memory allocation.
On a ubuntu system either install gperftools (see below) or the standalone version of TCMalloc (`sudo apt-get install libtcmalloc-minimal4`).
The library will automatically link against tcmalloc if it is found.
If you don't want this set the cmake flag `BUILD_WITH_TCMALLOC=OFF`.

### Use SSE
Streaming SIMD Extensions (SSE) are used by default to speed up vector calculations.
If your system does not support SSE2 and SSE3 set the flag `BUILD_WITH_SSE=OFF`.

### Profile the code

Install [gperftools](https://github.com/gperftools/gperftools) (e.g. on ubuntu systems run `sudo apt-get install libgoogle-perftools-dev`),
set the cmake variable `ENABLE_PROFILING=ON` and build in release mode with debug information.
Run the benchmark executable (should create `OctreeBuilderBenchmark.prof`), create a callgrind file with pprof and analyze the results with [kcachegrind](http://kcachegrind.sourceforge.net/html/Home.html):
~~~~~~~~~~~~~{.sh}
cd build
cmake .. -DCMAKE_BUILD_TYPE=RelWithDebInfo -DBUILD_BENCHMARKS=ON -DENABLE_PROFILING=ON
cd benchmarks
./benchmarks
google-pprof benchmarks OctreeBuilderBenchmark.prof --callgrind > bench.callgrind
kcachegrind bench.callgrind
~~~~~~~~~~~~~
