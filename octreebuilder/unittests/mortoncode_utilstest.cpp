#include <gmock/gmock.h>
#include <mortoncode_utils.h>
#include <vector_utils.h>

#include <limits>

using namespace octreebuilder;

TEST(MortonCodeUtilsTest, fitsInMortonCodeUtilsTest) {
    EXPECT_FALSE(fitsInMortonCode(Vector3i(std::numeric_limits<coord_t>::max())));
    EXPECT_TRUE(fitsInMortonCode(Vector3i(10000, 10000, 10000)));
    EXPECT_TRUE(fitsInMortonCode(Vector3i(100000, 100000, 100000)));
    EXPECT_TRUE(fitsInMortonCode(Vector3i(1000000, 1000000, 1000000)));
}

TEST(MortonCodeUtilsTest, getMaxLevelsForBoundingTest) {
    EXPECT_EQ(3, getOctreeDepthForBounding(Vector3i(7, 7, 7)));
    EXPECT_EQ(3, getOctreeDepthForBounding(Vector3i(7, 7, 0)));

    EXPECT_EQ(4, getOctreeDepthForBounding(Vector3i(8, 7, 7)));
}

TEST(MortonCodeUtilsTest, getMaxXYZForOctreeDepthTest) {
    EXPECT_EQ(Vector3i(0), getMaxXYZForOctreeDepth(0));
    EXPECT_EQ(Vector3i(1, 1, 1), getMaxXYZForOctreeDepth(1));
    EXPECT_EQ(Vector3i(3, 3, 3), getMaxXYZForOctreeDepth(2));
    EXPECT_EQ(Vector3i(7, 7, 7), getMaxXYZForOctreeDepth(3));
    EXPECT_EQ(Vector3i(15, 15, 15), getMaxXYZForOctreeDepth(4));
}

TEST(MortonCodeUtilsTest, getNodeSizeForLevelTest) {
    EXPECT_EQ(1, getOctantSizeForLevel(0));
    EXPECT_EQ(2, getOctantSizeForLevel(1));
    EXPECT_EQ(4, getOctantSizeForLevel(2));
    EXPECT_EQ(8, getOctantSizeForLevel(3));
}

TEST(MortonCodeUtilsTest, getMortonCodeForCoordinateTest) {
    EXPECT_EQ(0, getMortonCodeForCoordinate(Vector3i(0)));
    EXPECT_EQ(1, getMortonCodeForCoordinate(Vector3i(0, 0, 1)));
    EXPECT_EQ(2, getMortonCodeForCoordinate(Vector3i(0, 1, 0)));
    EXPECT_EQ(3, getMortonCodeForCoordinate(Vector3i(0, 1, 1)));
    EXPECT_EQ(4, getMortonCodeForCoordinate(Vector3i(1, 0, 0)));
    EXPECT_EQ(5, getMortonCodeForCoordinate(Vector3i(1, 0, 1)));
    EXPECT_EQ(6, getMortonCodeForCoordinate(Vector3i(1, 1, 0)));
    EXPECT_EQ(7, getMortonCodeForCoordinate(Vector3i(1, 1, 1)));
    EXPECT_EQ(8, getMortonCodeForCoordinate(Vector3i(0, 0, 2)));
    EXPECT_EQ(174, getMortonCodeForCoordinate(Vector3i(3, 5, 2)));
    EXPECT_EQ(5376, getMortonCodeForCoordinate(Vector3i(4, 8, 16)));
}

TEST(MortonCodeUtilsTest, getCoordinateForMortonCodeUtilsTest) {
    EXPECT_EQ(Vector3i(0), getCoordinateForMortonCode(0));
    EXPECT_EQ(Vector3i(1, 1, 1), getCoordinateForMortonCode(7));
    EXPECT_EQ(Vector3i(3, 0, 3), getCoordinateForMortonCode(45));
    EXPECT_EQ(Vector3i(2, 1, 2), getCoordinateForMortonCode(42));
    EXPECT_EQ(Vector3i(7, 7, 7), getCoordinateForMortonCode(511));
    EXPECT_EQ(Vector3i(4, 31, 52), getCoordinateForMortonCode(46546));
}

