#include <gmock/gmock.h>

#include <octantid.h>
#include <linearoctree.h>
#include <octree_utils.h>

#include <vector_utils.h>

TEST(OctreeUtilsTest, getSearchKeysTest) {
    const LinearOctree octree(OctantID(0, 4));

    // All search keys of the lower left front node are outside of the domain
    auto searchKeys = getSearchKeys(OctantID(Vector3i(0), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::IsEmpty());

    // ... no matter which level
    searchKeys = getSearchKeys(OctantID(Vector3i(0), 4), LinearOctree(OctantID(0, 5)));
    ASSERT_THAT(searchKeys, ::testing::IsEmpty());

    searchKeys = getSearchKeys(OctantID(Vector3i(1, 0, 0), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::ElementsAre(OctantID(Vector3i(2, 0, 0), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(0, 1, 0), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::ElementsAre(OctantID(Vector3i(0, 2, 0), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(0, 0, 1), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::ElementsAre(OctantID(Vector3i(0, 0, 2), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(1, 0, 1), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(2, 0, 1), 0), OctantID(Vector3i(2, 0, 2), 0), OctantID(Vector3i(1, 0, 2), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(1, 1, 0), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(2, 1, 0), 0), OctantID(Vector3i(2, 2, 0), 0), OctantID(Vector3i(1, 2, 0), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(0, 1, 1), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(0, 2, 1), 0), OctantID(Vector3i(0, 2, 2), 0), OctantID(Vector3i(0, 1, 2), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(1, 1, 1), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(2, 1, 1), 0), OctantID(Vector3i(1, 2, 1), 0), OctantID(Vector3i(1, 1, 2), 0),
                                                            OctantID(Vector3i(2, 2, 1), 0), OctantID(Vector3i(2, 1, 2), 0), OctantID(Vector3i(1, 2, 2), 0),
                                                            OctantID(Vector3i(2, 2, 2), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(4, 4, 2), 1), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(3, 4, 4), 0), OctantID(Vector3i(4, 3, 4), 0), OctantID(Vector3i(4, 4, 4), 0),
                                                            OctantID(Vector3i(3, 3, 4), 0), OctantID(Vector3i(3, 4, 3), 0), OctantID(Vector3i(4, 3, 3), 0),
                                                            OctantID(Vector3i(3, 3, 3), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(4, 4, 4), 1), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(3, 4, 4), 0), OctantID(Vector3i(4, 3, 4), 0), OctantID(Vector3i(4, 4, 3), 0),
                                                            OctantID(Vector3i(3, 3, 4), 0), OctantID(Vector3i(3, 4, 3), 0), OctantID(Vector3i(4, 3, 3), 0),
                                                            OctantID(Vector3i(3, 3, 3), 0)));

    searchKeys = getSearchKeys(OctantID(Vector3i(4, 6, 4), 1), octree);
    ASSERT_THAT(searchKeys, ::testing::UnorderedElementsAre(OctantID(Vector3i(3, 8, 4), 0), OctantID(Vector3i(4, 8, 4), 0), OctantID(Vector3i(4, 8, 3), 0),
                                                            OctantID(Vector3i(3, 7, 4), 0), OctantID(Vector3i(3, 8, 3), 0), OctantID(Vector3i(4, 7, 3), 0),
                                                            OctantID(Vector3i(3, 7, 3), 0)));

    // All search keys of the upper right back node are outside of the domain
    searchKeys = getSearchKeys(OctantID(Vector3i(8, 8, 8), 3), octree);
    ASSERT_THAT(searchKeys, ::testing::IsEmpty());

    searchKeys = getSearchKeys(OctantID(Vector3i(12, 12, 12), 2), octree);
    ASSERT_THAT(searchKeys, ::testing::IsEmpty());

    searchKeys = getSearchKeys(OctantID(Vector3i(14, 14, 14), 1), octree);
    ASSERT_THAT(searchKeys, ::testing::IsEmpty());

    searchKeys = getSearchKeys(OctantID(Vector3i(15, 15, 15), 0), octree);
    ASSERT_THAT(searchKeys, ::testing::IsEmpty());
}

TEST(OctreeUtilsTest, propagateRippleInUnbalancedTreeTest) {
    LinearOctree unbalancedOctree(OctantID(0, 4));

    /* Create unbalanced octree (thats not even four balanced)
     *  __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |__ __ __ __ __ __ __ __|__ __ __ __ __ __ __ __|
     * |           |     |__|__|                       |
     * |           |__ __|__|__|                       |
     * |           |     |     |                       |
     * |__ __ __ __|__ __|__ __|                       |
     * |           |           |                       |
     * |           |           |                       |
     * |           |           |                       |
     * |__ __ __ __|__ __ __ __|__ __ __ __ __ __ __ __|
     *
     */

    for (Vector3i c : VectorSpace(Vector3i(2))) {
        unbalancedOctree.insert(OctantID(c + Vector3i(6, 6, 6), 0));

        if (c != Vector3i(1, 1, 1)) {
            unbalancedOctree.insert(OctantID(c * 2 + Vector3i(4, 4, 4), 1));
            unbalancedOctree.insert(OctantID(c * 4, 2));
        }

        if (c != Vector3i(0, 0, 0)) {
            unbalancedOctree.insert(OctantID(c * 8, 3));
        }
    }

    unbalancedOctree.sortAndRemove();

    const LinearOctree balancedTree = balanceTree(unbalancedOctree);

    ASSERT_EQ(127, balancedTree.leafs().size());

    for (Vector3i c : VectorSpace(Vector3i(2))) {
        ASSERT_THAT(balancedTree.leafs(), ::testing::Contains(OctantID(c + Vector3i(6, 6, 6), 0)));
    }

    for (Vector3i c : VectorSpace(Vector3i(4))) {
        if (c != Vector3i(1, 1, 1)) {
            ASSERT_THAT(balancedTree.leafs(), ::testing::Contains(OctantID(c * 2 + Vector3i(4, 4, 4), 1)));
        }

        if (c.x() < 1 || c.x() > 2 || c.y() < 1 || c.y() > 2 || c.z() < 1 || c.z() > 2) {
            ASSERT_THAT(balancedTree.leafs(), ::testing::Contains(OctantID(c * 4, 2)));
        }
    }
}

TEST(OctreeUtilsTest, propagateRippleInIncompleteUnbalancedTreeTest) {
    LinearOctree unbalancedOctree(OctantID(0, 4));

    /* Create unbalanced octree (thats not even four balanced)
     *  __ __ __ __ __ __ __ __ __ __ __ __ __ __ __ __
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |                       |                       |
     * |__ __ __ __ __ __ __ __|__ __ __ __ __ __ __ __|
     *                   |__|__|                       |
     *                   |__|__|                       |
     *                         |                       |
     *                         |                       |
     *                         |                       |
     *                         |                       |
     *                         |                       |
     *                         |__ __ __ __ __ __ __ __|
     *
     */

    for (Vector3i c : VectorSpace(Vector3i(2))) {
        unbalancedOctree.insert(OctantID(c + Vector3i(6, 6, 6), 0));

        if (c != Vector3i(0, 0, 0)) {
            unbalancedOctree.insert(OctantID(c * 8, 3));
        }
    }

    unbalancedOctree.sortAndRemove();

    const LinearOctree balancedTree = balanceTree(unbalancedOctree);

    ASSERT_EQ(113, balancedTree.leafs().size());

    for (Vector3i c : VectorSpace(Vector3i(4))) {
        if (c.x() > 1 || c.y() > 1 || c.z() > 1) {
            ASSERT_THAT(balancedTree.leafs(), ::testing::Contains(OctantID(c * 2 + Vector3i(4, 4, 4), 1)));
        }

        if (c.x() > 2 || c.y() > 2 || c.z() > 2) {
            ASSERT_THAT(balancedTree.leafs(), ::testing::Contains(OctantID(c * 4, 2)));
        }
    }
}

TEST(OctreeUtilsTest, completeAndBalanceSubtreeNoLeafsTest) {
    ASSERT_THAT(createBalancedSubtree(OctantID(Vector3i(0), 0), {}).leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 0)));
    ASSERT_THAT(createBalancedSubtree(OctantID(Vector3i(0), 4), {}).leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 4)));
    ASSERT_THAT(createBalancedSubtree(OctantID(Vector3i(2), 1), {}).leafs(), ::testing::ElementsAre(OctantID(Vector3i(2), 1)));
}

