#include <hayai.hpp>

#include <octreebuilder/sequentialoctreebuilder.h>
#include <octreebuilder/paralleloctreebuilder.h>
#include <octreebuilder/octree.h>
#include <octreebuilder/box.h>
#include <octreebuilder/vector_utils.h>

#include <vector>
#include <random>

#ifdef PROFILING_ENABLED
#include <gperftools/profiler.h>
#endif

constexpr size_t SEED = 12492;
constexpr size_t NUM_INPUT_LEAFS = 10000;
constexpr coord_t MAX_COORD = 1000;
constexpr Vector3i MAX_XYZ = Vector3i(MAX_COORD);

constexpr coord_t squared(coord_t value) {
    return value * value;
}

constexpr coord_t SPHERE_RADIUS = 200;
constexpr coord_t SPHERE_MARGIN = 50;
constexpr Box SPHERE_BB(Vector3i(SPHERE_MARGIN), Vector3i((2 * SPHERE_RADIUS) + SPHERE_MARGIN));
constexpr Vector3i SPHERE_MAX_XYZ(2 * SPHERE_MARGIN + 2 * SPHERE_RADIUS);
constexpr coord_t SPHERE_HALF_SURFACE_THICKNESS = 1;
constexpr coord_t SPHERE_INNER_SQUARED_SURFACE_DISTANCE = squared(SPHERE_RADIUS - SPHERE_HALF_SURFACE_THICKNESS);
constexpr coord_t SPHERE_OUTER_SQUARED_SURFACE_DISTANCE = squared(SPHERE_RADIUS + SPHERE_HALF_SURFACE_THICKNESS);

static_assert(SPHERE_OUTER_SQUARED_SURFACE_DISTANCE > SPHERE_INNER_SQUARED_SURFACE_DISTANCE, "Sphere surface to thin.");

class OctreeBuilderBenchmark : public hayai::Fixture {
public:
    OctreeBuilderBenchmark() {
        {
            std::default_random_engine generator(SEED);
            std::uniform_int_distribution<coord_t> coordinateDistribution(0, MAX_COORD);
            auto genCoord = std::bind(coordinateDistribution, generator);

            m_uniformDistributedInputLeafs.reserve(NUM_INPUT_LEAFS);
            for (size_t i = 0; i < NUM_INPUT_LEAFS; i++) {
                m_uniformDistributedInputLeafs.push_back(Vector3i(genCoord(), genCoord(), genCoord()));
            }
        }

        {
            const Vector3i sphereCenter = SPHERE_BB.llf() + Vector3i(SPHERE_RADIUS);

            for (Vector3i point : ClosedVectorSpace(SPHERE_BB)) {
                Vector3i diff = point - sphereCenter;
                coord_t squaredDistance = diff.dot(diff);

                if (squaredDistance > SPHERE_INNER_SQUARED_SURFACE_DISTANCE && squaredDistance < SPHERE_OUTER_SQUARED_SURFACE_DISTANCE) {
                    m_sphereSurfaceLeafs.push_back(point);
                }
            }
        }

#ifdef PROFILING_ENABLED
        ProfilerStart("OctreeBuilderBenchmark.prof");
#endif
    }

    const std::vector<Vector3i>& getUniformDistributedInputLeafs() const {
        return m_uniformDistributedInputLeafs;
    }

    const std::vector<Vector3i>& getSphereSurfaceLeafs() const {
        return m_sphereSurfaceLeafs;
    }

    ~OctreeBuilderBenchmark() {
#ifdef PROFILING_ENABLED
        ProfilerStop();
#endif
    }

private:
    std::vector<Vector3i> m_uniformDistributedInputLeafs;
    std::vector<Vector3i> m_sphereSurfaceLeafs;
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

BENCHMARK_F(OctreeBuilderBenchmark, sphereSurfaceLeafsBalancedSequentialOctreeBuilder, 5, 2) {
    SequentialOctreeBuilder builder(SPHERE_MAX_XYZ, getSphereSurfaceLeafs().size());
    for (const Vector3i& leaf : getSphereSurfaceLeafs()) {
        builder.addLevelZeroLeaf(leaf);
    }
    builder.finishBuilding();
}

BENCHMARK_F(OctreeBuilderBenchmark, sphereSurfaceLeafsBalancedParallelOctreeBuilder, 5, 2) {
    ParallelOctreeBuilder builder(SPHERE_MAX_XYZ, getSphereSurfaceLeafs().size());
    for (const Vector3i& leaf : getSphereSurfaceLeafs()) {
        builder.addLevelZeroLeaf(leaf);
    }
    builder.finishBuilding();
}