TEST(MortonCodeUtilsTest, getMortonCodeForParentTest) {
    EXPECT_EQ(8, getMortonCodeForParent(8, 0));   // (0, 0, 2)
    EXPECT_EQ(8, getMortonCodeForParent(9, 0));   // (0, 0, 3)
    EXPECT_EQ(8, getMortonCodeForParent(10, 0));  // (0, 1, 2)
    EXPECT_EQ(8, getMortonCodeForParent(11, 0));  // (0, 1, 3)
    EXPECT_EQ(8, getMortonCodeForParent(12, 0));  // (1, 0, 2)
    EXPECT_EQ(8, getMortonCodeForParent(13, 0));  // (1, 0, 3)
    EXPECT_EQ(8, getMortonCodeForParent(14, 0));  // (1, 1, 2)
    EXPECT_EQ(8, getMortonCodeForParent(15, 0));  // (1, 1, 3)

    EXPECT_EQ(192, getMortonCodeForParent(232, 1));  // Parent of rectangle (2, 4, 6)->(4, 6, 8) = 232 is (0, 4, 4) -> (4, 8, 8) = 192
    EXPECT_EQ(192, getMortonCodeForParent(239, 1));  // Ignore bits of level 0
}

TEST(MortonCodeUtilsTest, getMortonCodeForAncestorTest) {
    EXPECT_EQ(0, getMortonCodeForAncestor(0, 0, 4));

    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(6, 4, 0)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(4, 6, 0)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(4, 4, 2)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(6, 6, 0)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(6, 4, 2)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(4, 6, 2)), 0, 2));
    EXPECT_EQ(getMortonCodeForCoordinate(Vector3i(4, 4, 0)), getMortonCodeForAncestor(getMortonCodeForCoordinate(Vector3i(6, 6, 2)), 0, 2));

    EXPECT_EQ(0, getMortonCodeForAncestor(0, 0, 0));
    EXPECT_EQ(0, getMortonCodeForAncestor(0, 1, 1));
    EXPECT_ANY_THROW(getMortonCodeForAncestor(0, 1, 0));
}

TEST(MortonCodeUtilsTest, getMortonCodesForChildrenTest) {
    EXPECT_ANY_THROW(getMortonCodesForChildren(getMortonCodeForCoordinate({0, 0, 0}), 0)) << "Leaf nodes can't have children.";

    // Test for direct parent of leaf nodes (most simple case parent at origin)
    EXPECT_THAT(getMortonCodesForChildren(0, 1), ::testing::ElementsAre(0, 1, 2, 3, 4, 5, 6, 7));

    // Same as above but for parent not at the origin
    morton_t parent = getMortonCodeForCoordinate({2, 4, 8});
    EXPECT_THAT(getMortonCodesForChildren(parent, 1),
                ::testing::ElementsAre(parent, parent + 1, parent + 2, parent + 3, parent + 4, parent + 5, parent + 6, parent + 7));

    // Test for level 2 parent (has children of size 2)
    EXPECT_THAT(getMortonCodesForChildren(0, 2), ::testing::ElementsAre(0, 8, 16, 24, 32, 40, 48, 56));

    // Note that at level 2 the node with llf (0, 0, 0) contains (1, 1, 1)... hence the result should be the same as above
    EXPECT_THAT(getMortonCodesForChildren(getMortonCodeForCoordinate({1, 1, 1}), 2),
                ::testing::ElementsAre(0, 8, 16, 24, 32, 40, 48, 56));
}

