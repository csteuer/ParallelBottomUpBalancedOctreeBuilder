#pragma once

#include "octreebuilder_api.h"
#include "octreebuilder.h"

#include "octantid.h"
#include "linearoctree.h"

#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <limits>

namespace octreebuilder {
/**
 * @brief 2:1 balances an incomplete unbalanced octree
 * @param octree The sorted unbalanced octree (can be incomplete)
 * @return A 2:1 balanced octree (might be incomplete depending on the input)
 *
 * The octree must not be complete but should contain all unbalanced octants and those that might be affected by the balancing.
 */
OCTREEBUILDER_API LinearOctree balanceTree(const LinearOctree& octree);

/**
 * @brief Creates a 2:1 balanced octree from a set of level 0 leafs
 * @param tree The incomplete tree. Must only contain leafs of level 0.
 * @param maxLevel The maximum level of all octants in the balanced tree
 */
OCTREEBUILDER_API void createBalancedSubtree(LinearOctree& tree, uint maxLevel = ::std::numeric_limits<uint>::max());

/**
 * @brief Creates a 2:1 balanced octree from a set of level 0 leafs
 * @param root The root of the incomplete tree
 * @param levelZeroLeafs The level zero leafs
 * @param maxLevel The maximum level of all octants in the balanced tree
 * @return The balanced octree
 */
OCTREEBUILDER_API LinearOctree
createBalancedSubtree(const OctantID& root, const ::std::vector<OctantID>& levelZeroLeafs, uint maxLevel = ::std::numeric_limits<uint>::max());

/**
 * @brief The Partition struct represents a partition of an octree into non overlapping subtrees
 */
struct OCTREEBUILDER_API Partition {
    Partition(const OctantID& rootOctant, const ::std::vector<LinearOctree>& partitionList);
    OctantID root;
    ::std::vector<LinearOctree> partitions;
};

/**
 * @brief Computes a partition of an incomplete octree for a parallel creation, so that the creation effort is balanced over all threads and communication is
 * minimized
 * @param globalRoot The root of the incomplete octree
 * @param levelZeroLeafs The level zero leafs of the incomplete octree
 * @param numThreads The number of threads used for parallel creation
 * @return A partition of the incomplete octree. Each subtree contains the level zero leafs inside its bounds.
 */
OCTREEBUILDER_API Partition computePartition(const OctantID& globalRoot, const ::std::vector<OctantID>& levelZeroLeafs, const int numThreads);

/**
 * @brief merges The unbalanced complete tree with the balanced incomplete tree
 * @param unbalancedTree A complete sorted unbalanced tree
 * @param balancedTree A incomplete sorted balanced tree
 * @return The merged tree. If balancedTree contains leafs for every unbalanced leaf in unbalancedTree the result is a balanced complete tree.
 */
OCTREEBUILDER_API LinearOctree mergeUnbalancedCompleteTreeWithBalancedIncompleteTree(const LinearOctree& unbalancedTree, const LinearOctree& balancedTree);

/**
 * @brief Creates all octants between start and end so that the level of each octant is maximal
 * @param start The start octant (start < end)
 * @param end The end octant
 * @return The ordered octants with maximum possible level between start and end so that for each octant O: start < O < end
 *
 */
OCTREEBUILDER_API ::std::vector<OctantID> completeRegion(const OctantID& start, const OctantID& end);

/**
 * @brief Retains the highest level octants of each region and fills the remaining space with octants that are as coarse as possible (max level). The octants
 * are called blocks.
 * @param globalRoot The root of the octree that will contains all blocks
 * @param completedRegions A list of completed regions
 * @return The global octree with the blocks as its leafs
 */
OCTREEBUILDER_API LinearOctree computeBlocksFromRegions(const OctantID& globalRoot, ::std::vector<::std::vector<OctantID>> completedRegions);

/**
 * @brief Computes the octant of minimal level that contains a and b.
 * @return The nearest common ancestor of a and b
 */
OCTREEBUILDER_API OctantID nearestCommonAncestor(const OctantID& a, const OctantID& b);

/**
 * @brief Creates the octants that are decendants of the keys at the lowest level and then completes the subtree
 * @param root The root of the subtree
 * @param lowestLevel The level of the octants created from the keys
 * @param keys Octansts of level zero in the tree
 * @return The complete 2:1 balanced subtree that contains all ancestors of all keys at lowestlevel
 */
OCTREEBUILDER_API ::std::vector<OctantID> completeSubtree(const OctantID& root, uint lowestLevel, const ::std::unordered_set<OctantID>& keys);

/**
 * @brief Creates a 2:1 blanced octree from a set of level zero leafs in parallel
 * @param root The root of the octree
 * @param levelZeroLeafs The level zero leafs
 * @param numThreads The number of threads used
 * @param maxLevel The maximum level of all leafs in the final 2:1 balanced octree
 * @return The complete 2:1 blanaced octree
 *
 * The final octree contains all level zero leafs the remaining space is covered with the minimum number of non-overlapping octants
 */
OCTREEBUILDER_API LinearOctree createBalancedOctreeParallel(const OctantID& root, const ::std::vector<OctantID>& levelZeroLeafs, const int numThreads,
                                                            const uint maxLevel = ::std::numeric_limits<uint>::max());
}
