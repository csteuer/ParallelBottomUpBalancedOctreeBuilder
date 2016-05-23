#include "octree_utils.h"

#include "box.h"
#include "octantid.h"
#include "linearoctree.h"

#include <assert.h>
#include <algorithm>
#include <array>
#include <omp.h>

#include "perfcounter.h"
#include "logging.h"
#include <iomanip>

constexpr std::array<Vector3i, 8> searchCornerOffsets{{Vector3i(0, 0, 0), Vector3i(-1, 0, 0), Vector3i(0, -1, 0), Vector3i(-1, -1, 0), Vector3i(0, 0, -1),
                                                       Vector3i(-1, 0, -1), Vector3i(0, -1, -1), Vector3i(-1, -1, -1)}};

std::vector<OctantID> getSearchKeys(const OctantID& octant, const LinearOctree& octree) {
    std::vector<OctantID> searchKeys;
    searchKeys.reserve(7);

    Vector3i searchCorner = getSearchCorner(octant.mcode(), octant.level());

    for (const Vector3i& coord : searchCornerOffsets) {
        Vector3i searchKeyLLF = searchCorner + coord;
        OctantID searchKey(searchKeyLLF, 0);

        if (searchKey != octant && octree.insideTreeBounds(searchKey) && !searchKey.isDecendantOf(octant)) {
            searchKeys.push_back(searchKey);
        }
    }

    return searchKeys;
}

LinearOctree balanceTree(const LinearOctree& octree) {
    LinearOctree result = octree;

    if (octree.depth() < 3) {
        // An octree with depth 2 can only have children of level 0 and 1... hence its always balanced
        return octree;
    }

    const uint numLevelsToCheck = octree.depth() - 2;

    std::vector<std::vector<OctantID>> octantsPerLevel(numLevelsToCheck, std::vector<OctantID>());
    for (const OctantID& octant : octree.leafs()) {
        if (octant.level() >= numLevelsToCheck) {
            continue;
        }
        octantsPerLevel.at(octant.level()).push_back(octant);
    }

    // We check for each node with level l if it has neighbours with a level greater than l+1 and split those neighbours.
    // The maximum level is octree.depth() - 1. Consequently nodes with level octree.depth() - 3 are the last ones that can have a
    // higher level neighbour with a level difference of more than 1.
    for (uint currentLevel = 0; currentLevel < numLevelsToCheck; currentLevel++) {
        std::unordered_map<OctantID, std::unordered_set<OctantID>> unbalanced_nodes;

        for (const OctantID& octant : octantsPerLevel.at(currentLevel)) {
            assert(octant.level() == currentLevel);

            std::vector<OctantID> searchKeys = getSearchKeys(octant, octree);
            for (const OctantID& searchKey : searchKeys) {
                OctantID unbalancedNode;

                if (!result.maximumLowerBound(searchKey, unbalancedNode)) {
                    continue;
                }

                assert(unbalancedNode < searchKey);
                if (unbalancedNode.level() <= currentLevel + 1 || !searchKey.isDecendantOf(unbalancedNode)) {
                    continue;
                }

                auto it = unbalanced_nodes.find(unbalancedNode);

                if (it != unbalanced_nodes.end()) {
                    it->second.insert(searchKey);
                } else {
                    unbalanced_nodes.insert(std::make_pair(unbalancedNode, std::unordered_set<OctantID>{searchKey}));
                }
            }
        }

        if (unbalanced_nodes.empty()) {
            continue;
        }

        for (const std::pair<OctantID, std::unordered_set<OctantID>>& unbalancedNode : unbalanced_nodes) {
            const auto subtree = completeSubtree(unbalancedNode.first, currentLevel + 1, unbalancedNode.second);

            result.replaceWithSubtree(unbalancedNode.first, subtree);

            for (const OctantID& subtreeOctant : subtree) {
                if (subtreeOctant.level() > currentLevel && subtreeOctant.level() < numLevelsToCheck) {
                    octantsPerLevel.at(subtreeOctant.level()).push_back(subtreeOctant);
                }
            }
        }

        result.sortAndRemove();
    }

    return result;
}

LinearOctree createBalancedSubtree(const OctantID& root, const std::vector<OctantID>& levelZeroLeafs, uint maxLevel) {
    LinearOctree tree(root);

    tree.insert(levelZeroLeafs.begin(), levelZeroLeafs.end());

    createBalancedSubtree(tree, maxLevel);

    return tree;
}