TEST(MortonCodeUtilsTest, getMortonCodesForNeighbourNodesTest) {
    // current level must not be greater than the depth of the tree
    EXPECT_ANY_THROW(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({0, 0, 0}), 1, 0));

    // node must not be outside of the tree
    EXPECT_ANY_THROW(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({1, 0, 0}), 0, 0));

    // in a tree with one node (depth = 0) the node can't have any neighbours
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({0, 0, 0}), 0, 0), ::testing::IsEmpty());

    // full neighbour hood 1x1x1 node
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({1, 1, 1}), 0, 2),
                ::testing::UnorderedElementsAreArray(
                    {getMortonCodeForCoordinate({0, 0, 0}), getMortonCodeForCoordinate({0, 0, 1}), getMortonCodeForCoordinate({0, 1, 0}),
                     getMortonCodeForCoordinate({0, 1, 1}), getMortonCodeForCoordinate({1, 0, 0}), getMortonCodeForCoordinate({1, 0, 1}),
                     getMortonCodeForCoordinate({1, 1, 0}), getMortonCodeForCoordinate({0, 0, 2}), getMortonCodeForCoordinate({0, 1, 2}),
                     getMortonCodeForCoordinate({1, 0, 2}), getMortonCodeForCoordinate({1, 1, 2}), getMortonCodeForCoordinate({0, 2, 0}),
                     getMortonCodeForCoordinate({0, 2, 1}), getMortonCodeForCoordinate({1, 2, 0}), getMortonCodeForCoordinate({1, 2, 1}),
                     getMortonCodeForCoordinate({2, 0, 0}), getMortonCodeForCoordinate({2, 0, 1}), getMortonCodeForCoordinate({2, 1, 1}),
                     getMortonCodeForCoordinate({2, 1, 0}), getMortonCodeForCoordinate({0, 2, 2}), getMortonCodeForCoordinate({1, 2, 2}),
                     getMortonCodeForCoordinate({2, 2, 0}), getMortonCodeForCoordinate({2, 2, 1}), getMortonCodeForCoordinate({2, 0, 2}),
                     getMortonCodeForCoordinate({2, 1, 2}), getMortonCodeForCoordinate({2, 2, 2})}));

    // neighbour hood of 1x1x1 lower left front border node
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({0, 0, 0}), 0, 1),
                ::testing::UnorderedElementsAreArray({getMortonCodeForCoordinate({0, 0, 1}), getMortonCodeForCoordinate({0, 1, 0}),
                                                      getMortonCodeForCoordinate({0, 1, 1}), getMortonCodeForCoordinate({1, 0, 0}),
                                                      getMortonCodeForCoordinate({1, 0, 1}), getMortonCodeForCoordinate({1, 1, 0}),
                                                      getMortonCodeForCoordinate({1, 1, 1})}));

    // full neighbour hood 1x1x1 upper right back border node
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({1, 1, 1}), 0, 1),
                ::testing::UnorderedElementsAreArray({getMortonCodeForCoordinate({0, 0, 0}), getMortonCodeForCoordinate({0, 0, 1}),
                                                      getMortonCodeForCoordinate({0, 1, 0}), getMortonCodeForCoordinate({0, 1, 1}),
                                                      getMortonCodeForCoordinate({1, 0, 0}), getMortonCodeForCoordinate({1, 0, 1}),
                                                      getMortonCodeForCoordinate({1, 1, 0})}));

    // full neighbour hood 2x2x2 node
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({2, 2, 2}), 1, 3),
                ::testing::UnorderedElementsAreArray(
                    {getMortonCodeForCoordinate({0, 0, 0}), getMortonCodeForCoordinate({0, 0, 2}), getMortonCodeForCoordinate({0, 2, 0}),
                     getMortonCodeForCoordinate({0, 2, 2}), getMortonCodeForCoordinate({2, 0, 0}), getMortonCodeForCoordinate({2, 0, 2}),
                     getMortonCodeForCoordinate({2, 2, 0}), getMortonCodeForCoordinate({0, 0, 4}), getMortonCodeForCoordinate({0, 2, 4}),
                     getMortonCodeForCoordinate({2, 0, 4}), getMortonCodeForCoordinate({2, 2, 4}), getMortonCodeForCoordinate({0, 4, 0}),
                     getMortonCodeForCoordinate({0, 4, 2}), getMortonCodeForCoordinate({2, 4, 0}), getMortonCodeForCoordinate({2, 4, 2}),
                     getMortonCodeForCoordinate({4, 0, 0}), getMortonCodeForCoordinate({4, 0, 2}), getMortonCodeForCoordinate({4, 2, 2}),
                     getMortonCodeForCoordinate({4, 2, 0}), getMortonCodeForCoordinate({0, 4, 4}), getMortonCodeForCoordinate({2, 4, 4}),
                     getMortonCodeForCoordinate({4, 4, 0}), getMortonCodeForCoordinate({4, 4, 2}), getMortonCodeForCoordinate({4, 0, 4}),
                     getMortonCodeForCoordinate({4, 2, 4}), getMortonCodeForCoordinate({4, 4, 4})}));

    // neighbour hood of 2x2x2 lower left front border node
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({0, 0, 0}), 1, 3),
                ::testing::UnorderedElementsAreArray({getMortonCodeForCoordinate({0, 0, 2}), getMortonCodeForCoordinate({0, 2, 0}),
                                                      getMortonCodeForCoordinate({0, 2, 2}), getMortonCodeForCoordinate({2, 0, 0}),
                                                      getMortonCodeForCoordinate({2, 0, 2}), getMortonCodeForCoordinate({2, 2, 0}),
                                                      getMortonCodeForCoordinate({2, 2, 2})}));

    // neighbour hood of 2x2x2 upper right back border node
    EXPECT_THAT(getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({6, 6, 6}), 1, 3),
                ::testing::UnorderedElementsAreArray({getMortonCodeForCoordinate({4, 4, 4}), getMortonCodeForCoordinate({4, 4, 6}),
                                                      getMortonCodeForCoordinate({4, 6, 4}), getMortonCodeForCoordinate({4, 6, 6}),
                                                      getMortonCodeForCoordinate({6, 4, 4}), getMortonCodeForCoordinate({6, 4, 6}),
                                                      getMortonCodeForCoordinate({6, 6, 4})}));
}

