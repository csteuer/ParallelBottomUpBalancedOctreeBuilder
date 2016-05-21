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

ParallelOctreeBuilder::ParallelOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint, uint maxLevel) : OctreeBuilder(maxXYZ, maxLevel) {
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
    OctantID root(Vector3i(0), getOctreeDepthForBounding(m_maxXYZ));

    pss::parallel_stable_sort(m_levelZeroLeafs.begin(), m_levelZeroLeafs.end());

    LinearOctree balancedOctree = createBalancedOctreeParallel(root, m_levelZeroLeafs, omp_get_max_threads(), maxLevel());

    std::unique_ptr<Octree> result(new OctreeImpl(balancedOctree));
    return result;
}
