#pragma once

#include "octreebuilder_api.h"


#include <vector>
#include <unordered_map>
#include <iosfwd>

#include "octantid.h"

/**
 * @brief The LinearOctree class stores a list of octants that are inside the tree's bounds.
 *
 * Octants are identified by thier morton code and level (OctantID).
 * Usually only the leafs are stored. However this is not guaranteed by the data structure.
 */
class OCTREEBUILDER_API LinearOctree {
public:
    typedef std::vector<OctantID> container_type;

    LinearOctree();
    LinearOctree(const OctantID& m_root, const container_type& leafs = {});

    /**
     * @brief the root of the tree.
     *
     * Defines the bounds of the tree.
     * Hence all stored octants are decendants of the root.
     */
    const OctantID& root() const;

    /**
     * @brief the depth of the tree (distance from root node to level 0).
     */
    uint depth() const;

    /**
     * @brief the id's of the octants stored in the tree.
     */
    const container_type& leafs() const;

    /**
     * @brief adds the octant to the tree (as the last item).
     * @param octant the id of the octant.
     *
     * Its not allowed to add duplicates (however its not checked in release mode).
     */
    void insert(const OctantID& octant);

    /**
     * @brief checks if the octant is stored in the linear tree.
     * @param octant the id of the octant.
     * @return true if octant is stored inside the tree and false otherwise.
     * @pre sortAndRemove must have been called before without calling a non const method in between.
     */
    bool hasLeaf(const OctantID& octant) const;

    /**
     * @brief replaces a octant with its 8 children.
     * @param octant the octant to replace.
     * @return the ids of the children.
     * @note The octant is only marked for removal and is still returned by the leafs method. To finally erase it sortAndRemove has to be called.
     *
     * Ignored if called twice for the same octant without calling sortAndRemove in between.
     * The octant to replace doesn't have to exist in the list, however its more efficient
     * to use the insert method in that case.
     */
    std::vector<OctantID> replaceWithChildren(const OctantID& octant);

    /**
     * @brief replaces an octant with the given subtree.
     * @param octant the id of the octant to replace.
     * @param subtree the octants of the subtree.
     * @note The octant is only marked for removal and is still returned by the leafs method. To finally erase it sortAndRemove has to be called.
     *
     * Each OctantID in the subtree should be a descendant of octant.
     * However this is not checked.
     * Ignored if called twice for the same octant without calling sortAndRemove in between.
     * The octant to replace doesn't have to exist in the list, however its more efficient
     * to use the insert method in that case.
     */
    void replaceWithSubtree(const OctantID& octant, const std::vector<OctantID>& subtree);

    /**
     * @brief for an octant A find the octant B in the linear tree, so that A > B and B is maximal (there is no other C in the tree with A > C and C > B).
     * @param octant the octant for which the lower bounds will be found.
     * @param lowerBound on success will refer to the lower bound of octant when the function returns. Otherwise the reference will be unchanged.
     * @return true if the search was successfull, false otherwise (there is no lower bound).
     * @pre sortAndRemove must have been called before without calling a non const method in between.
     */
    bool maximumLowerBound(const OctantID& octant, OctantID& lowerBound) const;

    /**
     * @brief checks wether the octant is inside the bounds of the tree.
     * @param octant the id of the octant.
     * @return true if the octant is inside the tree bounds, false otherwise.
     */
    bool insideTreeBounds(const OctantID& octant) const;

    /**
     * @brief the octant with the maximal id (level zero and maximal morton code) that is inside the tree bounds.
     * @note Does not depend on the octants stored octants.
     */
    OctantID deepestLastDecendant() const;

    /**
     * @brief the octant of level zero with the minimal id that is inside the tree bounds.
     * @note Does not depend on the octants stored octants.
     */
    OctantID deepestFirstDecendant() const;

    /**
     * @brief sorts the stored octants in ascending order by their id and erases all octants that where marked for removal.
     */
    void sortAndRemove();

private:
    OctantID m_root;
    OctantID m_deepestLastDecendant;
    container_type m_leafs;
    std::unordered_map<morton_t, uint> m_toRemove;
};

OCTREEBUILDER_API std::ostream& operator<<(std::ostream& s, const LinearOctree& octree);