TEST(MortonCodeUtilsTest, getMortonCodesForNeighbourNodesWithRootNotAtOriginTest) {
    ASSERT_THAT(
        getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({4, 4, 4}), 0, 2, Vector3i(4, 4, 4)),
        ::testing::UnorderedElementsAre(getMortonCodeForCoordinate({5, 4, 4}), getMortonCodeForCoordinate({4, 5, 4}), getMortonCodeForCoordinate({4, 4, 5}),
                                        getMortonCodeForCoordinate({5, 5, 4}), getMortonCodeForCoordinate({5, 4, 5}), getMortonCodeForCoordinate({4, 5, 5}),
                                        getMortonCodeForCoordinate({5, 5, 5})));

    const auto neighbourCodes = getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({5, 5, 5}), 0, 2, Vector3i(4, 4, 4));

    ASSERT_THAT(neighbourCodes, ::testing::SizeIs(26));
    for (Vector3i c : VectorSpace(Vector3i(4), Vector3i(6))) {
        if (c != Vector3i(5)) {
            ASSERT_THAT(neighbourCodes, ::testing::Contains(getMortonCodeForCoordinate(c)));
        }
    }

    ASSERT_THAT(
        getMortonCodesForNeighbourOctants(getMortonCodeForCoordinate({6, 6, 6}), 1, 2, Vector3i(4, 4, 4)),
        ::testing::UnorderedElementsAre(getMortonCodeForCoordinate({4, 4, 4}), getMortonCodeForCoordinate({6, 4, 4}), getMortonCodeForCoordinate({4, 6, 4}),
                                        getMortonCodeForCoordinate({4, 4, 6}), getMortonCodeForCoordinate({6, 6, 4}), getMortonCodeForCoordinate({6, 4, 6}),
                                        getMortonCodeForCoordinate({4, 6, 6})));
}