void createBalancedSubtree(LinearOctree& tree, uint maxLevel) {
    if (tree.leafs().empty()) {
        tree.insert(tree.root());
        return;
    }

    std::unordered_set<OctantID> nonEmptyNodes;
    nonEmptyNodes.reserve(tree.leafs().size());

    for (const OctantID& leaf : tree.leafs()) {
        assert(leaf.level() == 0);
        nonEmptyNodes.insert(leaf);
    }

    uint currentLevel = 0;
    maxLevel = std::min(maxLevel, tree.depth());

    for (; currentLevel < maxLevel; currentLevel++) {
        std::unordered_set<OctantID> nonEmptyParentNodes;

        // The list of nodes that ensure a level difference of 1 between all nodes that have a common vertex.
        // Either these nodes (which are of the next level) or their child nodes must exist in the tree.
        std::unordered_set<OctantID> guardParentNodes;

        // add the siblings of all non empty nodes
        for (const OctantID& current_node : nonEmptyNodes) {
            OctantID parent = current_node.parent();

            if (!nonEmptyParentNodes.insert(parent).second) {
                continue;
            }

            for (const OctantID& child : parent.children()) {
                // Do not add the current_node it self or any other nonEmptyNode (its represented by its children...)
                if (child != current_node && nonEmptyNodes.count(child) == 0) {
                    tree.insert(child);
                }
            }

            // add the guard nodes (only if there are nodes at the next level at all)
            if (currentLevel < maxLevel - 1) {
                // For each nonEmptyParentNode its sibling nodes will be added in the next iteration.
                // However that is not enough to fullfill the rules of the octree where there must only
                // be a difference of 1 in level between nodes that share a vertex.
                // Hence we must add the remaining nodes manually
                for (const OctantID& guard : parent.potentialNeighbours(tree)) {
                    guardParentNodes.insert(guard);
                }
            }
        }

        // Add the guard nodes
        for (const OctantID& guard : guardParentNodes) {
            // In one line: checks that guard is not already in nonEmptyParentNodes (which would mean that its children are already part of the tree)
            // and if thats not the case add's it to the nonEmptyParentNodes list (this ensures that its siblings are added in the next iteration)
            if (nonEmptyParentNodes.insert(guard).second) {
                tree.insert(guard);
            }
        }

        // in the next level the current non-empty parent nodes are the next non-empty nodes
        nonEmptyNodes = nonEmptyParentNodes;
    }

    if (currentLevel != tree.depth()) {
        assert(currentLevel == maxLevel);

        // max level is capped... hence fill the empty parts of the octree with nodes of the current level
        const coord_t nodeSize = getOctantSizeForLevel(currentLevel);
        Vector3i treeMaxXYZ = getMaxXYZForOctreeDepth(tree.depth());

        for (coord_t x = 0; x < treeMaxXYZ.x(); x += nodeSize) {
            for (coord_t y = 0; y < treeMaxXYZ.y(); y += nodeSize) {
                for (coord_t z = 0; z < treeMaxXYZ.z(); z += nodeSize) {
                    OctantID node(Vector3i(x, y, z), currentLevel);

                    if (nonEmptyNodes.count(node) == 0) {
                        tree.insert(node);
                    }
                }
            }
        }
    }

    tree.sortAndRemove();
}

Partition::Partition(const OctantID& rootOctant, const std::vector<LinearOctree>& partitionList) : root(rootOctant), partitions(partitionList) {
}

