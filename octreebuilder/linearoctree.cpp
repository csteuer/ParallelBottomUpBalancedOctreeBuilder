#include "linearoctree.h"

#include "octantid.h"
#include "parallel_stable_sort.h"

#include <algorithm>
#include <ostream>
#include <assert.h>

std::ostream& operator<<(std::ostream& s, const LinearOctree& octree) {
    s << "Octree with layers. Size: " << octree.leafs().size() << std::endl;
    for (uint l = 0; l < octree.depth(); l++) {
        s << "Level " << l << " leafs: " << std::endl;
        for (const OctantID& octant : octree.leafs()) {
            if (octant.level() == l) {
                s << "  " << octant << std::endl;
            }
        }
    }
    return s;
}

LinearOctree::LinearOctree() {
}

LinearOctree::LinearOctree(const OctantID& root, const container_type& leafs) : m_root(root), m_leafs(leafs) {
    m_deepestLastDecendant = OctantID(getMaxXYZForOctreeDepth(root.level()) + root.coord(), 0);
}

const OctantID& LinearOctree::root() const {
    return m_root;
}

uint LinearOctree::depth() const {
    return m_root.level();
}

const LinearOctree::container_type& LinearOctree::leafs() const {
    return m_leafs;
}

void LinearOctree::insert(const OctantID& octant) {
    if (octant < m_root || octant > m_deepestLastDecendant) {
        throw std::runtime_error("LinearOctree::insert: Invalid parameter octant out of bounds.");
    }

    assert(std::find(m_leafs.begin(), m_leafs.end(), octant) == m_leafs.end());
    m_leafs.push_back(octant);
}

bool LinearOctree::hasLeaf(const OctantID& octant) const {
    if (!insideTreeBounds(octant)) {
        return false;
    }

    return std::find(m_leafs.begin(), m_leafs.end(), octant) != m_leafs.end();
}

std::vector<OctantID> LinearOctree::replaceWithChildren(const OctantID& octant) {
    auto children = octant.children();

    replaceWithSubtree(octant, children);

    return children;
}

void LinearOctree::replaceWithSubtree(const OctantID& octant, const std::vector<OctantID>& subtree) {

    if (!insideTreeBounds(octant)) {
        throw std::runtime_error("LinearOctree::replaceWithSubtree: Invalid parameter octant out of bounds.");
    }

    if (m_toRemove.insert(std::make_pair(octant.mcode(), octant.level())).second) {
        m_leafs.insert(m_leafs.end(), subtree.begin(), subtree.end());
    }
}

bool LinearOctree::maximumLowerBound(const OctantID& octant, OctantID& lowerBound) const {
    auto it = std::lower_bound(m_leafs.begin(), m_leafs.end(), octant);

    if (it != m_leafs.begin()) {
        --it;
    } else {
        return false;
    }

    lowerBound = *it;

    return true;
}

bool LinearOctree::insideTreeBounds(const OctantID& octant) const {
    return octant >= m_root && octant <= m_deepestLastDecendant;
}

OctantID LinearOctree::deepestLastDecendant() const {
    return m_deepestLastDecendant;
}

OctantID LinearOctree::deepestFirstDecendant() const {
    OctantID dfd(root().mcode(), 0);
    return dfd;
}

void LinearOctree::sortAndRemove() {
    if (!m_toRemove.empty()) {
        m_leafs.erase(std::remove_if(m_leafs.begin(), m_leafs.end(), [this](const OctantID& octant) {
            auto it = m_toRemove.find(octant.mcode());
            if (it != m_toRemove.end() && it->second == octant.level()) {
                return true;
            }
            return false;
        }), m_leafs.end());
        m_toRemove.clear();
    }
    pss::parallel_stable_sort(m_leafs.begin(), m_leafs.end());
}