TEST(OctreeUtilsTest, completeAndBalanceSubtreeAtOriginTest) {
    LinearOctree octree = createBalancedSubtree(OctantID(Vector3i(0), 4), {OctantID(Vector3i(0), 0)});

    ASSERT_THAT(octree.leafs(), ::testing::SizeIs(29));

    for (Vector3i c : VectorSpace(Vector3i(2))) {
        ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(c, 0))));

        if (c != Vector3i(0)) {
            ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(c * 2, 1))));
            ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(c * 4, 2))));
            ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(c * 8, 3))));
        }
    }
}

TEST(OctreeUtilsTest, completeAndBalanceSubtreeNotAtOrigin) {
    LinearOctree octree = createBalancedSubtree(OctantID(Vector3i(16, 0, 0), 4), {OctantID(Vector3i(16, 0, 0), 0)});

    ASSERT_THAT(octree.leafs(), ::testing::SizeIs(29));

    for (Vector3i c : VectorSpace(Vector3i(2))) {
        ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(Vector3i(16, 0, 0) + c, 0))));

        if (c != Vector3i(0)) {
            ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(Vector3i(16, 0, 0) + c * 2, 1))));
            ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(Vector3i(16, 0, 0) + c * 4, 2))));
            ASSERT_THAT(octree.leafs(), ::testing::Contains(::testing::Eq(OctantID(Vector3i(16, 0, 0) + c * 8, 3))));
        }
    }
}