Partition computePartition(const OctantID& globalRoot, const std::vector<OctantID>& levelZeroLeafs, const int numThreads) {
    if (levelZeroLeafs.empty()) {
        throw std::runtime_error("computePartition: Invalid parameter. No level zero leaves.");
    }

    assert(levelZeroLeafs.front() <= levelZeroLeafs.back());  // levelZeroLeafs must be sorted

    const size_t leafsPerProcessor = levelZeroLeafs.size() / numThreads;

    std::vector<std::vector<OctantID>> completedRegions;

    if (leafsPerProcessor > 2) {
        completedRegions.reserve(numThreads);

        for (int t = 0; t < numThreads; t++) {
            size_t start = t * leafsPerProcessor;
            size_t end = (t < numThreads - 1 ? (t + 1) * leafsPerProcessor : levelZeroLeafs.size()) - 1;

            std::vector<OctantID> region = completeRegion(levelZeroLeafs.at(start), levelZeroLeafs.at(end));

            if (!region.empty()) {
                completedRegions.push_back(region);
            }
        }
    }

    LinearOctree blocks = computeBlocksFromRegions(globalRoot, completedRegions);

    if (blocks.leafs().empty()) {
        std::vector<LinearOctree> partitions = {LinearOctree(globalRoot, levelZeroLeafs)};
        return Partition(globalRoot, partitions);
    }

    std::vector<LinearOctree> partitions;
    partitions.reserve(blocks.leafs().size());

    for (const OctantID& block : blocks.leafs()) {
        partitions.push_back(LinearOctree(block));
    }

    auto partiotionsIterator = partitions.begin();

    for (const OctantID& levelZeroLeaf : levelZeroLeafs) {
        while (partiotionsIterator != partitions.end() && !partiotionsIterator->insideTreeBounds(levelZeroLeaf)) {
            ++partiotionsIterator;
        }

        if (partiotionsIterator == partitions.end()) {
            // Either a bug or levelZeroLeafs are not sorted
            throw std::runtime_error("computePartition: Invalid state. No block for level zero leaf found.");
        }

        partiotionsIterator->insert(levelZeroLeaf);
    }

    assert(blocks.root() == globalRoot);
    return Partition(blocks.root(), partitions);
}

std::vector<OctantID> completeRegion(const OctantID& start, const OctantID& end) {
    if (start > end) {
        throw std::runtime_error("completeRegion: Invalid parameters. Start must be less or equal than end. ");
    } else if (start == end) {
        return {};
    }

    OctantID root = nearestCommonAncestor(start, end);
    LinearOctree result(root);

    std::vector<OctantID> possibleLeafs = root.children();

    while (!possibleLeafs.empty()) {
        std::vector<OctantID> new_possibleLeafs;
        new_possibleLeafs.reserve(8 * possibleLeafs.size());

        for (const OctantID& leaf : possibleLeafs) {
            if (start < leaf && leaf < end && !end.isDecendantOf(leaf)) {
                result.insert(leaf);
            } else if (end.isDecendantOf(leaf) || start.isDecendantOf(leaf)) {
                auto children = leaf.children();
                new_possibleLeafs.insert(new_possibleLeafs.end(), children.begin(), children.end());
            }
        }

        possibleLeafs = new_possibleLeafs;
    }

    result.sortAndRemove();
    return std::vector<OctantID>(result.leafs().begin(), result.leafs().end());
}

LinearOctree computeBlocksFromRegions(const OctantID& globalRoot, std::vector<std::vector<OctantID>> completedRegions) {
    LinearOctree result(globalRoot);

    if (completedRegions.empty()) {
        return result;
    }

    assert(completedRegions.front().front() <= completedRegions.back().back());

    // Only retain the highest level octants of each region
    for (size_t i = 0; i < completedRegions.size(); i++) {
        const std::vector<OctantID> currentRegion = completedRegions.at(i);

        // Find the maximum level of all octants in the region (at least 1)
        uint maxLevel = 1;
        for (const OctantID& octant : currentRegion) {
            maxLevel = std::max(octant.level(), maxLevel);
        }

        // Remove all octants that are not of the maximum level
        std::vector<OctantID> filteredRegion;
        filteredRegion.reserve(currentRegion.size());

        for (const OctantID& octant : currentRegion) {
            if (octant.level() == maxLevel) {
                filteredRegion.push_back(octant);
            }
        }

        completedRegions.at(i) = filteredRegion;
    }

    completedRegions.erase(std::remove_if(completedRegions.begin(), completedRegions.end(), [](const std::vector<OctantID>& completedRegion) {
        return completedRegion.empty();
    }), completedRegions.end());

    for (size_t i = 0; i < completedRegions.size(); i++) {
        std::vector<OctantID>& blocks = completedRegions.at(i);

        if (i == 0) {
            OctantID first = nearestCommonAncestor(result.deepestFirstDecendant(), blocks.front());
            if (first != blocks.front()) {
                blocks.insert(blocks.begin(), first.children().front());
            }
        }

        if (i < completedRegions.size() - 1) {
            blocks.push_back(completedRegions.at(i + 1).front());
        } else {
            OctantID last = nearestCommonAncestor(result.deepestLastDecendant(), blocks.back());
            if (last != blocks.back()) {
                blocks.push_back(last.children().back());
            }
        }

        for (size_t y = 0; y < blocks.size() - 1; y++) {
            std::vector<OctantID> completedBlockRange = completeRegion(blocks.at(y), blocks.at(y + 1));
            result.insert(blocks.at(y));
            result.insert(completedBlockRange.begin(), completedBlockRange.end());
        }

        if (i == completedRegions.size() - 1) {
            result.insert(blocks.back());
        }
    }

    return result;
}

