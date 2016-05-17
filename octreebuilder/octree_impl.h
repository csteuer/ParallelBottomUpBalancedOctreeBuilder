#pragma once

#pragma once

#include "octreebuilder_api.h"
#include "octree.h"
#include "box.h"

#include <vector>
#include <unordered_set>
#include <unordered_map>

class LinearOctree;

class OCTREEBUILDER_API OctreeImpl : public Octree {
public:
    OctreeImpl(std::vector<std::unordered_set<morton_t>> tree);
    OctreeImpl(const LinearOctree& linearOctree);

    virtual Vector3i getMaxXYZ() const override;

    virtual uint getDepth() const override;

    virtual uint getMaxLevel() const override;

    virtual size_t getNumNodes() const override;

    virtual OctreeNode getNode(const size_t& i) const override;

    virtual OctreeNode tryGetNodeAt(const Vector3i& llf, uint level) const override;

    virtual OctreeNode nodeAt(const Vector3i& point) const override;

    virtual std::vector<OctreeNode> kNearestNodes(const Vector3i& point) const override;

    virtual std::vector<OctreeNode> getNeighbourNodes(const OctreeNode& n, OctreeNode::Face sharedFace) const override;

private:
    // morton codes grouped by level
    std::vector<std::unordered_set<morton_t>> m_tree;
    // linear tree
    std::vector<std::pair<morton_t, uint>> m_nodeList;
    Box m_bounding;
};
