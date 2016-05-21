#include "sequentialoctreebuilder.h"

#include "box.h"

#include "octree_impl.h"

#include <vector>
#include <algorithm>
#include <assert.h>
#include <stack>

SequentialOctreeBuilder::SequentialOctreeBuilder(const Vector3i& maxXYZ, size_t numLevelZeroLeafsHint, uint maxLevel) : OctreeBuilder(maxXYZ, maxLevel) {
    if (!fitsInMortonCode(maxXYZ)) {
        throw std::runtime_error("Space to large for octree creation.");
    }
    m_depth = getOctreeDepthForBounding(maxXYZ);

    if (numLevelZeroLeafsHint > 0) {
        m_nonEmptyNodesOfCurrentLevel.reserve(numLevelZeroLeafsHint);
    }
}

morton_t SequentialOctreeBuilder::addLevelZeroLeaf(const Vector3i& c) {
    morton_t mortonCode = getMortonCodeForCoordinate(c);

    m_nonEmptyNodesOfCurrentLevel.insert(mortonCode);

    return mortonCode;
}

void SequentialOctreeBuilder::insertIntoTree(const morton_t& node, const uint& level) {
#ifndef NDEBUG
    Vector3i maxXYZ = getMaxXYZForOctreeDepth(static_cast<uint>(m_tree.size() - 1));
    Box treeOuterBounding(maxXYZ + Vector3i(1));
    Vector3i node_llf = getCoordinateForMortonCode(node);
    coord_t nodeSize = getOctantSizeForLevel(level);
    Box nodeBounding(node_llf, node_llf + Vector3i(nodeSize));
    assert(treeOuterBounding.contains(nodeBounding));
#endif
    m_tree.at(level).insert(node);
}

void SequentialOctreeBuilder::createSiblingsAndGuardNodes(const morton_t current_node, const uint currentLevel) {
    const morton_t parent = getMortonCodeForParent(current_node, currentLevel);

    if (!m_nonEmptyParentNodesOfCurrentLevel.insert(parent).second) {
        return;
    }

    auto siblingsAndSelf = getMortonCodesForChildren(parent, currentLevel + 1);

    for (const morton_t& child : siblingsAndSelf) {
        // Do not add the current_node it self or any other nonEmptyNode (its represented by its children...)
        if (child != current_node && m_nonEmptyNodesOfCurrentLevel.count(child) == 0) {
            insertIntoTree(child, currentLevel);
            m_nonEmptyNodesOfCurrentLevel.insert(child);
        }
    }

    // add the guard nodes (only if there are nodes at the next level at all)
    if (currentLevel < m_finalLevel - 1) {
        // For each nonEmptyParentNode its sibling nodes will be added in the next iteration.
        // However that is not enough to fullfill the rules of the octree where there must only
        // be a difference of 1 in level between nodes that share a vertex.
        // Hence we must add the remaining nodes manually
        for (const morton_t& guard : getMortonCodesForNeighbourOctants(parent, currentLevel + 1, m_depth)) {
            m_guardNodesOfCurrentLevel.insert(guard);
        }
    }
}

std::unique_ptr<Octree> SequentialOctreeBuilder::finishBuilding() {
    // Creates a SpaceFillingOctree in a bottom-up method:
    //  We iterate of each level (starting with the leafs).
    //  In each iteration we add the siblings of all non empty nodes.
    //  That is in the first iteration we add the siblings of all initial leaf-nodes.
    //  The parent nodes of the inserted leaf-nodes then become the non-empty nodes in the next level.
    //  Furthermore we add guard nodes (explained below) to ensure a level difference of 1

    m_tree = std::vector<std::unordered_set<morton_t>>(m_depth + 1);
    uint currentLevel = 0;
    m_finalLevel = std::min(maxLevel(), m_depth);

    for (const morton_t p : m_nonEmptyNodesOfCurrentLevel) {
        insertIntoTree(p, currentLevel);
    }

    for (; currentLevel < m_finalLevel; currentLevel++) {
        m_nonEmptyParentNodesOfCurrentLevel.clear();

        // The list of nodes that ensure a level difference of 1 between all nodes that have a common vertex.
        // Either these nodes (which are of the next level) or their child nodes must exist in the tree.
        m_guardNodesOfCurrentLevel.clear();

        NodeSet nonEmptyNodes = m_nonEmptyNodesOfCurrentLevel;

        // add the siblings of all non empty nodes
        for (const NodeSet::value_type& current_node : nonEmptyNodes) {
            createSiblingsAndGuardNodes(current_node, currentLevel);
        }

        // Add the guard nodes
        for (const morton_t guard : m_guardNodesOfCurrentLevel) {
            // In one line: checks that guard is not already in nonEmptyParentNodes (which would mean that its children are already part of the tree)
            // and if thats not the case add's it to the nonEmptyParentNodes list (this ensures that its siblings are added in the next iteration)
            if (m_nonEmptyParentNodesOfCurrentLevel.insert(guard).second) {
                insertIntoTree(guard, currentLevel + 1);
            }
        }

        // in the next level the current non-empty parent nodes are the next non-empty nodes
        m_nonEmptyNodesOfCurrentLevel = m_nonEmptyParentNodesOfCurrentLevel;
    }

    if (currentLevel != m_depth) {
        assert(currentLevel == maxLevel());

        // max level is capped... hence fill the empty parts of the octree with nodes of the current level
        coord_t nodeSize = getOctantSizeForLevel(currentLevel);
        Vector3i treeMaxXYZ = getMaxXYZForOctreeDepth(m_depth);

        for (coord_t x = 0; x < treeMaxXYZ.x(); x += nodeSize) {
            for (coord_t y = 0; y < treeMaxXYZ.y(); y += nodeSize) {
                for (coord_t z = 0; z < treeMaxXYZ.z(); z += nodeSize) {
                    morton_t node_llf = getMortonCodeForCoordinate(Vector3i(x, y, z));

                    if (m_nonEmptyNodesOfCurrentLevel.count(node_llf) == 0) {
                        insertIntoTree(node_llf, currentLevel);
                    }
                }
            }
        }
    }

    std::unique_ptr<Octree> result(new OctreeImpl(std::move(m_tree)));

    return result;
}