OctantID nearestCommonAncestor(const OctantID& a, const OctantID& b) {
    const auto resultPair = nearestCommonAncestor(a.mcode(), b.mcode(), a.level(), b.level());
    return OctantID(resultPair.first, resultPair.second);
}

std::vector<OctantID> completeSubtree(const OctantID& root, uint lowestLevel, const std::unordered_set<OctantID>& keys) {
    if (root.level() == lowestLevel || keys.empty()) {
        throw std::runtime_error("completeSubtree: Invalid parameter(s). Empty subtree if lowest level is equal to root level or no keys.");
    }

    if (root.level() == lowestLevel + 1) {
        return root.children();
    }

    std::vector<OctantID> result;
    std::unordered_set<OctantID> currentLevelLeafs;

    for (const OctantID& key : keys) {
        assert(key.level() == 0);

        OctantID leaf = key.ancestorAtLevel(lowestLevel);
        if (currentLevelLeafs.insert(leaf).second) {
            result.push_back(leaf);
        }
    }

    for (uint l = lowestLevel; l < root.level(); l++) {
        std::unordered_set<OctantID> currentLevelParents;

        for (const OctantID& leaf : currentLevelLeafs) {
            OctantID parent = leaf.parent();

            if (!currentLevelParents.insert(parent).second) {
                continue;
            }

            for (const OctantID& child : parent.children()) {
                if (currentLevelLeafs.count(child)) {
                    continue;
                }
                result.push_back(child);
            }
        }

        currentLevelLeafs = currentLevelParents;
    }

    return result;
}

static void collectBoundaryLeafs(const LinearOctree& partition, const LinearOctree& globalTree, std::vector<OctantID>& outBoundaryOctants) {
    for (const OctantID& leaf : partition.leafs()) {
        if (leaf.isBoundaryOctant(partition, globalTree)) {
            outBoundaryOctants.push_back(leaf);
        }
    }
}

static std::vector<OctantID> flattenPartitions(const std::vector<LinearOctree>& partitions) {
    size_t numLeafs = 0;
    for (const LinearOctree& partition : partitions) {
        numLeafs += partition.leafs().size();
    }

    std::vector<OctantID> allLeafs;
    allLeafs.reserve(numLeafs);
    for (const LinearOctree& partition : partitions) {
        allLeafs.insert(allLeafs.end(), partition.leafs().begin(), partition.leafs().end());
    }
    return allLeafs;
}

static LinearOctree mergePartitionsAndBalancedBoundaryTree(const std::vector<OctantID>& flatUnbalancedTree, const LinearOctree& balancedBoundaryTree) {
    if (balancedBoundaryTree.leafs().empty()) {
        return LinearOctree(balancedBoundaryTree.root(), flatUnbalancedTree);
    }

    const size_t numLeafsInMergedTree = flatUnbalancedTree.size() + balancedBoundaryTree.leafs().size();
    LinearOctree mergedTree(balancedBoundaryTree.root(), numLeafsInMergedTree);

    auto balancingOctantsIterator = balancedBoundaryTree.leafs().begin();

    auto unbalancedOctantsInsertRangeBegin = flatUnbalancedTree.begin();
    auto unbalancedOctantsIterator = flatUnbalancedTree.begin();

    for (; unbalancedOctantsIterator != flatUnbalancedTree.end(); ++unbalancedOctantsIterator) {
        const OctantID& current = *unbalancedOctantsIterator;

        if (current.mcode() == balancingOctantsIterator->mcode()) {
            assert(*balancingOctantsIterator == current || balancingOctantsIterator->isDecendantOf(current));

            const OctantID& next = *(unbalancedOctantsIterator + 1);

            auto balancingOctantsRangeStart = balancingOctantsIterator;

            while (balancingOctantsIterator != balancedBoundaryTree.leafs().end() && *balancingOctantsIterator < next) {
                ++balancingOctantsIterator;
            }

            mergedTree.insert(unbalancedOctantsInsertRangeBegin, unbalancedOctantsIterator);
            unbalancedOctantsInsertRangeBegin = unbalancedOctantsIterator + 1;

            mergedTree.insert(balancingOctantsRangeStart, balancingOctantsIterator);
        }
    }

    mergedTree.insert(unbalancedOctantsInsertRangeBegin, unbalancedOctantsIterator);

    assert(balancingOctantsIterator == balancedBoundaryTree.leafs().end() || *balancingOctantsIterator == flatUnbalancedTree.back() ||
           balancingOctantsIterator->isDecendantOf(flatUnbalancedTree.back()));
    mergedTree.insert(balancingOctantsIterator, balancedBoundaryTree.leafs().end());

    return mergedTree;
}