TEST(MortonCodeUtilsTest, getMaxLevelOfLLFTest) {
    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(0), 3));

    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(0, 0, 8), 3));
    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(0, 8, 0), 3));
    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(8, 0, 0), 3));
    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(0, 16, 8), 3));

    EXPECT_EQ(2, getMaxLevelOfLLF(Vector3i(0, 0, 4), 3));
    EXPECT_EQ(2, getMaxLevelOfLLF(Vector3i(0, 4, 0), 3));
    EXPECT_EQ(2, getMaxLevelOfLLF(Vector3i(4, 0, 0), 3));
    EXPECT_EQ(2, getMaxLevelOfLLF(Vector3i(20, 16, 4), 3));

    EXPECT_EQ(1, getMaxLevelOfLLF(Vector3i(0, 0, 2), 3));
    EXPECT_EQ(1, getMaxLevelOfLLF(Vector3i(0, 2, 0), 3));
    EXPECT_EQ(1, getMaxLevelOfLLF(Vector3i(2, 0, 0), 3));
    EXPECT_EQ(1, getMaxLevelOfLLF(Vector3i(16, 4, 2), 3));

    EXPECT_EQ(0, getMaxLevelOfLLF(Vector3i(0, 0, 1), 3));
    EXPECT_EQ(0, getMaxLevelOfLLF(Vector3i(0, 1, 0), 3));
    EXPECT_EQ(0, getMaxLevelOfLLF(Vector3i(1, 0, 0), 3));
    EXPECT_EQ(0, getMaxLevelOfLLF(Vector3i(4, 2, 1), 3));

    EXPECT_EQ(2, getMaxLevelOfLLF(Vector3i(12, 0, 4), 5));

    EXPECT_EQ(0, getMaxLevelOfLLF(Vector3i(10, 2, 3), 5));

    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(16, 0, 56), 5));
    EXPECT_EQ(3, getMaxLevelOfLLF(Vector3i(16, 8, 56), 5));
}

TEST(MortonCodeUtilsTest, getSearchCornerTest) {
    ASSERT_EQ(Vector3i(0), getSearchCorner(getMortonCodeForCoordinate(Vector3i(0)), 0));
    ASSERT_EQ(Vector3i(2, 0, 0), getSearchCorner(getMortonCodeForCoordinate(Vector3i(1, 0, 0)), 0));
    ASSERT_EQ(Vector3i(2, 2, 0), getSearchCorner(getMortonCodeForCoordinate(Vector3i(1, 1, 0)), 0));
    ASSERT_EQ(Vector3i(0, 2, 0), getSearchCorner(getMortonCodeForCoordinate(Vector3i(0, 1, 0)), 0));

    ASSERT_EQ(Vector3i(0, 0, 2), getSearchCorner(getMortonCodeForCoordinate(Vector3i(0, 0, 1)), 0));
    ASSERT_EQ(Vector3i(2, 0, 2), getSearchCorner(getMortonCodeForCoordinate(Vector3i(1, 0, 1)), 0));
    ASSERT_EQ(Vector3i(2, 2, 2), getSearchCorner(getMortonCodeForCoordinate(Vector3i(1, 1, 1)), 0));
    ASSERT_EQ(Vector3i(0, 2, 2), getSearchCorner(getMortonCodeForCoordinate(Vector3i(0, 1, 1)), 0));

    ASSERT_EQ(Vector3i(8, 8, 8), getSearchCorner(getMortonCodeForCoordinate(Vector3i(8, 8, 8)), 1));
    ASSERT_EQ(Vector3i(12, 8, 8), getSearchCorner(getMortonCodeForCoordinate(Vector3i(10, 8, 8)), 1));
    ASSERT_EQ(Vector3i(12, 12, 8), getSearchCorner(getMortonCodeForCoordinate(Vector3i(10, 10, 8)), 1));
    ASSERT_EQ(Vector3i(8, 12, 8), getSearchCorner(getMortonCodeForCoordinate(Vector3i(8, 10, 8)), 1));

    ASSERT_EQ(Vector3i(8, 8, 12), getSearchCorner(getMortonCodeForCoordinate(Vector3i(8, 8, 10)), 1));
    ASSERT_EQ(Vector3i(12, 8, 12), getSearchCorner(getMortonCodeForCoordinate(Vector3i(10, 8, 10)), 1));
    ASSERT_EQ(Vector3i(12, 12, 12), getSearchCorner(getMortonCodeForCoordinate(Vector3i(10, 10, 10)), 1));
    ASSERT_EQ(Vector3i(8, 12, 12), getSearchCorner(getMortonCodeForCoordinate(Vector3i(8, 10, 10)), 1));

    ASSERT_EQ(Vector3i(4, 4, 4), getSearchCorner(getMortonCodeForCoordinate(Vector3i(4, 4, 2)), 1));
}

