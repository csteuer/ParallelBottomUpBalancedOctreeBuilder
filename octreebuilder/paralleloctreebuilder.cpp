#include "paralleloctreebuilder.h"

#include "octree_impl.h"

#include "linearoctree.h"
#include "octree_utils.h"
#include "parallel_stable_sort.h"
#include "mortoncode.h"

#include <omp.h>
#include <algorithm>
#include <assert.h>

#include "perfcounter.h"
#include "logging.h"
#include <iomanip>

ParallelOctreeBuilder::ParallelOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint, uint maxLevel) : OctreeBuilder(maxXYZ, maxLevel) {
    if (!fitsInMortonCode(maxXYZ)) {
        throw std::runtime_error("Space to large for octree creation.");
    }

    m_levelZeroLeafsSet.reserve(numLevelZeroLeafsHint);
}

morton_t ParallelOctreeBuilder::addLevelZeroLeaf(const Vector3i& c) {
    morton_t mortonCode = getMortonCodeForCoordinate(c);

    m_levelZeroLeafsSet.insert(mortonCode);

    return mortonCode;
}

std::unique_ptr<Octree> ParallelOctreeBuilder::finishBuilding() {
    PerfCounter perfCounter;

    OctantID root(Vector3i(0), getOctreeDepthForBounding(m_maxXYZ));

    perfCounter.start();
    std::vector<OctantID> levelZeroLeafs;
    levelZeroLeafs.reserve(m_levelZeroLeafsSet.size());
    for (morton_t mcode : m_levelZeroLeafsSet) {
        levelZeroLeafs.push_back(OctantID(mcode, 0));
    }
    LOG_PROF(std::left << std::setw(30) << "Create level zero leafs list: " << perfCounter);

    perfCounter.start();
    pss::parallel_stable_sort(levelZeroLeafs.begin(), levelZeroLeafs.end());
    LOG_PROF(std::left << std::setw(30) << "Sort level zero leafs list: " << perfCounter);

    perfCounter.start();
    LinearOctree balancedOctree = createBalancedOctreeParallel(root, levelZeroLeafs, omp_get_max_threads(), maxLevel());
    LOG_PROF(std::left << std::setw(30) << "Created octree: " << perfCounter);

    std::unique_ptr<Octree> result(new OctreeImpl(std::move(balancedOctree)));
    return result;
}