TEST(OctreeUtilsTest, nearestCommonAncestorTest) {
    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 0), OctantID(0, 0)), ::testing::Eq(OctantID(0, 0)));
    ASSERT_THAT(nearestCommonAncestor(OctantID(1, 0), OctantID(0, 0)), ::testing::Eq(OctantID(0, 1)));

    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 0), OctantID({8, 8, 8}, 3)), ::testing::Eq(OctantID(0, 4)));
    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 1), OctantID({8, 8, 8}, 3)), ::testing::Eq(OctantID(0, 4)));
    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 2), OctantID({8, 8, 8}, 3)), ::testing::Eq(OctantID(0, 4)));
    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 3), OctantID({8, 8, 8}, 3)), ::testing::Eq(OctantID(0, 4)));
    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 4), OctantID({8, 8, 8}, 3)), ::testing::Eq(OctantID(0, 4)));
    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 5), OctantID({8, 8, 8}, 3)), ::testing::Eq(OctantID(0, 5)));

    ASSERT_THAT(nearestCommonAncestor(OctantID({14, 12, 10}, 1), OctantID({15, 9, 14}, 0)), ::testing::Eq(OctantID({8, 8, 8}, 3)));

    ASSERT_THAT(nearestCommonAncestor(OctantID(0, 0), OctantID(0, 3)), ::testing::Eq(OctantID(0, 3)));
    ASSERT_THAT(nearestCommonAncestor(OctantID({7, 7, 7}, 0), OctantID({4, 4, 4}, 2)), ::testing::Eq(OctantID({4, 4, 4}, 2)));
}

