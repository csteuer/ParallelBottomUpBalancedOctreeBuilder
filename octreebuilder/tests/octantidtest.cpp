#include <gmock/gmock.h>

#include <octantid.h>
#include <linearoctree.h>

#include <vector_utils.h>

TEST(OctantIDTest, coordinateConstructorTest) {
    EXPECT_THAT(OctantID(Vector3i(0), 0), ::testing::Property(&OctantID::mcode, ::testing::Eq(getMortonCodeForCoordinate(Vector3i(0)))));
    EXPECT_THAT(OctantID(Vector3i(0), 1), ::testing::Property(&OctantID::mcode, ::testing::Eq(getMortonCodeForCoordinate(Vector3i(0)))));
    EXPECT_THAT(OctantID(Vector3i(1, 2, 3), 0), ::testing::Property(&OctantID::mcode, ::testing::Eq(getMortonCodeForCoordinate(Vector3i(1, 2, 3)))));
}

TEST(OctantIDTest, equalityTest) {
    EXPECT_THAT(OctantID(0, 0), ::testing::Eq(OctantID(0, 0)));
    EXPECT_THAT(OctantID(1, 0), ::testing::Eq(OctantID(1, 0)));
    EXPECT_THAT(OctantID(1, 1), ::testing::Eq(OctantID(1, 1)));

    EXPECT_THAT(OctantID(0, 1), ::testing::Not(::testing::Eq(OctantID(0, 0))));
    EXPECT_THAT(OctantID(1, 0), ::testing::Not(::testing::Eq(OctantID(2, 0))));
}

TEST(OctantIDTest, unEqualityTest) {
    EXPECT_THAT(OctantID(0, 1), ::testing::Ne(OctantID(0, 0)));
    EXPECT_THAT(OctantID(1, 0), ::testing::Ne(OctantID(2, 0)));

    EXPECT_THAT(OctantID(0, 0), ::testing::Not(::testing::Ne(OctantID(0, 0))));
    EXPECT_THAT(OctantID(1, 0), ::testing::Not(::testing::Ne(OctantID(1, 0))));
    EXPECT_THAT(OctantID(1, 1), ::testing::Not(::testing::Ne(OctantID(1, 1))));
}

TEST(OctantIDTest, coordTest) {
    EXPECT_EQ(OctantID(0, 0).coord(), Vector3i(0));
    EXPECT_EQ(OctantID(Vector3i(1, 2, 3), 0).coord(), Vector3i(1, 2, 3));
    EXPECT_EQ(OctantID(Vector3i(8, 12, 10), 2).coord(), Vector3i(8, 12, 10));
}

TEST(OctantIDTest, isDecendantTest) {
    EXPECT_TRUE(OctantID(0, 0).isDecendantOf(OctantID(0, 1)));

    EXPECT_TRUE(OctantID(Vector3i(6, 7, 5), 0).isDecendantOf(OctantID(Vector3i(4, 4, 4), 2)));
    EXPECT_TRUE(OctantID(Vector3i(6, 6, 6), 1).isDecendantOf(OctantID(Vector3i(4, 4, 4), 2)));

    EXPECT_FALSE(OctantID(Vector3i(6, 8, 5), 0).isDecendantOf(OctantID(Vector3i(4, 4, 4), 2)));
    EXPECT_FALSE(OctantID(Vector3i(6, 6, 8), 1).isDecendantOf(OctantID(Vector3i(4, 4, 4), 2)));

    EXPECT_FALSE(OctantID(Vector3i(4, 4, 4), 2).isDecendantOf(OctantID(Vector3i(4, 4, 4), 2)));
}

