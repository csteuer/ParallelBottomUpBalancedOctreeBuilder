#pragma once

#include "octreebuilder_api.h"
#include "mortoncode.h"

#include <iosfwd>

namespace octreebuilder {

/**
 * @brief A node of an octree that represents a cuboid part of space
 */
class OCTREEBUILDER_API OctreeNode {
public:
    OctreeNode();
    OctreeNode(morton_t morton_encoded_llf, uint level);
    OctreeNode(const Vector3i& coordinate, uint level);

    /**
     * @brief The faces of an octree node
     */
    enum Face { LEFT = 0, RIGHT = 1, FRONT = 2, BACK = 3, BOTTOM = 4, TOP = 5 };

    /**
     * @brief Returns the unit normal of a face
     */
    static Vector3i getNormalOfFace(const Face& f);

    /**
     * @brief Returns true if this is a valid octree node
     * @note A invalid octree node is not equal to any other node (even if its also an invalid node)
     */
    bool isValid() const;

    /**
     * @brief The lower left front vertex of the cuboid represent by this node
     */
    Vector3i getLLF() const;

    /**
     * @brief The morton encoded LLF
     */
    morton_t getMortonEncodedLLF() const;

    /**
     * @brief The side legth of the cuboid represent by this node
     */
    coord_t getSize() const;

    /**
     * @brief The level of the node in the octree
     *
     * The level is the inverse of the depth at which the node is located in the octree.
     * Hence leafs nodes are at level 0
     */
    uint getLevel() const;

    bool operator==(const OctreeNode& o) const;

    bool operator!=(const OctreeNode& o) const;

private:
    morton_t m_morton_llf;
    uint m_level;
};

OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const OctreeNode::Face& f);
}
