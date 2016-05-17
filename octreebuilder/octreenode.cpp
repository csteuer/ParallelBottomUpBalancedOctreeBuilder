#include "octreenode.h"

#include <limits>
#include <ostream>

OctreeNode::OctreeNode() : m_morton_llf(std::numeric_limits<morton_t>::max()), m_level(std::numeric_limits<uint>::max()) {
}

OctreeNode::OctreeNode(morton_t morton_encoded_llf, uint level)
    : m_morton_llf(morton_encoded_llf), m_level(level) {
}

Vector3i OctreeNode::getNormalOfFace(const OctreeNode::Face& f) {
    switch (f) {
        case LEFT:
            return Vector3i(-1, 0, 0);
        case RIGHT:
            return Vector3i(1, 0, 0);
        case FRONT:
            return Vector3i(0, -1, 0);
        case BACK:
            return Vector3i(0, 1, 0);
        case BOTTOM:
            return Vector3i(0, 0, -1);
        case TOP:
            return Vector3i(0, 0, 1);
    }
    return Vector3i(0, 0, 0);
}

bool OctreeNode::isValid() const {
    return m_morton_llf != static_cast<morton_t>(-1) && m_level != static_cast<uint>(-1);
}

Vector3i OctreeNode::getLLF() const {
    return getCoordinateForMortonCode(m_morton_llf);
}

morton_t OctreeNode::getMortonEncodedLLF() const {
    return m_morton_llf;
}

coord_t OctreeNode::getStableIndex() const {
    return static_cast<coord_t>(m_morton_llf);
}

coord_t OctreeNode::getSize() const {
    return getOctantSizeForLevel(m_level);
}

uint OctreeNode::getLevel() const {
    return m_level;
}

bool OctreeNode::operator==(const OctreeNode& o) const {
    if (!this->isValid() || !o.isValid()) {
        return false;
    }

    return m_level == o.getLevel() && m_morton_llf == o.getMortonEncodedLLF();
}

bool OctreeNode::operator!=(const OctreeNode& o) const {
    if (!this->isValid() || !o.isValid()) {
        return true;
    }

    return m_level != o.getLevel() && m_morton_llf != o.getMortonEncodedLLF();
}

std::ostream& operator<<(std::ostream& s, const OctreeNode::Face& f) {
    switch (f) {
        case OctreeNode::Face::LEFT:
            s << "LEFT";
            break;
        case OctreeNode::Face::RIGHT:
            s << "RIGHT";
            break;
        case OctreeNode::Face::FRONT:
            s << "FRONT";
            break;
        case OctreeNode::Face::BACK:
            s << "BACK";
            break;
        case OctreeNode::Face::TOP:
            s << "TOP";
            break;
        case OctreeNode::Face::BOTTOM:
            s << "BOTTOM";
            break;
    }

    return s;
}