TEST(OctantIDTest, orderTest) {
    // Ordering must be by morton code
    EXPECT_THAT(OctantID(0, 0), ::testing::Lt(OctantID(1, 0)));

    // If morton code is equal parents must be less than their children
    EXPECT_THAT(OctantID(0, 1), ::testing::Lt(OctantID(0, 0)));

    EXPECT_THAT(OctantID(1, 0), ::testing::Not(::testing::Lt(OctantID(1, 0))));
    EXPECT_THAT(OctantID(0, 0), ::testing::Not(::testing::Lt(OctantID(0, 1))));
    EXPECT_THAT(OctantID(0, 0), ::testing::Not(::testing::Lt(OctantID(0, 0))));

    EXPECT_THAT(OctantID(0, 0), ::testing::Le(OctantID(1, 0)));
    EXPECT_THAT(OctantID(0, 1), ::testing::Le(OctantID(0, 0)));
    EXPECT_THAT(OctantID(0, 0), ::testing::Le(OctantID(0, 0)));
    EXPECT_THAT(OctantID(1, 0), ::testing::Not(::testing::Le(OctantID(0, 0))));

    EXPECT_THAT(OctantID(1, 0), ::testing::Gt(OctantID(0, 0)));
    EXPECT_THAT(OctantID(0, 0), ::testing::Gt(OctantID(0, 1)));
    EXPECT_THAT(OctantID(0, 0), ::testing::Not(::testing::Gt(OctantID(1, 0))));
    EXPECT_THAT(OctantID(0, 1), ::testing::Not(::testing::Gt(OctantID(0, 0))));
    EXPECT_THAT(OctantID(0, 0), ::testing::Not(::testing::Gt(OctantID(0, 0))));

    EXPECT_THAT(OctantID(1, 0), ::testing::Ge(OctantID(0, 0)));
    EXPECT_THAT(OctantID(0, 0), ::testing::Ge(OctantID(0, 1)));
    EXPECT_THAT(OctantID(0, 0), ::testing::Ge(OctantID(0, 0)));
    EXPECT_THAT(OctantID(0, 0), ::testing::Not(::testing::Ge(OctantID(1, 0))));
}

TEST(OctantIDTest, parentTest) {
    EXPECT_EQ(OctantID(Vector3i(0), 1), OctantID(Vector3i(0), 0).parent());
    EXPECT_EQ(OctantID(Vector3i(0), 1), OctantID(Vector3i(1, 1, 1), 0).parent());

    EXPECT_EQ(OctantID(Vector3i(4, 4, 0), 2), OctantID(Vector3i(6, 4, 2), 1).parent());
}

TEST(OctantIDTest, ancestorAtLevelTest) {
    EXPECT_EQ(OctantID(0, 0).ancestorAtLevel(1), OctantID(0, 0).parent());
    EXPECT_EQ(OctantID(7, 0).ancestorAtLevel(1), OctantID(0, 0).parent());

    EXPECT_EQ(OctantID(16, 1).ancestorAtLevel(4), OctantID(0, 4));
    EXPECT_EQ(OctantID(4288, 2).ancestorAtLevel(4), OctantID(4096, 4));

    EXPECT_EQ(OctantID(0, 0).ancestorAtLevel(0), OctantID(0, 0));
    EXPECT_EQ(OctantID(0, 1).ancestorAtLevel(1), OctantID(0, 1));

    EXPECT_ANY_THROW(OctantID(0, 1).ancestorAtLevel(0));
}

TEST(OctantIDTest, childrenTest) {
    EXPECT_THAT(OctantID(Vector3i(0), 1).children(), ::testing::ElementsAre(OctantID(0, 0), OctantID(1, 0), OctantID(2, 0), OctantID(3, 0), OctantID(4, 0),
                                                                            OctantID(5, 0), OctantID(6, 0), OctantID(7, 0)));

    OctantID parent(Vector3i(8, 8, 8), 3);
    EXPECT_THAT(parent.children(),
                ::testing::ElementsAre(OctantID(parent.mcode(), 2), OctantID(parent.mcode() + 64, 2), OctantID(parent.mcode() + 128, 2),
                                       OctantID(parent.mcode() + 192, 2), OctantID(parent.mcode() + 256, 2), OctantID(parent.mcode() + 320, 2),
                                       OctantID(parent.mcode() + 384, 2), OctantID(parent.mcode() + 448, 2)));

    EXPECT_ANY_THROW(OctantID(0, 0).children());
}