MATCHER_P2(IsValidPartition, levelZeroLeafs, globalTree, std::string(negation ? "isn't" : "is") + " valid partition of tree with root " +
                                                             ::testing::PrintToString(globalTree.root()) + ", and level zero leafs: " +
                                                             ::testing::PrintToString(levelZeroLeafs)) {
    // Just a functional test if all leafs are still presenent in the partition and
    // if the partition covers the whole tree

    const Partition& partition = arg;

    if (partition.root != globalTree.root()) {
        *result_listener << "partition root " << partition.root << " isn't equal to tree root " << globalTree.root();
        return false;
    }

    // Check that the partiton covers the whole tree

    if (partition.partitions.empty()) {
        *result_listener << "partition is empty.";
        return false;
    }

    if (partition.partitions.front().deepestFirstDecendant() != globalTree.deepestFirstDecendant()) {
        *result_listener << "partition does not cover the whole tree. Wrong start partition.";
        return false;
    }

    if (partition.partitions.back().deepestLastDecendant() != globalTree.deepestLastDecendant()) {
        *result_listener << "partition does not cover the whole tree. Wrong end partition.";
        return false;
    }

    for (size_t i = 0; i < partition.partitions.size() - 1; i++) {
        if (partition.partitions.at(i).deepestLastDecendant().mcode() + 1 != partition.partitions.at(i + 1).deepestFirstDecendant().mcode()) {
            *result_listener << "partition does not cover the whole tree. Partition #" << i << " deepest last decendant is "
                             << partition.partitions.at(i).deepestLastDecendant().mcode() << " and partition #" << i + 1 << " deepest first descendant is "
                             << partition.partitions.at(i).deepestFirstDecendant().mcode();
            return false;
        }
    }

    std::vector<OctantID> partitionLeafs;
    for (const LinearOctree& partition : partition.partitions) {
        for (const OctantID& leaf : partition.leafs()) {
            partitionLeafs.push_back(leaf);
        }
    }

    if (!::testing::Matches(::testing::ContainerEq(levelZeroLeafs))(partitionLeafs)) {
        *result_listener << " leafs in partition are not the same as the original leafs:";
        ::testing::ExplainMatchResult(::testing::ContainerEq(levelZeroLeafs), partitionLeafs, result_listener);
        return false;
    }

    return true;
}

TEST(OctreeUtilsTest, computePartitionTest) {
    // Just a functional test if all leafs are still presenent in the partition and
    // if the partition covers the whole tree

    const LinearOctree globalTree(OctantID(0, 3));

    std::vector<OctantID> levelZeroLeafs;
    for (morton_t mcode = 32; mcode <= globalTree.deepestLastDecendant().mcode() - 32; mcode += 8) {
        levelZeroLeafs.push_back(OctantID(mcode, 0));
    }

    Partition partition = computePartition(globalTree.root(), levelZeroLeafs, 2);
    ASSERT_THAT(partition, IsValidPartition(levelZeroLeafs, globalTree));

    partition = computePartition(globalTree.root(), levelZeroLeafs, 4);
    ASSERT_THAT(partition, IsValidPartition(levelZeroLeafs, globalTree));

    partition = computePartition(globalTree.root(), levelZeroLeafs, 8);
    ASSERT_THAT(partition, IsValidPartition(levelZeroLeafs, globalTree));
}

TEST(OctreeUtilsTest, computePartitionWithLessLeafsThenThreadsTest) {
    const LinearOctree globalTree(OctantID(0, 3));
    const std::vector<OctantID> levelZeroLeafs{OctantID(0, 0), OctantID(511, 0)};

    Partition partition = computePartition(globalTree.root(), levelZeroLeafs, 4);
    ASSERT_THAT(partition, IsValidPartition(levelZeroLeafs, globalTree));
}

TEST(OctreeUtilsTest, completeRegionTest) {
    const auto result = completeRegion(OctantID(36, 0), OctantID(294, 0));

    ASSERT_THAT(result, ::testing::ElementsAreArray({OctantID(37, 0), OctantID(38, 0), OctantID(39, 0), OctantID(40, 1), OctantID(48, 1), OctantID(56, 1),
                                                     OctantID(64, 2), OctantID(128, 2), OctantID(192, 2), OctantID(256, 1), OctantID(264, 1), OctantID(272, 1),
                                                     OctantID(280, 1), OctantID(288, 0), OctantID(289, 0), OctantID(290, 0), OctantID(291, 0), OctantID(292, 0),
                                                     OctantID(293, 0)}));
}

