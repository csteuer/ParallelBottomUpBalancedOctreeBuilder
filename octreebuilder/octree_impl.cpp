#include "octree_impl.h"

#include <cmath>
#include <algorithm>
#include <functional>
#include <atomic>

#include "octree.h"
#include "box.h"
#include "linearoctree.h"
#include "octantid.h"
#include "mortoncode_utils.h"

#include "perfcounter.h"
#include <iostream>

namespace octreebuilder {

OctreeImpl::OctreeImpl(::std::vector<::std::unordered_set<morton_t>> tree) : m_tree(::std::move(tree)) {
    size_t numLeafs = 0;
    for (const auto& leafSet : tree) {
        numLeafs += leafSet.size();
    }

    const uint depth = static_cast<uint>(m_tree.size() - 1);
    m_linearTree = LinearOctree(OctantID(0, depth), numLeafs);
    m_bounding = Box(getMaxXYZForOctreeDepth(depth));

    for (uint l = 0; l < m_tree.size(); l++) {
        for (const morton_t& mcode : m_tree.at(l)) {
            m_linearTree.insert(OctantID(mcode, l));
        }
    }

    m_linearTree.sortAndRemove();
}

OctreeImpl::OctreeImpl(LinearOctree&& linearOctree) : m_bounding(Box(getMaxXYZForOctreeDepth(linearOctree.depth()))) {
    PerfCounter perfCounter;

    perfCounter.start();
    m_tree = ::std::vector<::std::unordered_set<morton_t>>(linearOctree.depth() + 1);
    {
        ::std::vector<size_t> numLeafsPerLevel(linearOctree.depth() + 1, 0);
        for (const OctantID& node : linearOctree.leafs()) {
            numLeafsPerLevel.at(node.level())++;
        }

        for (size_t i = 0; i < m_tree.size(); i++) {
            m_tree.at(i).reserve(numLeafsPerLevel.at(i));
        }
    }
    LOG_PROF("Allocated set tree: " << perfCounter);

    perfCounter.start();
    for (const OctantID& node : linearOctree.leafs()) {
        m_tree.at(node.level()).insert(node.mcode());
    }
    LOG_PROF("Filled set tree: " << perfCounter);

    m_linearTree = ::std::move(linearOctree);
}

Vector3i OctreeImpl::getMaxXYZ() const {
    return m_bounding.urb();
}

uint OctreeImpl::getDepth() const {
    return m_linearTree.depth();
}

uint OctreeImpl::getMaxLevel() const {
    for (uint level = getDepth(); level > 0; level--) {
        if (!m_tree.at(level).empty()) {
            return level;
        }
    }

    if (!m_tree.empty() && !m_tree.front().empty()) {
        return 0;
    }

    throw ::std::runtime_error("Can't determine the maximum level of an empty octree.");
}

size_t OctreeImpl::getNumNodes() const {
    return m_linearTree.leafs().size();
}

OctreeNode OctreeImpl::getNode(const size_t& i) const {
    const OctantID& octant = m_linearTree.leafs().at(i);

    OctreeNode n(octant.mcode(), octant.level());

    return n;
}

OctreeNode OctreeImpl::tryGetNodeAt(const Vector3i& llf, uint level) const {
    if (level >= m_tree.size()) {
        return OctreeNode();
    }

    morton_t mcode = getMortonCodeForCoordinate(llf);

    if (m_tree.at(level).count(mcode) == 1) {
        return OctreeNode(mcode, level);
    }

    return OctreeNode();
}

::std::vector<OctreeNode> OctreeImpl::getNeighbourNodes(const OctreeNode& n, OctreeNode::Face sharedFace) const {
    ::std::vector<OctreeNode> neighbourNodes;

    if (n.getLevel() == getDepth()) {
        // n is root node... no neighbours (note: this should rarely happen as it means that the tree is empty)
        return neighbourNodes;
    }

    // we only have to check 3 levels for neighbours: the parent level, the same level and the child level (in respect to n's level)
    // (this is because the level difference of adjacent nodes must never be greater 1)

    // check for neighbours on the same level
    Vector3i neighbourLLF = n.getLLF() + OctreeNode::getNormalOfFace(sharedFace) * n.getSize();

    if (!m_bounding.contains(neighbourLLF)) {
        // if the direct neighbour of n (wether it exists or not) is outside of the tree then neither the neighbours on the child level nor
        // on the parent level are inside the tree and hence can't exist
        return neighbourNodes;
    }

    OctantID possibleNeighbour(neighbourLLF, n.getLevel());

    if (m_tree.at(possibleNeighbour.level()).count(possibleNeighbour.mcode()) > 0) {
        neighbourNodes.push_back(OctreeNode(possibleNeighbour.mcode(), possibleNeighbour.level()));
        return neighbourNodes;
    } else if (m_tree.at(possibleNeighbour.level() + 1).count(possibleNeighbour.mcode())) {
        neighbourNodes.push_back(OctreeNode(possibleNeighbour.mcode(), possibleNeighbour.level() + 1));
        return neighbourNodes;
    }

    OctantID possibleParentNeighbour = possibleNeighbour.parent();

    if (m_tree.at(possibleParentNeighbour.level()).count(possibleParentNeighbour.mcode()) > 0) {
        neighbourNodes.push_back(OctreeNode(possibleParentNeighbour.mcode(), possibleParentNeighbour.level()));
        return neighbourNodes;
    }

    // check child level... obviously the neighbours at the child level must be children of the neighbour node n's level
    const ::std::array<morton_t, 8> possibleChildren = getMortonCodesForChildren(possibleNeighbour.mcode(), possibleNeighbour.level());
    ::std::array<size_t, 4> neighbourChildrenIndices;

    // get the indices of the 4 neighbour nodes in possibleChildren
    switch (sharedFace) {
        case OctreeNode::LEFT:
            neighbourChildrenIndices = {{4, 5, 6, 7}};
            break;
        case OctreeNode::RIGHT:
            neighbourChildrenIndices = {{0, 1, 2, 3}};
            break;
        case OctreeNode::FRONT:
            neighbourChildrenIndices = {{2, 3, 6, 7}};
            break;
        case OctreeNode::BACK:
            neighbourChildrenIndices = {{0, 1, 4, 5}};
            break;
        case OctreeNode::BOTTOM:
            neighbourChildrenIndices = {{1, 3, 5, 7}};
            break;
        case OctreeNode::TOP:
            neighbourChildrenIndices = {{0, 2, 4, 6}};
            break;
    }

    uint childLevel = n.getLevel() - 1;
    neighbourNodes.reserve(4);
    for (const size_t& index : neighbourChildrenIndices) {
        morton_t childNeighbourCode = possibleChildren.at(index);

        OctreeNode childNode(childNeighbourCode, childLevel);

        if (m_tree.at(childLevel).count(childNeighbourCode) == 0) {
            // a neighbour must exist in a valid tree (we have checked above that n is not at the boundary)... since
            // neither a neighbour on the same level nor on the parent level exists there must be
            // 4 neighbours at the child level
            throw ::std::runtime_error("Invalid parameter 'n' or invalid octree.");
        }

        neighbourNodes.push_back(childNode);
    }

    return neighbourNodes;
}

Octree::OctreeState OctreeImpl::checkState() const {
    if (getDepth() == 0) {
        return Octree::OctreeState::VALID;
    }

    // Check sorted
    if (!m_linearTree.leafs().empty()) {
        for (size_t i = 0; i < m_linearTree.leafs().size() - 1; i++) {
            if (m_linearTree.leafs().at(i) > m_linearTree.leafs().at(i + 1)) {
                return Octree::OctreeState::UNSORTED;
            }
        }
    }

    // Check complete and not overlapping
    if (m_linearTree.leafs().empty() || m_linearTree.leafs().front().mcode() != 0) {
        return Octree::OctreeState::INCOMPLETE;
    }

    if (LinearOctree(m_linearTree.leafs().back()).deepestLastDecendant() != m_linearTree.deepestLastDecendant()) {
        return Octree::OctreeState::INCOMPLETE;
    }

    for (size_t i = 0; i < m_linearTree.leafs().size() - 1; i++) {
        morton_t nextExpectedMortonCode = LinearOctree(m_linearTree.leafs().at(i)).deepestLastDecendant().mcode() + 1;
        morton_t nextMortonCode = m_linearTree.leafs().at(i + 1).mcode();

        if (nextMortonCode > nextExpectedMortonCode) {
            return Octree::OctreeState::INCOMPLETE;
        } else if (nextMortonCode < nextExpectedMortonCode) {
            return Octree::OctreeState::OVERLAPPING;
        }
    }

    // Check balanced
    for (size_t i = 0; i < m_linearTree.leafs().size(); i++) {
        const OctantID& octant = m_linearTree.leafs().at(i);

        for (const OctantID& searchKey : octant.getSearchKeys(m_linearTree)) {
            OctantID neighbour;
            if (!m_linearTree.maximumLowerBound(searchKey, neighbour) || !searchKey.isDecendantOf(neighbour)) {
                continue;
            }

            if ((neighbour.level() > octant.level() && neighbour.level() - octant.level() > 1) ||
                (neighbour.level() < octant.level() && octant.level() - neighbour.level() > 1)) {
                return Octree::OctreeState::UNBALANCED;
            }
        }
    }

    return Octree::OctreeState::VALID;
}
}
