#include <gmock/gmock.h>

#include <octantid.h>
#include <linearoctree.h>

#include <vector_utils.h>

using namespace octreebuilder;

TEST(LinearOctreeTest, depthTest) {
    LinearOctree octree(OctantID(Vector3i(0), 4));
    EXPECT_THAT(octree, ::testing::Property(&LinearOctree::depth, ::testing::Eq(4)));
}

TEST(LinearOctreeTest, insertTest) {
    LinearOctree octree(OctantID(Vector3i(0), 2));
    ASSERT_THAT(octree.leafs(), ::testing::IsEmpty());

    octree.insert(OctantID(Vector3i(0), 0));
    ASSERT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 0)));

    octree.insert(OctantID(Vector3i(0), 1));
    octree.sortAndRemove();
    ASSERT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 1), OctantID(Vector3i(0), 0)));

    octree.insert(OctantID(Vector3i(0, 0, 2), 0));
    octree.sortAndRemove();
    ASSERT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 1), OctantID(Vector3i(0), 0),
                                                       OctantID(Vector3i(0, 0, 2), 0)));

    octree.insert(OctantID(Vector3i(0), 2));
    octree.sortAndRemove();
    ASSERT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 2), OctantID(Vector3i(0), 1),
                                                       OctantID(Vector3i(0), 0), OctantID(Vector3i(0, 0, 2), 0)));
}

TEST(LinearOctreeTest, insertRangeTest) {
    LinearOctree octree(OctantID(Vector3i(0), 2));
    ASSERT_THAT(octree.leafs(), ::testing::IsEmpty());

    LinearOctree::container_type input { OctantID(Vector3i(0), 0), OctantID(Vector3i(0), 1), OctantID(Vector3i(0, 0, 2), 0), OctantID(Vector3i(0), 2) };
    octree.insert(input.begin(), input.end());
    octree.sortAndRemove();
    ASSERT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(Vector3i(0), 2), OctantID(Vector3i(0), 1),
                                                       OctantID(Vector3i(0), 0), OctantID(Vector3i(0, 0, 2), 0)));
}

TEST(LinearOctreeTest, insideTreeBoundsTest) {
    LinearOctree octree(OctantID(Vector3i(0, 0, 0), 2));

    for (Vector3i c : VectorSpace(Vector3i(8))) {
        if (c.x() > 3 || c.y() > 3 || c.z() > 3) {
            ASSERT_FALSE(octree.insideTreeBounds(OctantID(c, 0)));
        } else {
            ASSERT_TRUE(octree.insideTreeBounds(OctantID(c, 0)));
        }
    }

    for (Vector3i c : VectorSpace(Vector3i(4))) {
        if (c.x() > 1 || c.y() > 1 || c.z() > 1) {
            ASSERT_FALSE(octree.insideTreeBounds(OctantID(c * 2, 1)));
        } else {
            ASSERT_TRUE(octree.insideTreeBounds(OctantID(c * 2, 1)));
        }
    }

    ASSERT_TRUE(octree.insideTreeBounds(OctantID(Vector3i(0, 0, 0), 2)));
    ASSERT_FALSE(octree.insideTreeBounds(OctantID(Vector3i(0, 0, 0), 3)));
    ASSERT_FALSE(octree.insideTreeBounds(OctantID(Vector3i(4, 0, 0), 2)));
}

TEST(LinearOctreeTest, containsWithOffsetTest) {
    const Vector3i offset(8, 8, 8);

    LinearOctree octree(OctantID(offset, 1));

    for (Vector3i c : VectorSpace(Vector3i(-2), Vector3i(4))) {
        if (c.x() > 1 || c.y() > 1 || c.z() > 1 || c.x() < 0 || c.y() < 0 || c.z() < 0) {
            ASSERT_FALSE(octree.insideTreeBounds(OctantID(offset + c, 0)));
        } else {
            ASSERT_TRUE(octree.insideTreeBounds(OctantID(offset + c, 0)));
        }
    }

    ASSERT_FALSE(octree.insideTreeBounds(OctantID(Vector3i(0, 0, 0), 0)));
}

