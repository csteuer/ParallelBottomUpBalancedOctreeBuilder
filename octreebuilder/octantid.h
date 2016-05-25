#pragma once

#include "octreebuilder_api.h"

#include <vector>
#include <iosfwd>

#include "mortoncode.h"

namespace octreebuilder {
class LinearOctree;

/**
 * @brief The OctantID class uniquely identifies the octants of an octree data structure. Each octant represents a cube in 3D space.
 *
 * The morton code together with the level is a unique identifier.
 *
 * OctantID defines a linear order over all octants with the following property: If Octant A is a child of Octant B then A > B.
 */
class OCTREEBUILDER_API OctantID {
public:
    OctantID();
    OctantID(const Vector3i& coord, uint level);
    OctantID(morton_t mcode, uint level);

    /**
     * @brief The morton code of the octant.
     */
    morton_t mcode() const;

    /**
     * @brief The level of the octant.
     */
    uint level() const;

    /**
     * @brief Coordinate of the lower left front vertex of the octant
     */
    Vector3i coord() const;

    /**
     * @brief The parent of the octant in the octree hierachy (contains this octant)
     * @return The OctantID of the parent octant. The parent's type will always be 0.
     */
    OctantID parent() const;

    /**
     * @brief The ancestor (self, parent, grandparent, great-grantparent, etc..)  of the octant in the octree hierachy
     * @param level The level of the ancestor. Must be greater or equal than the level of this node.
     * @return The OctantID of the ancestor octant with the given level. The ancestors's type will be equal to this octant.
     */
    OctantID ancestorAtLevel(uint level) const;

    /**
     * @brief The eight (direct) children of the octant
     */
    ::std::vector<OctantID> children() const;

    /**
     * @brief Tests whether this octant is a descendant of another octant
     * @param possibleAncestor The OctantID of the possible ancestor octant
     * @return True if this octant is a descendant of possibleAncestor, false otherwise
     */
    bool isDecendantOf(const OctantID& possibleAncestor) const;

    /**
     * @brief The octants that are adjacent to this octant (share at least one vertex), have the same level and are inside the octree
     * @param octree A octree that contains this octant inside its bounds
     * @return The list of all neighbour octants. Can be empty. Contains at most 26 OctantID objects.
     */
    ::std::vector<OctantID> potentialNeighbours(const LinearOctree& octree) const;

    /**
     * @brief Same as potentialNeighbours but does not contain siblings of this node (same parent)
     * @param octree A octree that contains this octant inside its bounds (does not have to be an element of the linear tree)
     * @return The list of all neighbour octants that are't siblings of this node. Can be empty. Contains at most 19 OctantID objects.
     */
    ::std::vector<OctantID> potentialNeighboursWithoutSiblings(const LinearOctree& octree) const;

    /**
     * @brief Tests whether this octant is at the boundary of a subtree (block) but not at the boundary of the global octree
     * @param block A subtree that contains this octant inside its bounds
     * @param globalTree A octree that contains this octant inside its bounds
     * @return True if this octant is at the boundary of block but not at the boundary of the global octree
     * @note If block is not inside the bounds of globalTree or this octant is not inside the bounds of block the result is undefined
     */
    bool isBoundaryOctant(const LinearOctree& block, const LinearOctree& globalTree) const;

    /**
     * @brief Tests whether this octant is at the boundary of a subtree (block) but not at the boundary of the global octree
     * @param blockLLF The llf of the block
     * @param blockURB The urb of the block
     * @param treeLLF The llf of the global tree
     * @param treeURB The urb of the global tree
     * @return True if this octant is at the boundary of block but not at the boundary of the global octree
     * @note If block is not inside the bounds of the global tree or this octant is not inside the bounds of block the result is undefined
     * @remark This overload of isBoundaryOctant has better performance. If the same block and global tree is used for multiple octants use this method.
     */
    bool isBoundaryOctant(const Vector3i& blockLLF, const Vector3i& blockURB, const Vector3i& treeLLF, const Vector3i& treeURB) const;

    /**
     * @brief The octants at level 0 whose ancestors might violate the balance condition with respect to this octant
     * @param octree An octree that contains this octant inside its bounds (does not have to be an actual leaf of the linear tree)
     * @return All level 0 octants with the specified condition that are inside bounds of the octree.
     */
    ::std::vector<OctantID> getSearchKeys(const LinearOctree& octree) const;

private:
    morton_t m_mcode;
    uint m_level;
};

bool OCTREEBUILDER_API operator<(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator<=(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator>(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator>=(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator==(const OctantID& a, const OctantID& b);
bool OCTREEBUILDER_API operator!=(const OctantID& a, const OctantID& b);
OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const OctantID& n);
}

namespace std {
    template <>
    struct hash<octreebuilder::OctantID> {
        typedef octreebuilder::OctantID argument_type;
        typedef octreebuilder::morton_t result_type;
        result_type operator()(argument_type const& octant) const {
            return octant.mcode();
        }
    };
}
