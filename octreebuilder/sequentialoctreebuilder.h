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
    typedef std::unordered_set<morton_t> NodeSet;
    void insertIntoTree(const morton_t& node, const uint& level);
    void createSiblingsAndGuardNodes(const morton_t current_node, const uint currentLevel);

    std::vector<std::unordered_set<morton_t>> m_tree;

    NodeSet m_nonEmptyNodesOfCurrentLevel;
    NodeSet m_nonEmptyParentNodesOfCurrentLevel;
    NodeSet m_guardNodesOfCurrentLevel;
    uint m_finalLevel;
    uint m_depth;
};