TEST(LinearOctreeTest, maximumLowerBoundsTest) {
    LinearOctree octree(OctantID(0, 2));
    octree.insert(OctantID(0, 1));
    octree.insert(OctantID(8, 1));
    octree.insert(OctantID(16, 1));
    octree.insert(OctantID(24, 1));
    octree.insert(OctantID(32, 1));
    octree.insert(OctantID(40, 1));
    octree.insert(OctantID(48, 1));
    octree.insert(OctantID(56, 1));
    octree.sortAndRemove();

    OctantID result(999, 999);
    for (morton_t resultMcode = 0; resultMcode < 64; resultMcode += 8) {
        for (morton_t i = resultMcode; i < resultMcode + 8; i++) {
            ASSERT_THAT(octree.maximumLowerBound(OctantID(i, 0), result), ::testing::Eq(true));
            ASSERT_THAT(result, ::testing::Eq(OctantID(resultMcode, 1)));
        }
    }

    ASSERT_THAT(octree.maximumLowerBound(OctantID(0, 1), result), ::testing::Eq(false));

    for (morton_t resultMcode = 8; resultMcode < 64; resultMcode += 8) {
        ASSERT_THAT(octree.maximumLowerBound(OctantID(resultMcode, 1), result), ::testing::Eq(true));
        ASSERT_THAT(result, ::testing::Eq(OctantID(resultMcode - 8, 1)));
    }
}

TEST(LinearOctreeTest, hasLeafTest) {
    LinearOctree octree(OctantID(0, 4));
    octree.insert(OctantID(12, 0));
    octree.insert(OctantID(0, 1));
    octree.insert(OctantID(33, 3));
    octree.insert(OctantID(4, 2));

    octree.insert(OctantID(9, 0));

    ASSERT_TRUE(octree.hasLeaf(OctantID(12, 0)));
    ASSERT_TRUE(octree.hasLeaf(OctantID(0, 1)));
    ASSERT_TRUE(octree.hasLeaf(OctantID(33, 3)));
    ASSERT_TRUE(octree.hasLeaf(OctantID(4, 2)));
    ASSERT_TRUE(octree.hasLeaf(OctantID(9, 0)));

    ASSERT_FALSE(octree.hasLeaf(OctantID(13, 0)));
    ASSERT_FALSE(octree.hasLeaf(OctantID(34, 3)));

    // Different level => different node
    ASSERT_FALSE(octree.hasLeaf(OctantID(0, 0)));
    ASSERT_FALSE(octree.hasLeaf(OctantID(33, 2)));
    ASSERT_FALSE(octree.hasLeaf(OctantID(33, 1)));
}

TEST(LinearOctreeTest, replaceWithChildrenTest) {
    LinearOctree octree(OctantID(0, 2), {OctantID(0, 2)});

    octree.replaceWithChildren(OctantID(0, 2));
    octree.sortAndRemove();

    EXPECT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(0, 1), OctantID(8, 1), OctantID(16, 1), OctantID(24, 1), OctantID(32, 1), OctantID(40, 1),
                                                       OctantID(48, 1), OctantID(56, 1)));

    octree.replaceWithChildren(OctantID(8, 1));
    octree.replaceWithChildren(OctantID(8, 1));  // Should be ignored
    octree.sortAndRemove();

    EXPECT_THAT(octree.leafs(),
                ::testing::ElementsAreArray(::std::vector<OctantID>{OctantID(0, 1), OctantID(8, 0), OctantID(9, 0), OctantID(10, 0), OctantID(11, 0),
                                                                  OctantID(12, 0), OctantID(13, 0), OctantID(14, 0), OctantID(15, 0), OctantID(16, 1),
                                                                  OctantID(24, 1), OctantID(32, 1), OctantID(40, 1), OctantID(48, 1), OctantID(56, 1)}));
}