TEST(OctreeUtilsTest, computeBlocksFromRegionsTest) {
    const std::vector<OctantID> region0 = {OctantID(37, 0),  OctantID(38, 0),  OctantID(39, 0),  OctantID(40, 1),  OctantID(48, 1),
                                           OctantID(56, 1),  OctantID(64, 2),  OctantID(128, 2), OctantID(192, 2), OctantID(256, 1),
                                           OctantID(264, 1), OctantID(272, 1), OctantID(280, 1), OctantID(288, 0), OctantID(289, 0),
                                           OctantID(290, 0), OctantID(291, 0), OctantID(292, 0), OctantID(293, 0)};

    const std::vector<OctantID> region1 = {OctantID(295, 0), OctantID(296, 1), OctantID(304, 1), OctantID(312, 1), OctantID(320, 2), OctantID(385, 0)};

    const LinearOctree blocks = computeBlocksFromRegions(OctantID(0, 3), {region0, region1});

    ASSERT_THAT(blocks.root(), ::testing::Eq(OctantID(0, 3)));
    EXPECT_THAT(blocks.leafs(), ::testing::ElementsAre(OctantID(0, 2), OctantID(64, 2), OctantID(128, 2), OctantID(192, 2), OctantID(256, 2), OctantID(320, 2),
                                                       OctantID(384, 2), OctantID(448, 2)));
}

TEST(OctreeUtilsTest, completeSubtreeTest) {
    auto result = completeSubtree(OctantID(0, 2), 0, {OctantID(2, 0), OctantID(3, 0)});
    ASSERT_THAT(result, ::testing::UnorderedElementsAreArray({OctantID(0, 0), OctantID(1, 0), OctantID(2, 0), OctantID(3, 0), OctantID(4, 0), OctantID(5, 0),
                                                              OctantID(6, 0), OctantID(7, 0), OctantID(8, 1), OctantID(16, 1), OctantID(24, 1), OctantID(32, 1),
                                                              OctantID(40, 1), OctantID(48, 1), OctantID(56, 1)}));

    result = completeSubtree(OctantID(0, 2), 1, {OctantID(2, 0), OctantID(3, 0)});
    ASSERT_THAT(result, ::testing::UnorderedElementsAreArray({OctantID(0, 1), OctantID(8, 1), OctantID(16, 1), OctantID(24, 1), OctantID(32, 1),
                                                              OctantID(40, 1), OctantID(48, 1), OctantID(56, 1)}));

    result = completeSubtree(OctantID(0, 3), 1, {OctantID(2, 0), OctantID(3, 0)});
    ASSERT_THAT(result, ::testing::UnorderedElementsAreArray({OctantID(0, 1), OctantID(8, 1), OctantID(16, 1), OctantID(24, 1), OctantID(32, 1),
                                                              OctantID(40, 1), OctantID(48, 1), OctantID(56, 1), OctantID(64, 2), OctantID(128, 2),
                                                              OctantID(192, 2), OctantID(256, 2), OctantID(320, 2), OctantID(384, 2), OctantID(448, 2)}));
}

TEST(OctreeUtilsTest, mergeUnbalancedCompleteTreeWithBalancedIncompleteTreeTest) {
    LinearOctree completeUnbalancedTree(
        OctantID(0, 3), {OctantID(0, 1), OctantID(8, 1), OctantID(16, 1), OctantID(24, 1), OctantID(32, 1), OctantID(40, 1), OctantID(48, 1), OctantID(56, 0), OctantID(57, 0),
                         OctantID(58, 0), OctantID(59, 0), OctantID(60, 0), OctantID(61, 0), OctantID(62, 0), OctantID(63, 0), OctantID(64, 2),
                         OctantID(128, 2), OctantID(192, 2), OctantID(192, 2), OctantID(256, 2), OctantID(320, 2), OctantID(384, 2), OctantID(448, 2)});

    LinearOctree incompleteBalancedTree(OctantID(0, 3));
    for (morton_t mcode = 64; mcode < 512; mcode += 8) {
        incompleteBalancedTree.insert(OctantID(mcode, 1));
    }

    const auto result = mergeUnbalancedCompleteTreeWithBalancedIncompleteTree(completeUnbalancedTree, incompleteBalancedTree);

    for (morton_t mcode = 0; mcode < 512; mcode += 8) {
        if (mcode == 56) {
            for (morton_t lzeroMcode = 56; lzeroMcode < 64; lzeroMcode++) {
                ASSERT_THAT(result.leafs(), ::testing::Contains(OctantID(lzeroMcode, 0)));
            }
        } else {
            ASSERT_THAT(result.leafs(), ::testing::Contains(OctantID(mcode, 1)));
        }
    }

    ASSERT_THAT(result.leafs(), ::testing::SizeIs(71));
}
