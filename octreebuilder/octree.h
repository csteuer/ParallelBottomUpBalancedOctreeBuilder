#pragma once

#include "octreebuilder_api.h"
#include "octreenode.h"

#include "vector3i.h"

#include <memory>
#include <iosfwd>

class LinearOctree;

/**
 * @brief An octree datastructure optimized for spatial queries
 *
 * Octree nodes are cubes in the 3d-space.
 * Level zero nodes are of a size 1.
 * The domain/bounding of the octree itself is a cube with the origin (0,0,0) and the side length 2^depth.
 *
 * The octree only contains non overlapping leaf nodes.
 *
 * The level of nodes that share at least one vertex differes by at most 1 if the octree is balanced.
 */
class OCTREEBUILDER_API Octree {
public:

    /**
     * @brief the most upper right back llf of all possible octree nodes
     */
    virtual Vector3i getMaxXYZ() const = 0;

    /**
     * @brief the depth of the octree (length of path from root node to leaf node)
     */
    virtual uint getDepth() const = 0;

    /**
     * @brief the maximum level over all nodes in the tree
     * @note throws an error in case of an empty octree
     */
    virtual uint getMaxLevel() const = 0;

    /**
     * @brief the number of nodes in the octree
     */
    virtual size_t getNumNodes() const = 0;

    /**
     * @brief the i-th node in the octree
     */
    virtual OctreeNode getNode(const size_t& i) const = 0;

    /**
     * @brief returns the node with llf and level
     * @param llf lower left front vertex of the node
     * @param level the level of the node
     * @return the specified node or a invalid node if no such node exists
     */
    virtual OctreeNode tryGetNodeAt(const Vector3i& llf, uint level) const = 0;

    /**
     * @brief returns the node that contains the point (or rather the voxel at that location)
     */
    virtual OctreeNode nodeAt(const Vector3i& point) const = 0;

    /**
     * @brief returns the k nearest nodes to the point
     *
     * TODO: Add test
     */
    virtual std::vector<OctreeNode> kNearestNodes(const Vector3i& point, const u_int k) const = 0;

    /**
     * @brief find the neighbour node(s) of n at sharedFace
     * @param n the node whose neighbours should be found
     * @param sharedFace the face of n that is shared (completely or partly) with each neighbour node
     * @return a list of 0 (no neighbours), 1 or 4 nodes that are neighbours of n at sharedFace
     */
    virtual std::vector<OctreeNode> getNeighbourNodes(const OctreeNode& n, OctreeNode::Face sharedFace) const = 0;

    virtual ~Octree();
};

OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const OctreeNode& n);
OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const Octree& tree);
OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const std::unique_ptr<Octree>& tree);
