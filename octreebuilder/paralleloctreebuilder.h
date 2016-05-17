#pragma once

#include "octreebuilder_api.h"

#include "octreebuilder.h"
#include "octantid.h"

#include <unordered_set>

class OCTREEBUILDER_API ParallelOctreeBuilder : public OctreeBuilder {
public:
    /**
     * @see OctreeBuilder::OctreeBuilder
     * @param numLevelZeroLeafsHint the approximate number of level zero leafs that will be added. The hint is used to improve performance.
     */
    explicit ParallelOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint = 0, uint maxLevel = std::numeric_limits<uint>::max());

    virtual morton_t addLevelZeroLeaf(const Vector3i& c) override;
    virtual std::unique_ptr<Octree> finishBuilding(bool) override;

    bool parallel() const;
    void setParallel(bool parallel);

private:
    bool m_parallel;
    std::unordered_set<morton_t> m_levelZeroLeafsSet;
    std::vector<OctantID> m_levelZeroLeafs;
};
