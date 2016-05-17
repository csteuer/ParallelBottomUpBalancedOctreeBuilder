#pragma once

#include "octreebuilder_api.h"

#include "vector3i.h"

#include "mortoncode.h"

#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <limits>

class Octree;

/**
 * @brief Creates a octree with a bottom-up method
 *
 * The resulting octree has the following properties:
 *  - it has the minimum number of nodes
 *  - it contains all leaf-nodes added to the builder
 */
class OCTREEBUILDER_API OctreeBuilder {
public:
    /**
     * @brief creates a OctreeBuilder instance
     * @param maxXYZ the most upper right back llf of all level zero leaf nodes that will be added (defines the domain/bounding of the octree)
     * @param maxLevel the maximum level of any octree node
     * @note the bounding box of an octree is always a cuboid... hence it might be larger (but never smaller) than the space defined by (0,0,0) and maxXYZ
     */
    explicit OctreeBuilder(const Vector3i& maxXYZ, uint maxLevel = std::numeric_limits<uint>::max());

    /**
     * @brief adds a leaf node of level 0 to the octree
     * @param c the llf of the node
     * @return the morton encoded llf of the node
     *
     * A leaf can be added multiple times.
     */
    virtual morton_t addLevelZeroLeaf(const Vector3i& c) = 0;

    virtual std::unique_ptr<Octree> finishBuilding(bool balanced = false) = 0;

    virtual ~OctreeBuilder();

protected:
    Vector3i m_maxXYZ;

    uint maxLevel();

private:
    uint m_maxLevel;
};