LinearOctree mergeUnbalancedCompleteTreeWithBalancedIncompleteTree(const LinearOctree& unbalancedTree, const LinearOctree& balancedTree) {
    return mergePartitionsAndBalancedBoundaryTree(unbalancedTree.leafs(), balancedTree);
}

static void parallelCreateBalancedSubtrees(std::vector<LinearOctree>& partitions, const uint maxLevel) {
#pragma omp parallel for schedule(dynamic, 1)
    for (size_t i = 0; i < partitions.size(); i++) {
        createBalancedSubtree(partitions.at(i), maxLevel);
    }
}

static std::vector<std::vector<OctantID>> parallelCollectBoundaryLeafs(const Partition& partition) {
    const LinearOctree globalTree(partition.root);

    std::vector<std::vector<OctantID>> boundaryOctantsPerPartition(partition.partitions.size());
#pragma omp parallel for schedule(dynamic, 1)
    for (size_t i = 0; i < partition.partitions.size(); i++) {
        const LinearOctree& currentPartition = partition.partitions.at(i);
        std::vector<OctantID>& boundaryOctants = boundaryOctantsPerPartition.at(i);

        collectBoundaryLeafs(currentPartition, globalTree, boundaryOctants);
    }

    return boundaryOctantsPerPartition;
}

static LinearOctree createBoundaryOctantsTree(const std::vector<std::vector<OctantID>>& boundaryOctantsPerPartition, const OctantID& globalTreeRoot) {

    size_t numBoundaryOctants = 0;
    for (const std::vector<OctantID>& boundaryOctants : boundaryOctantsPerPartition) {
        numBoundaryOctants += boundaryOctants.size();
    }

    LinearOctree boundaryOctantsTree(globalTreeRoot, numBoundaryOctants);

    for (const std::vector<OctantID>& boundaryOctants : boundaryOctantsPerPartition) {
        boundaryOctantsTree.insert(boundaryOctants.begin(), boundaryOctants.end());
    }

    return boundaryOctantsTree;
}

LinearOctree createBalancedOctreeParallel(const OctantID& root, const std::vector<OctantID>& levelZeroLeafs, const int numThreads, const uint maxLevel) {
    PerfCounter perfCounter;

    perfCounter.start();
    Partition computedPartition = computePartition(root, levelZeroLeafs, numThreads);

    LOG_PROF(std::left << std::setw(30) << "Created partition: " << perfCounter);

    perfCounter.start();
    parallelCreateBalancedSubtrees(computedPartition.partitions, maxLevel);
    LOG_PROF(std::left << std::setw(30) << "Created balanced subtrees: " << perfCounter);

    perfCounter.start();
    std::vector<std::vector<OctantID>> boundaryOctantsPerPartition = parallelCollectBoundaryLeafs(computedPartition);
    LOG_PROF(std::left << std::setw(30) << "Collected boundary leafs: " << perfCounter);

    perfCounter.start();
    LinearOctree boundaryOctantsTree = createBoundaryOctantsTree(boundaryOctantsPerPartition, computedPartition.root);
    LOG_PROF(std::left << std::setw(30) << "Created boundary tree: " << perfCounter);

    perfCounter.start();
    LinearOctree balancedBoundaryTree = balanceTree(boundaryOctantsTree);
    LOG_PROF(std::left << std::setw(30) << "Balanced boundary tree: " << perfCounter);

    perfCounter.start();
    std::vector<OctantID> leafsOfAllPartitions = flattenPartitions(computedPartition.partitions);
    LOG_PROF(std::left << std::setw(30) << "Flatten boundary tree: " << perfCounter);

    perfCounter.start();
    LinearOctree result = mergePartitionsAndBalancedBoundaryTree(leafsOfAllPartitions, balancedBoundaryTree);
    LOG_PROF(std::left << std::setw(30) << "Merged boundary tree: " << perfCounter);

    return result;
}
