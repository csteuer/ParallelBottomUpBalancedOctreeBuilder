#include "octree.h"

#include <ostream>

namespace octreebuilder {

::std::ostream& operator<<(::std::ostream& s, const OctreeNode& n) {
    s << "{ llf: " << n.getLLF() << ", level: " << n.getLevel() << ", size: " << n.getSize() << ", morton_llf: " << n.getMortonEncodedLLF() << " }";
    s << ::std::endl;
    return s;
}

::std::ostream& operator<<(::std::ostream& s, const Octree& tree) {
    s << "{ depth: " << tree.getDepth() << ", maxLevel: " << tree.getMaxLevel() << ", maxXYZ: " << tree.getMaxXYZ() << ", numNodes: " << tree.getNumNodes()
      << " }";
    return s;
}

::std::ostream& operator<<(::std::ostream& s, const ::std::unique_ptr<Octree>& tree) {
    if (tree != nullptr) {
        s << *tree;
    } else {
        s << "NULL";
    }
    return s;
}

Octree::~Octree() {
}

::std::ostream& operator<<(::std::ostream& s, const Octree::OctreeState& octreeState) {
    switch (octreeState) {
        case Octree::OctreeState::INCOMPLETE:
            s << "INCOMPLETE";
            break;
        case Octree::OctreeState::OVERLAPPING:
            s << "OVERLAPPING";
            break;
        case Octree::OctreeState::UNBALANCED:
            s << "UNBALANCED";
            break;
        case Octree::OctreeState::UNSORTED:
            s << "UNSORTED";
            break;
        case Octree::OctreeState::VALID:
            s << "VALID";
            break;
    }

    return s;
}
}
