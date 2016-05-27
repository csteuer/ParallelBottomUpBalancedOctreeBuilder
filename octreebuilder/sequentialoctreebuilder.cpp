#include "sequentialoctreebuilder.h"
#include "mortoncode_utils.h"

#include "octree_impl.h"
#include "octree_utils.h"

#include "perfcounter.h"
#include <iostream>

namespace octreebuilder {

SequentialOctreeBuilder::SequentialOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint, uint maxLevel) : OctreeBuilder(maxXYZ, maxLevel) {
    if (!fitsInMortonCode(maxXYZ)) {
        throw ::std::runtime_error("Space to large for octree creation.");
    }

    if (numLevelZeroLeafsHint > 0) {
        m_levelZeroLeafsSet.reserve(numLevelZeroLeafsHint);
    }
}

morton_t SequentialOctreeBuilder::addLevelZeroLeaf(const Vector3i& c) {
    morton_t mortonCode = getMortonCodeForCoordinate(c);

    m_levelZeroLeafsSet.insert(mortonCode);

    return mortonCode;
}

::std::unique_ptr<Octree> SequentialOctreeBuilder::finishBuilding() {
    PerfCounter perfCounter;

    perfCounter.start();
    LinearOctree linearOctree(OctantID(0, getOctreeDepthForBounding(m_maxXYZ)), m_levelZeroLeafsSet.size());

    for (const morton_t& mcode : m_levelZeroLeafsSet) {
        linearOctree.insert(OctantID(mcode, 0));
    }

    LOG_PROF("Created initial tree: " << perfCounter);

    perfCounter.start();
    createBalancedSubtree(linearOctree, maxLevel());
    LOG_PROF("Created balanced tree: " << perfCounter);

    return ::std::unique_ptr<Octree>(new OctreeImpl(::std::move(linearOctree)));
}
}
