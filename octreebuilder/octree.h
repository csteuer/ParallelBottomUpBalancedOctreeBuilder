#pragma once

#include "octreebuilder_api.h"
#include "octreenode.h"

#include "vector3i.h"

#include <vector>
#include <memory>
#include <iosfwd>

namespace octreebuilder {

class LinearOctree;

/**
 * @brief An octree datastructure
 *
 * Octree nodes are cubes in the 3d-space.
 * Level zero nodes are of a size 1.
 * The domain/bounding of the octree itself is a cube with the origin (0,0,0) and the side length 2^depth.
 * The octree only contains non overlapping leaf nodes.
 * The octree is 2:1 balanced: The level of nodes that share at least one vertex differes by at most 1.
 */
class OCTREEBUILDER_API Octree {
public:
    /**
     * @brief The most upper right back llf of all possible octree nodes
     */
    virtual Vector3i getMaxXYZ() const = 0;

    /**
     * @brief The depth of the octree (length of path from root node to leaf node)
     */
    virtual uint getDepth() const = 0;

    /**
     * @brief The maximum level over all nodes in the tree
     * @note Throws an error in case of an empty octree
     */
    virtual uint getMaxLevel() const = 0;

    /**
     * @brief The number of nodes in the octree
     */
    virtual size_t getNumNodes() const = 0;

    /**
     * @brief The i-th node in the octree
     */
    virtual OctreeNode getNode(const size_t& i) const = 0;

    /**
     * @brief Returns the node with llf and level
     * @param llf Lower left front vertex of the node
     * @param level The level of the node
     * @return The specified node or a invalid node if no such node exists
     */
    virtual OctreeNode tryGetNodeAt(const Vector3i& llf, uint level) const = 0;

    /**
     * @brief Find the neighbour node(s) of n at sharedFace
     * @param n The node whose neighbours should be found
     * @param sharedFace The face of n that is shared (completely or partly) with each neighbour node
     * @return A list of 0 (no neighbours), 1 or 4 nodes that are neighbours of n at sharedFace
     */
    virtual ::std::vector<OctreeNode> getNeighbourNodes(const OctreeNode& n, OctreeNode::Face sharedFace) const = 0;

    enum class OctreeState { VALID, INCOMPLETE, OVERLAPPING, UNSORTED, UNBALANCED };

    /**
     * @brief Checks if the octree is 2:1 balanced, sorted and only contains non overlapping leaf nodes that cover the whole space of the tree (complete)
     * @return true If the octree is valid, false otherwise
     */
    virtual OctreeState checkState() const = 0;

    virtual ~Octree();
};

OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const OctreeNode& n);
OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const Octree& tree);
OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const ::std::unique_ptr<Octree>& tree);
OCTREEBUILDER_API ::std::ostream& operator<<(::std::ostream& s, const Octree::OctreeState& octreeState);
}
