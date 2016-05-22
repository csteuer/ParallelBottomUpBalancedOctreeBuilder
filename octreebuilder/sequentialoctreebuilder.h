#pragma once

#include "octreebuilder_api.h"
#include "octreebuilder.h"

#include <unordered_set>

class OCTREEBUILDER_API SequentialOctreeBuilder : public OctreeBuilder {
public:
    /**
     * @see OctreeBuilder::OctreeBuilder
     * @param numLevelZeroLeafsHint the approximate number of level zero leafs that will be added. The hint is used to improve performance.
     */
    explicit SequentialOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint = 0, uint maxLevel = std::numeric_limits<uint>::max());

    virtual morton_t addLevelZeroLeaf(const Vector3i& c) override;

    virtual std::unique_ptr<Octree> finishBuilding() override;

private:
    std::unordered_set<morton_t> m_levelZeroLeafsSet;
};
