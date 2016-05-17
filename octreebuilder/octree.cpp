#include "octree.h"

#include <ostream>

std::ostream& operator<<(std::ostream& s, const OctreeNode& n) {
    s << "{ llf: " << n.getLLF() << ", level: " << n.getLevel() << ", size: " << n.getSize() << ", morton_llf: " << n.getMortonEncodedLLF() << " }";
    s << std::endl;
    return s;
}

std::ostream& operator<<(std::ostream& s, const Octree& tree) {
    s << "{ depth: " << tree.getDepth() << ", maxLevel: " << tree.getMaxLevel() << ", maxXYZ: " << tree.getMaxXYZ() << ", numNodes: " << tree.getNumNodes()
      << "}";
    return s;
}

std::ostream& operator<<(std::ostream& s, const std::unique_ptr<Octree>& tree) {
    if (tree != nullptr) {
        s << *tree;
    } else {
        s << "NULL";
    }
    return s;
}

Octree::~Octree() {
}
