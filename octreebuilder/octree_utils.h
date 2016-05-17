#pragma once

#include "octreebuilder_api.h"
#include "octreebuilder.h"

#include "octantid.h"
#include "linearoctree.h"

#include <vector>
#include <limits>


/**
 * @brief the octants at level 0 whose ancestors might violate the balance condition with respect to the given octant
 * @param octant the octant for which the balance condition is checked
 * @param octree a octree that contains this octant inside its bounds (does not have to be an element of the linear tree)
 * @return all level 0 octants with the specified condition that are inside the octree
 */
OCTREEBUILDER_API std::vector<OctantID> getSearchKeys(const OctantID& octant, const LinearOctree& octree);


/**
 * @brief 2:1 balances an incomplete unbalanced octree
 * @param octree the sorted unbalanced octree (can be incomplete)
 * @return a 2:1 balanced octree (might be incomplete depending on the input)
 *
 * The octree must not be complete but should contain all unbalanced octants and those that might be affected by the balancing.
 */
OCTREEBUILDER_API LinearOctree balanceTree(const LinearOctree& octree);

/**
 * @brief creates a 2:1 balanced octree from a set of level 0 leafs
 * @param tree the incomplete tree. Must only contain leafs of level 0.
 * @param maxLevel the maximum level of all octants in the balanced tree
 */
OCTREEBUILDER_API void createBalancedSubtree(LinearOctree& tree, uint maxLevel = std::numeric_limits<uint>::max());

/**
 * @brief creates a 2:1 balanced octree from a set of level 0 leafs
 * @param root the root of the incomplete tree
 * @param levelZeroLeafs the level zero leafs
 * @param maxLevel the maximum level of all octants in the balanced tree
 * @return the balanced octree
 */
OCTREEBUILDER_API LinearOctree
createBalancedSubtree(const OctantID& root, const std::vector<OctantID>& levelZeroLeafs, uint maxLevel = std::numeric_limits<uint>::max());

/**
 * @brief The Partition struct represents a partition of an octree into non overlapping subtrees
 */
struct Partition {
    Partition(const OctantID& rootOctant, const std::vector<LinearOctree>& partitionList);
    OctantID root;
    std::vector<LinearOctree> partitions;
};

/**
 * @brief computes a partition of an incomplete octree for a parallel creation, so that the creation effort is balanced over all threads and communication is minimized
 * @param globalRoot the root of the incomplete octree
 * @param levelZeroLeafs the level zero leafs of the incomplete octree
 * @param numThreads the number of threads used for parallel creation
 * @return a partition of the incomplete octree. Each subtree contains the level zero leafs inside its bounds.
 */
OCTREEBUILDER_API Partition computePartition(const OctantID& globalRoot, const std::vector<OctantID>& levelZeroLeafs, int numThreads);

/**
 * @brief creates all octants between start and end so that the level of each octant is maximal
 * @param start the start octant (start < end)
 * @param end the end octant
 * @return the ordered octants with maximum possible level between start and end so that for each octant O: start < O < end
 *
 */
OCTREEBUILDER_API std::vector<OctantID> completeRegion(const OctantID& start, const OctantID& end);

/**
 * @brief retains the highest level octants of each region and fills the remaining space with octants that are as coarse as possible (max level). The octants are called blocks.
 * @param globalRoot the root of the octree that will contains all blocks
 * @param completedRegions a list of completed regions
 * @return the global octree with the blocks as its leafs
 */
OCTREEBUILDER_API LinearOctree computeBlocksFromRegions(const OctantID& globalRoot, std::vector<std::vector<OctantID>> completedRegions);

/**
 * @brief computes the octant of minimal level that contains a and b.
 * @return the nearest common ancestor of a and b
 */
OCTREEBUILDER_API OctantID nearestCommonAncestor(const OctantID& a, const OctantID& b);

/**
 * @brief creates the octants that are decendants of the keys at the lowest level and then completes the subtree
 * @param root the root of the subtree
 * @param lowestLevel the level of the octants created from the keys
 * @param keys octansts of level zero in the tree
 * @return the complete 2:1 balanced subtree that contains all ancestors of all keys at lowestlevel
 */
OCTREEBUILDER_API std::vector<OctantID> completeSubtree(const OctantID& root, uint lowestLevel, const std::unordered_set<OctantID>& keys);

/**
 * @brief creates a 2:1 blanced octree from a set of level zero leafs in parallel
 * @param root the root of the octree
 * @param levelZeroLeafs the level zero leafs
 * @param numThreads the number of threads used
 * @param maxLevel the maximum level of all leafs in the final 2:1 balanced octree
 * @return the complete 2:1 blanaced octree
 *
 * The final octree contains all level zero leafs the remaining space is covered with the minimum number of non-overlapping octants
 */
OCTREEBUILDER_API LinearOctree createBalancedOctreeParallel(const OctantID& root, const std::vector<OctantID>& levelZeroLeafs, int numThreads,
                                                      uint maxLevel = std::numeric_limits<uint>::max());
