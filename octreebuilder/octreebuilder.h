#pragma once

#include "octreebuilder_api.h"

#include "vector3i.h"

#include "mortoncode.h"

#include <memory>
#include <limits>

namespace octreebuilder {

class Octree;

/**
 * @brief Creates a 2:1 balanced octree with a bottom-up method
 *
 * The resulting octree has the following properties:
 *  - It is complete and has the minimum number of nodes.
 *  - It contains all leaf-nodes added to the builder.
 *  - The level of adjacent octants differs at most by 1 (adjacent = share at least one vertex).
 */
class OCTREEBUILDER_API OctreeBuilder {
public:
    /**
     * @brief Creates a OctreeBuilder instance
     * @param maxXYZ The most upper right back llf of all level zero leaf nodes that will be added (defines the domain/bounding of the octree)
     * @param maxLevel The maximum level of any octree node
     * @note The bounding box of an octree is always a cuboid... hence it might be larger (but never smaller) than the space defined by (0,0,0) and maxXYZ
     */
    explicit OctreeBuilder(const Vector3i& maxXYZ, uint maxLevel = ::std::numeric_limits<uint>::max());

    /**
     * @brief Adds a leaf node of level 0 to the octree
     * @param c The llf of the node
     * @return Tshe morton encoded llf of the node
     *
     * A leaf can be added multiple times.
     */
    virtual morton_t addLevelZeroLeaf(const Vector3i& c) = 0;

    virtual ::std::unique_ptr<Octree> finishBuilding() = 0;

    virtual ~OctreeBuilder();

protected:
    Vector3i m_maxXYZ;

    uint maxLevel();

private:
    uint m_maxLevel;
};
}