TEST(OctantIDTest, potentialNeighboursTest) {
    ASSERT_THAT(OctantID(Vector3i(0), 0).potentialNeighbours(LinearOctree(OctantID(0, 0))), ::testing::IsEmpty());

    auto neighbours = OctantID(Vector3i(0), 0).potentialNeighbours(LinearOctree(OctantID(0, 1)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(7));
    for (Vector3i c : VectorSpace(Vector3i(2))) {
        if (c != Vector3i(0)) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c, 0)));
        }
    }

    ASSERT_THAT(OctantID(Vector3i(1), 0).potentialNeighbours(LinearOctree(OctantID(0, 0))), ::testing::IsEmpty());

    neighbours = OctantID(Vector3i(1), 0).potentialNeighbours(LinearOctree(OctantID(0, 1)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(7));
    for (Vector3i c : VectorSpace(Vector3i(2))) {
        if (c != Vector3i(1)) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c, 0)));
        }
    }

    neighbours = OctantID(Vector3i(1), 0).potentialNeighbours(LinearOctree(OctantID(0, 2)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(26));
    for (Vector3i c : VectorSpace(Vector3i(3))) {
        if (c != Vector3i(1)) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c, 0)));
        }
    }

    neighbours = OctantID(Vector3i(8, 8, 0), 3).potentialNeighbours(LinearOctree(OctantID(0, 5)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(17));
    for (Vector3i c : VectorSpace(Vector3i(3, 3, 2))) {
        if (c != Vector3i(1, 1, 0)) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c * 8, 3)));
        }
    }

    neighbours = OctantID(Vector3i(8, 8, 8), 3).potentialNeighbours(LinearOctree(OctantID(0, 5)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(26));
    for (Vector3i c : VectorSpace(Vector3i(3, 3, 3))) {
        if (c != Vector3i(1, 1, 1)) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c * 8, 3)));
        }
    }

    neighbours = OctantID(Vector3i(12, 12, 12), 2).potentialNeighbours(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(7));
    for (Vector3i c : VectorSpace(Vector3i(2, 2, 2))) {
        if (c != Vector3i(1, 1, 1)) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(Vector3i(8, 8, 8) + c * 4, 2)));
        }
    }
}

TEST(OctantIDTest, potentialNeighboursWithoutSiblingsTest) {
    ASSERT_THAT(OctantID(Vector3i(0), 0).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 0))), ::testing::IsEmpty());
    ASSERT_THAT(OctantID(Vector3i(0), 0).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 1))), ::testing::IsEmpty());
    ASSERT_THAT(OctantID(Vector3i(1), 0).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 0))), ::testing::IsEmpty());
    ASSERT_THAT(OctantID(Vector3i(1), 0).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 1))), ::testing::IsEmpty());

    auto neighbours = OctantID(Vector3i(1), 0).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 2)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(19));
    for (Vector3i c : VectorSpace(Vector3i(3))) {
        if (c.x() > 1 || c.y() > 1 || c.z() > 1) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c, 0)));
        }
    }

    neighbours = OctantID(Vector3i(8, 8, 0), 3).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 5)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(10));
    for (Vector3i c : VectorSpace(Vector3i(3, 3, 2))) {
        if (c.x() > 1 || c.y() > 1) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c * 8, 3)));
        }
    }

    neighbours = OctantID(Vector3i(8, 8, 8), 3).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(0, 5)));
    ASSERT_THAT(neighbours, ::testing::SizeIs(19));
    for (Vector3i c : VectorSpace(Vector3i(3, 3, 3))) {
        if (c.x() > 1 || c.y() > 1 || c.z() > 1) {
            ASSERT_THAT(neighbours, ::testing::Contains(OctantID(c * 8, 3)));
        }
    }

    ASSERT_THAT(OctantID(Vector3i(12, 12, 12), 2).potentialNeighboursWithoutSiblings(LinearOctree(OctantID(Vector3i(8, 8, 8), 3))), ::testing::IsEmpty());
}

