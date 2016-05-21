#include <hayai.hpp>

#include <octreebuilder/sequentialoctreebuilder.h>
#include <octreebuilder/paralleloctreebuilder.h>
#include <octreebuilder/octree.h>

#include <vector>
#include <random>

#ifdef PROFILING_ENABLED
#include <gperftools/profiler.h>
#endif

constexpr size_t SEED = 12492;
constexpr size_t NUM_INPUT_LEAFS = 10000;
constexpr coord_t MAX_COORD = 1000;
constexpr Vector3i MAX_XYZ = Vector3i(MAX_COORD);

class OctreeBuilderBenchmark : public hayai::Fixture {
public:

    OctreeBuilderBenchmark() {
        std::default_random_engine generator(SEED);
        std::uniform_int_distribution<coord_t> coordinateDistribution(0, MAX_COORD);
        auto genCoord = std::bind(coordinateDistribution, generator);

        m_uniformDistributedInputLeafs.reserve(NUM_INPUT_LEAFS);
        for (size_t i = 0; i < NUM_INPUT_LEAFS; i++) {
            m_uniformDistributedInputLeafs.push_back(Vector3i(genCoord(), genCoord(), genCoord()));
        }

#ifdef PROFILING_ENABLED
        ProfilerStart("OctreeBuilderBenchmark.prof");
#endif
    }

    const std::vector<Vector3i>& getUniformDistributedInputLeafs() const {
        return m_uniformDistributedInputLeafs;
    }

    ~OctreeBuilderBenchmark()  {
#ifdef PROFILING_ENABLED
        ProfilerStop();
#endif
    }

private:
    std::vector<Vector3i> m_uniformDistributedInputLeafs;
};


BENCHMARK_F(OctreeBuilderBenchmark, uniformDistributionBalancedSequentialOctreeBuilder, 5, 2) {
    SequentialOctreeBuilder builder(MAX_XYZ, NUM_INPUT_LEAFS);
    for (const Vector3i& leaf : getUniformDistributedInputLeafs()) {
        builder.addLevelZeroLeaf(leaf);
    }
    builder.finishBuilding();
}

BENCHMARK_F(OctreeBuilderBenchmark, uniformDistributionBalancedParallelOctreeBuilder, 5, 2) {
    ParallelOctreeBuilder builder(MAX_XYZ, NUM_INPUT_LEAFS);
    for (const Vector3i& leaf : getUniformDistributedInputLeafs()) {
        builder.addLevelZeroLeaf(leaf);
    }
    builder.finishBuilding();
}
