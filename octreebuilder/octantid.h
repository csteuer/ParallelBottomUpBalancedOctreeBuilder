#pragma once

#include "octreebuilder_api.h"

#include <vector>
#include <iosfwd>

#include "mortoncode.h"

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
     * @brief the morton code of the octant.
     */
    morton_t mcode() const;

    /**
     * @brief the level of the octant.
     */
    uint level() const;

    /**
     * @brief coordinate of the lower left front vertex of the octant
     */
    Vector3i coord() const;

    /**
     * @brief the parent of the octant in the octree hierachy (contains this octant)
     * @return the OctantID of the parent octant. The parent's type will always be 0.
     */
    OctantID parent() const;

    /**
     * @brief the ancestor (self, parent, grandparent, great-grantparent, etc..)  of the octant in the octree hierachy
     * @param level the level of the ancestor. Must be greater or equal than the level of this node.
     * @return the OctantID of the ancestor octant with the given level. The ancestors's type will be equal to this octant.
     */
    OctantID ancestorAtLevel(uint level) const;

    /**
     * @brief the eight (direct) children of the octant
     */
    std::vector<OctantID> children() const;

    /**
     * @brief tests whether this octant is a descendant of another octant
     * @param possibleAncestor the OctantID of the possible ancestor octant
     * @return true if this octant is a descendant of possibleAncestor, false otherwise
     */
    bool isDecendantOf(const OctantID& possibleAncestor) const;

    /**
     * @brief the octants that are adjacent to this octant (share at least one vertex), have the same level and are inside the octree
     * @param octree a octree that contains this octant inside its bounds
     * @return the list of all neighbour octants. Can be empty. Contains at most 26 OctantID objects.
     */
    std::vector<OctantID> potentialNeighbours(const LinearOctree& octree) const;

    /**
     * @brief same as potentialNeighbours but does not contain siblings of this node (same parent)
     * @param octree a octree that contains this octant inside its bounds (does not have to be an element of the linear tree)
     * @return the list of all neighbour octants that are't siblings of this node. Can be empty. Contains at most 19 OctantID objects.
     */
    std::vector<OctantID> potentialNeighboursWithoutSiblings(const LinearOctree& octree) const;

    /**
     * @brief tests whether this octant is at the boundary of a subtree (block) but not at the boundary of the global octree
     * @param block a subtree that contains this octant inside its bounds
     * @param globalTree a octree that contains this octant inside its bounds
     * @return true if this octant is at the boundary of block but not at the boundary of the global octree
     * @note If block is not inside the bounds of globalTree or this octant is not inside the bounds of block the result is undefined
     */
    bool isBoundaryOctant(const LinearOctree& block, const LinearOctree& globalTree) const;

private:
    morton_t m_mcode;
    uint m_level;
};

namespace std {
template <>
struct hash<OctantID> {
    typedef OctantID argument_type;
    typedef morton_t result_type;
    result_type operator()(argument_type const& octant) const {
        return octant.mcode();
    }
};
}

bool OCTREEBUILDER_API operator<(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator<=(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator>(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator>=(const OctantID& left, const OctantID& right);
bool OCTREEBUILDER_API operator==(const OctantID& a, const OctantID& b);
bool OCTREEBUILDER_API operator!=(const OctantID& a, const OctantID& b);
OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const OctantID& n);