TEST(LinearOctreeTest, replaceNonExistingOctantWithChildrenTest) {
    LinearOctree octree(OctantID(0, 2));
    octree.replaceWithChildren(OctantID(0, 1));
    octree.sortAndRemove();

    EXPECT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(0, 0), OctantID(1, 0), OctantID(2, 0), OctantID(3, 0), OctantID(4, 0), OctantID(5, 0),
                                                       OctantID(6, 0), OctantID(7, 0)));
}

TEST(LinearOctreeTest, replaceOctantOutsideBoundsWithChildrenTest) {
    LinearOctree octree(OctantID(0, 2));
    EXPECT_ANY_THROW(octree.replaceWithChildren(OctantID(64, 1)));
}

TEST(LinearOctreeTest, replaceWithSubtreeTest) {
    LinearOctree octree(OctantID(0, 2), {OctantID(0, 1), OctantID(8, 1), OctantID(16, 1)});

    octree.replaceWithSubtree(OctantID(16, 1), {OctantID(16, 0), OctantID(17, 0), OctantID(18, 0), OctantID(19, 0)});
    octree.sortAndRemove();

    EXPECT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(0, 1), OctantID(8, 1), OctantID(16, 0), OctantID(17, 0), OctantID(18, 0), OctantID(19, 0)));

    octree.replaceWithSubtree(OctantID(0, 1), {OctantID(0, 0)});
    octree.replaceWithSubtree(OctantID(0, 1), {OctantID(1, 0)});  // Should be ignored
    octree.sortAndRemove();

    EXPECT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(0, 0), OctantID(8, 1), OctantID(16, 0), OctantID(17, 0), OctantID(18, 0), OctantID(19, 0)));
}

TEST(LinearOctreeTest, replaceNonExistingOctantWithSubtreeTest) {
    LinearOctree octree(OctantID(0, 2));
    octree.replaceWithSubtree(OctantID(0, 1), {OctantID(0, 0), OctantID(1, 0), OctantID(2, 0)});
    octree.sortAndRemove();

    EXPECT_THAT(octree.leafs(), ::testing::ElementsAre(OctantID(0, 0), OctantID(1, 0), OctantID(2, 0)));
}

TEST(LinearOctreeTest, replaceOctantOutsideBoundsWithSubtreeTest) {
    LinearOctree octree(OctantID(0, 2));
    EXPECT_ANY_THROW(octree.replaceWithSubtree(OctantID(64, 1), {OctantID(64, 0)}));
}

TEST(LinearOctreeTest, deepestLastDecendantTest) {
    LinearOctree octree(OctantID(0, 1));
    ASSERT_EQ(OctantID(7, 0), octree.deepestLastDecendant());

    octree = LinearOctree(OctantID(0, 2));
    ASSERT_EQ(OctantID(63, 0), octree.deepestLastDecendant());

    octree = LinearOctree(OctantID(8, 1));
    ASSERT_EQ(OctantID(15, 0), octree.deepestLastDecendant());

    octree = LinearOctree(OctantID(64, 2));
    ASSERT_EQ(OctantID(127, 0), octree.deepestLastDecendant());
}

TEST(LinearOctreeTest, deepestFirstDecendantTest) {
    LinearOctree octree(OctantID(0, 1));
    ASSERT_EQ(OctantID(0, 0), octree.deepestFirstDecendant());

    octree = LinearOctree(OctantID(0, 2));
    ASSERT_EQ(OctantID(0, 0), octree.deepestFirstDecendant());

    octree = LinearOctree(OctantID(8, 1));
    ASSERT_EQ(OctantID(8, 0), octree.deepestFirstDecendant());

    octree = LinearOctree(OctantID(64, 2));
    ASSERT_EQ(OctantID(64, 0), octree.deepestFirstDecendant());
}