TEST(OctantIDTest, isBoundaryOctantAtOriginTest) {
    EXPECT_TRUE(OctantID(Vector3i(1, 0, 0), 0).isBoundaryOctant(LinearOctree(OctantID(0, 1)), LinearOctree(OctantID(0, 2))));
    EXPECT_TRUE(OctantID(Vector3i(2, 0, 0), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(2, 0, 0), 1)), LinearOctree(OctantID(0, 2))));

    EXPECT_FALSE(OctantID(Vector3i(0, 0, 0), 0).isBoundaryOctant(LinearOctree(OctantID(0, 1)), LinearOctree(OctantID(0, 2))));
    EXPECT_FALSE(OctantID(Vector3i(3, 0, 0), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(2, 0, 0), 1)), LinearOctree(OctantID(0, 2))));
}

TEST(OctantIDTest, isBoundaryOctantNotAtOriginLevelZeroTest) {
    EXPECT_TRUE(OctantID(Vector3i(6, 6, 4), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_TRUE(OctantID(Vector3i(6, 4, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_TRUE(OctantID(Vector3i(4, 6, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_TRUE(OctantID(Vector3i(4, 4, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_TRUE(OctantID(Vector3i(4, 6, 4), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_TRUE(OctantID(Vector3i(6, 4, 4), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_TRUE(OctantID(Vector3i(4, 4, 4), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));

    EXPECT_FALSE(OctantID(Vector3i(6, 6, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(6, 6, 5), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(6, 5, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(5, 6, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(6, 5, 5), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(5, 5, 6), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(5, 6, 5), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
    EXPECT_FALSE(OctantID(Vector3i(5, 5, 5), 0).isBoundaryOctant(LinearOctree(OctantID(Vector3i(4, 4, 4), 2)), LinearOctree(OctantID(0, 3))));
}

TEST(OctantIDTest, isBoundaryOctantNotAtOriginLevelTwoTest) {
    EXPECT_TRUE(OctantID(Vector3i(12, 12, 8), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_TRUE(OctantID(Vector3i(12, 8, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_TRUE(OctantID(Vector3i(8, 12, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_TRUE(OctantID(Vector3i(8, 8, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_TRUE(OctantID(Vector3i(8, 12, 8), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_TRUE(OctantID(Vector3i(12, 8, 8), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_TRUE(OctantID(Vector3i(8, 8, 8), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));

    EXPECT_FALSE(OctantID(Vector3i(12, 12, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(12, 12, 14), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(12, 14, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(14, 12, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(12, 14, 14), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(14, 14, 12), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(14, 12, 14), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
    EXPECT_FALSE(OctantID(Vector3i(14, 14, 14), 1).isBoundaryOctant(LinearOctree(OctantID(Vector3i(8, 8, 8), 3)), LinearOctree(OctantID(0, 4))));
}

TEST(OctantIDTest, isBoundaryOctantVectorParameters) {
    EXPECT_TRUE(OctantID(Vector3i(2, 2, 2), 0).isBoundaryOctant(Vector3i(2, 2, 2), Vector3i(3, 3, 3), Vector3i(0, 0, 0), Vector3i(3, 3, 3)));
    EXPECT_FALSE(OctantID(Vector3i(3, 3, 3), 0).isBoundaryOctant(Vector3i(2, 2, 2), Vector3i(3, 3, 3), Vector3i(0, 0, 0), Vector3i(3, 3, 3)));

    EXPECT_TRUE(OctantID(Vector3i(6, 6, 4), 1).isBoundaryOctant(Vector3i(4, 4, 4), Vector3i(7, 7, 7), Vector3i(0, 0, 0), Vector3i(7, 7, 7)));
    EXPECT_FALSE(OctantID(Vector3i(6, 6, 6), 1).isBoundaryOctant(Vector3i(4, 4, 4), Vector3i(7, 7, 7), Vector3i(0, 0, 0), Vector3i(7, 7, 7)));
}