TEST(MortonCodeUtilsTest, isDecendantTest) {
    ASSERT_FALSE(isMortonCodeDecendant(0, 0, 0, 0));
    ASSERT_FALSE(isMortonCodeDecendant(0, 1, 0, 0));
    ASSERT_TRUE(isMortonCodeDecendant(0, 0, 0, 1));

    ASSERT_TRUE(isMortonCodeDecendant(getMortonCodeForCoordinate(Vector3i(1, 1, 1)), 0, getMortonCodeForCoordinate(Vector3i(0)), 1));
    ASSERT_FALSE(isMortonCodeDecendant(getMortonCodeForCoordinate(Vector3i(1, 2, 1)), 0, getMortonCodeForCoordinate(Vector3i(0)), 1));

    for (morton_t m = 0; m < 1000; m++) {
        for (uint l = 0; l < 5; l++) {
            morton_t parent = getMortonCodeForParent(m, l);

            ASSERT_TRUE(isMortonCodeDecendant(m, 0, parent, l + 1));

            for (const morton_t& parent_sibling : getMortonCodesForChildren(parent, l + 2)) {
                if (parent_sibling != parent) {
                    ASSERT_FALSE(isMortonCodeDecendant(m, 0, parent_sibling, l + 1));
                }
            }
        }
    }
}

TEST(MortonCodeUtilsTest, nearestCommonAncestoTest) {
    ASSERT_THAT(nearestCommonAncestor(0, 0, 0, 0), ::testing::Eq(std::make_pair(morton_t(0), 0u)));
    ASSERT_THAT(nearestCommonAncestor(0, 1, 0, 0), ::testing::Eq(std::make_pair(morton_t(0), 1u)));

    ASSERT_THAT(nearestCommonAncestor(0, getMortonCodeForCoordinate({8, 8, 8}), 0, 3), ::testing::Eq(std::make_pair(morton_t(0), 4u)));
    ASSERT_THAT(nearestCommonAncestor(0, getMortonCodeForCoordinate({8, 8, 8}), 1, 3), ::testing::Eq(std::make_pair(morton_t(0), 4u)));
    ASSERT_THAT(nearestCommonAncestor(0, getMortonCodeForCoordinate({8, 8, 8}), 2, 3), ::testing::Eq(std::make_pair(morton_t(0), 4u)));
    ASSERT_THAT(nearestCommonAncestor(0, getMortonCodeForCoordinate({8, 8, 8}), 3, 3), ::testing::Eq(std::make_pair(morton_t(0), 4u)));
    ASSERT_THAT(nearestCommonAncestor(0, getMortonCodeForCoordinate({8, 8, 8}), 4, 3), ::testing::Eq(std::make_pair(morton_t(0), 4u)));
    ASSERT_THAT(nearestCommonAncestor(0, getMortonCodeForCoordinate({8, 8, 8}), 5, 3), ::testing::Eq(std::make_pair(morton_t(0), 5u)));

    ASSERT_THAT(nearestCommonAncestor(getMortonCodeForCoordinate({14, 12, 10}), getMortonCodeForCoordinate({15, 9, 14}), 1, 0),
                ::testing::Eq(std::make_pair(getMortonCodeForCoordinate({8, 8, 8}), 3u)));
}
