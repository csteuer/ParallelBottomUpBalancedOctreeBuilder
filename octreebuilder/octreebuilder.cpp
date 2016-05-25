#include "octreebuilder.h"

namespace octreebuilder {

OctreeBuilder::OctreeBuilder(const Vector3i& maxXYZ, uint maxLevel) : m_maxXYZ(maxXYZ), m_maxLevel(maxLevel) {
}

uint OctreeBuilder::maxLevel() {
    return m_maxLevel;
}

OctreeBuilder::~OctreeBuilder() {
}
}
