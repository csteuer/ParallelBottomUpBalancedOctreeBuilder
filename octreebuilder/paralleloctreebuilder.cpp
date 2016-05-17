#include "paralleloctreebuilder.h"

#include "octree_impl.h"

#include "linearoctree.h"
#include "octree_utils.h"
#include "parallel_stable_sort.h"
#include "mortoncode.h"

#include <omp.h>
#include <algorithm>
#include <assert.h>

#include "logging.h"
#include "perfinfo.h"

ParallelOctreeBuilder::ParallelOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint, uint maxLevel)
    : OctreeBuilder(maxXYZ, maxLevel), m_parallel(true) {
    if (!fitsInMortonCode(maxXYZ)) {
        throw std::runtime_error("Space to large for octree creation.");
    }

    m_levelZeroLeafsSet.reserve(numLevelZeroLeafsHint);
    m_levelZeroLeafs.reserve(numLevelZeroLeafsHint);
}

morton_t ParallelOctreeBuilder::addLevelZeroLeaf(const Vector3i& c) {
    OctantID leaf(c, 0);
    auto insertPair = m_levelZeroLeafsSet.insert(leaf.mcode());

    if (insertPair.second) {
        m_levelZeroLeafs.push_back(leaf);
    }

    return leaf.mcode();
}

std::unique_ptr<Octree> ParallelOctreeBuilder::finishBuilding(bool) {
    START_NEW_PERF_COUNTER(overallPerf)

    OctantID root(Vector3i(0), getOctreeDepthForBounding(m_maxXYZ));

    NEW_PERF_COUNTER(sortPerf)
    NEW_PERF_COUNTER(createPerf)

    LinearOctree balancedOctree(root);

    if (m_parallel) {
        START_PERF(sortPerf)
        pss::parallel_stable_sort(m_levelZeroLeafs.begin(), m_levelZeroLeafs.end());
        STOP_PERF(sortPerf)

        START_PERF(createPerf)
        balancedOctree = createBalancedOctreeParallel(root, m_levelZeroLeafs, omp_get_max_threads(), maxLevel());
        STOP_PERF(createPerf)
    } else {
        START_PERF(sortPerf)
        for (const OctantID& p : m_levelZeroLeafs) {
            balancedOctree.insert(p);
        }
        STOP_PERF(sortPerf)

        START_PERF(sortPerf)
        createBalancedSubtree(balancedOctree, maxLevel());
        PERF(createPerf.setWorkSize(balancedOctree.leafs().size()))
        STOP_PERF(sortPerf)
    }


    START_NEW_PERF_COUNTER(createSFOPerf)
    std::unique_ptr<Octree> result(new OctreeImpl(balancedOctree));
    STOP_PERF(createSFOPerf)

    STOP_PERF(overallPerf)

    LOG_PERF("TwoStageSpaceFillingOctreeBuilder: finishBuilding( " << overallPerf << " ):")
    LOG_PERF("TwoStageSpaceFillingOctreeBuilder:  Sort: " << sortPerf)
    LOG_PERF("TwoStageSpaceFillingOctreeBuilder:  Create: " << createPerf)
    LOG_PERF("TwoStageSpaceFillingOctreeBuilder:  SpaceFillingOctree: " << createSFOPerf)
    return result;
}

bool ParallelOctreeBuilder::parallel() const {
    return m_parallel;
}

void ParallelOctreeBuilder::setParallel(bool parallel) {
    m_parallel = parallel;
}
