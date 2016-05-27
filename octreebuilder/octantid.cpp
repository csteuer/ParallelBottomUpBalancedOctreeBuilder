#include "octantid.h"

#include "linearoctree.h"
#include "mortoncode_utils.h"

#include <ostream>
#include <assert.h>

namespace octreebuilder {

OctantID::OctantID() : m_mcode(0), m_level(0) {
}

OctantID::OctantID(const Vector3i& coord, uint level) : m_mcode(getMortonCodeForCoordinate(coord)), m_level(level) {
}

OctantID::OctantID(morton_t mcode, uint level) : m_mcode(mcode), m_level(level) {
}

morton_t OctantID::mcode() const {
    return m_mcode;
}

uint OctantID::level() const {
    return m_level;
}

Vector3i OctantID::coord() const {
    return getCoordinateForMortonCode(m_mcode);
}

OctantID OctantID::parent() const {
    return OctantID(getMortonCodeForParent(m_mcode, m_level), m_level + 1);
}

OctantID OctantID::ancestorAtLevel(uint level) const {
    morton_t ancestorCode = getMortonCodeForAncestor(m_mcode, m_level, level);

    return OctantID(ancestorCode, level);
}

::std::vector<OctantID> OctantID::children() const {
    if (m_level == 0) {
        throw ::std::runtime_error("OctantID::children: A level 0 node has no children.");
    }

    ::std::vector<OctantID> result;
    result.reserve(8);

    auto mcodes = getMortonCodesForChildren(m_mcode, m_level);

    assert(mcodes.size() == 8);

    uint childLevel = m_level - 1;

    for (const morton_t& mcode : mcodes) {
        result.push_back(OctantID(mcode, childLevel));
    }

    return result;
}

bool OctantID::isDecendantOf(const OctantID& possibleAncestor) const {
    return isMortonCodeDecendant(m_mcode, m_level, possibleAncestor.m_mcode, possibleAncestor.m_level);
}

::std::vector<OctantID> OctantID::potentialNeighbours(const LinearOctree& octree) const {
    ::std::vector<OctantID> result;

    if (!octree.insideTreeBounds(*this)) {
        return result;
    }

    result.reserve(26);

    const coord_t octantSize = getOctantSizeForLevel(m_level);

    Vector3i thisOctantLLF = this->coord();

    for (const Vector3i& offset :
         {Vector3i(-1, -1, -1), Vector3i(0, -1, -1), Vector3i(1, -1, -1), Vector3i(-1, 0, -1), Vector3i(0, 0, -1), Vector3i(1, 0, -1), Vector3i(-1, 1, -1),
          Vector3i(0, 1, -1), Vector3i(1, 1, -1), Vector3i(-1, -1, 0), Vector3i(0, -1, 0), Vector3i(1, -1, 0), Vector3i(-1, 0, 0), Vector3i(1, 0, 0),
          Vector3i(-1, 1, 0), Vector3i(0, 1, 0), Vector3i(1, 1, 0), Vector3i(-1, -1, 1), Vector3i(0, -1, 1), Vector3i(1, -1, 1), Vector3i(-1, 0, 1),
          Vector3i(0, 0, 1), Vector3i(1, 0, 1), Vector3i(-1, 1, 1), Vector3i(0, 1, 1), Vector3i(1, 1, 1)}) {
        Vector3i neighbourLLF = thisOctantLLF + offset * octantSize;
        OctantID neighbour(neighbourLLF, m_level);

        if (octree.insideTreeBounds(neighbour)) {
            result.push_back(neighbour);
        }
    }

    return result;
}

::std::vector<OctantID> OctantID::potentialNeighboursWithoutSiblings(const LinearOctree& octree) const {
    ::std::vector<OctantID> result;

    if (!octree.insideTreeBounds(*this)) {
        return result;
    }

    result.reserve(19);

    const coord_t octantSize = getOctantSizeForLevel(m_level);

    Vector3i thisOctantLLF = this->coord();

    OctantID thisOctantsParent = this->parent();

    for (const Vector3i& offset :
         {Vector3i(-1, -1, -1), Vector3i(0, -1, -1), Vector3i(1, -1, -1), Vector3i(-1, 0, -1), Vector3i(0, 0, -1), Vector3i(1, 0, -1), Vector3i(-1, 1, -1),
          Vector3i(0, 1, -1), Vector3i(1, 1, -1), Vector3i(-1, -1, 0), Vector3i(0, -1, 0), Vector3i(1, -1, 0), Vector3i(-1, 0, 0), Vector3i(1, 0, 0),
          Vector3i(-1, 1, 0), Vector3i(0, 1, 0), Vector3i(1, 1, 0), Vector3i(-1, -1, 1), Vector3i(0, -1, 1), Vector3i(1, -1, 1), Vector3i(-1, 0, 1),
          Vector3i(0, 0, 1), Vector3i(1, 0, 1), Vector3i(-1, 1, 1), Vector3i(0, 1, 1), Vector3i(1, 1, 1)}) {
        Vector3i neighbourLLF = thisOctantLLF + offset * octantSize;
        OctantID neighbour(neighbourLLF, m_level);

        if (octree.insideTreeBounds(neighbour) && neighbour.parent() != thisOctantsParent) {
            result.push_back(neighbour);
        }
    }

    return result;
}

static bool componentwiseAEqBAndNotEqC(const Vector3i& a, const Vector3i& b, const Vector3i& c) {
    return (a.x() == b.x() && a.x() != c.x()) || (a.y() == b.y() && a.y() != c.y()) || (a.z() == b.z() && a.z() != c.z());
}

bool OctantID::isBoundaryOctant(const LinearOctree& block, const LinearOctree& globalTree) const {
    assert(globalTree.insideTreeBounds(block.root()) && block.insideTreeBounds(*this));

    const coord_t treeSize = getOctantSizeForLevel(globalTree.depth());
    const Vector3i treeLLF = globalTree.root().coord();
    const Vector3i treeURB = treeLLF + Vector3i(treeSize);

    const coord_t blockSize = getOctantSizeForLevel(block.depth());
    const Vector3i blockLLF = block.root().coord();
    const Vector3i blockURB = blockLLF + Vector3i(blockSize);

    return isBoundaryOctant(blockLLF, blockURB, treeLLF, treeURB);
}

bool OctantID::isBoundaryOctant(const Vector3i& blockLLF, const Vector3i& blockURB, const Vector3i& treeLLF, const Vector3i& treeURB) const {
    const coord_t octantSize = getOctantSizeForLevel(m_level);
    const Vector3i octantLLF = this->coord();
    const Vector3i octantURB = octantLLF + Vector3i(octantSize);

    return componentwiseAEqBAndNotEqC(octantLLF, blockLLF, treeLLF) || componentwiseAEqBAndNotEqC(octantURB, blockURB, treeURB);
}

::std::vector<OctantID> OctantID::getSearchKeys(const LinearOctree& octree) const {
    ::std::vector<OctantID> searchKeys;
    searchKeys.reserve(7);

    Vector3i searchCorner = getSearchCorner(this->mcode(), this->level());

    for (const Vector3i& coord : {Vector3i(0, 0, 0), Vector3i(-1, 0, 0), Vector3i(0, -1, 0), Vector3i(-1, -1, 0), Vector3i(0, 0, -1), Vector3i(-1, 0, -1),
                                  Vector3i(0, -1, -1), Vector3i(-1, -1, -1)}) {
        Vector3i searchKeyLLF = searchCorner + coord;
        OctantID searchKey(searchKeyLLF, 0);

        if (searchKey != *this && octree.insideTreeBounds(searchKey) && !searchKey.isDecendantOf(*this)) {
            searchKeys.push_back(searchKey);
        }
    }

    return searchKeys;
}

bool operator<(const OctantID& left, const OctantID& right) {
    if (left.mcode() != right.mcode()) {
        return left.mcode() < right.mcode();
    }
    return left.level() > right.level();
}

bool operator<=(const OctantID& left, const OctantID& right) {
    if (left.mcode() != right.mcode()) {
        return left.mcode() < right.mcode();
    }
    return left.level() >= right.level();
}

bool operator>(const OctantID& left, const OctantID& right) {
    if (left.mcode() != right.mcode()) {
        return left.mcode() > right.mcode();
    }
    return left.level() < right.level();
}

bool operator>=(const OctantID& left, const OctantID& right) {
    if (left.mcode() != right.mcode()) {
        return left.mcode() > right.mcode();
    }
    return left.level() <= right.level();
}

bool operator==(const OctantID& a, const OctantID& b) {
    return a.level() == b.level() && a.mcode() == b.mcode();
}

bool operator!=(const OctantID& a, const OctantID& b) {
    return a.level() != b.level() || a.mcode() != b.mcode();
}

::std::ostream& operator<<(::std::ostream& s, const OctantID& n) {
    s << "{ mcode: " << n.mcode() << ", llf: " << n.coord() << ", level: " << n.level() << "}";
    return s;
}
}
