#include <gmock/gmock.h>

#include <vector_utils.h>
#include <box.h>

TEST(VoxelCoordinateUtilsTest, emptyVoxelRangeTest) {

    // This tests the iterator as well as empty() and size() methods
    EXPECT_THAT(VectorSpace(Vector3i(0)), ::testing::AllOf(::testing::ElementsAre(), ::testing::IsEmpty(), ::testing::SizeIs(0)));
    EXPECT_THAT(VectorSpace(Vector3i(-1)), ::testing::AllOf(::testing::ElementsAre(), ::testing::IsEmpty(), ::testing::SizeIs(0)));
}

TEST(VoxelCoordinateUtilsTest, linearVoxelRangeTest) {

    EXPECT_THAT(VectorSpace(Vector3i(5, 1, 1)), ::testing::AllOf(
                    ::testing::ElementsAre(Vector3i(0, 0, 0), Vector3i(1, 0, 0), Vector3i(2, 0, 0), Vector3i(3, 0, 0), Vector3i(4, 0, 0)),
                    ::testing::SizeIs(5)));

    EXPECT_THAT(VectorSpace(Vector3i(1, 5, 1)), ::testing::AllOf(
                    ::testing::ElementsAre(Vector3i(0, 0, 0), Vector3i(0, 1, 0), Vector3i(0, 2, 0), Vector3i(0, 3, 0), Vector3i(0, 4, 0)),
                    ::testing::SizeIs(5)));

    EXPECT_THAT(VectorSpace(Vector3i(1, 1, 5)), ::testing::AllOf(
                    ::testing::ElementsAre(Vector3i(0, 0, 0), Vector3i(0, 0, 1), Vector3i(0, 0, 2), Vector3i(0, 0, 3), Vector3i(0, 0, 4)),
                    ::testing::SizeIs(5)));
}

TEST(VoxelCoordinateUtilsTest, blockVoxelRangeTest) {

    EXPECT_THAT(VectorSpace(Vector3i(2, 2, 2)), ::testing::AllOf(

                    // We explicitly expected an ascending order with priority x,y,z (VoxelGrids can optimize layout accordingly)
                    ::testing::ElementsAre(Vector3i(0, 0, 0), Vector3i(1, 0, 0),
                                           Vector3i(0, 1, 0), Vector3i(1, 1, 0),
                                           Vector3i(0, 0, 1), Vector3i(1, 0, 1),
                                           Vector3i(0, 1, 1), Vector3i(1, 1, 1)),
                    ::testing::SizeIs(8)));


}

TEST(VoxelCoordinateUtilsTest, boxVoxelRangeTest) {

    EXPECT_THAT(VectorSpace(Box(Vector3i(0, 0, 0), Vector3i(2, 2, 2))), ::testing::AllOf(

                    // We explicitly expected an ascending order with priority x,y,z (VoxelGrids can optimize layout accordingly)
                    ::testing::ElementsAre(Vector3i(0, 0, 0), Vector3i(1, 0, 0),
                                           Vector3i(0, 1, 0), Vector3i(1, 1, 0),
                                           Vector3i(0, 0, 1), Vector3i(1, 0, 1),
                                           Vector3i(0, 1, 1), Vector3i(1, 1, 1)),
                    ::testing::SizeIs(8)));


}

TEST(VoxelCoordinateUtilsTest, closedVoxelRangeTest) {

    EXPECT_THAT(ClosedVectorSpace(Vector3i(1, 1, 1)), ::testing::AllOf(

                    // We explicitly expected an ascending order with priority x,y,z (VoxelGrids can optimize layout accordingly)
                    ::testing::ElementsAre(Vector3i(0, 0, 0), Vector3i(1, 0, 0),
                                           Vector3i(0, 1, 0), Vector3i(1, 1, 0),
                                           Vector3i(0, 0, 1), Vector3i(1, 0, 1),
                                           Vector3i(0, 1, 1), Vector3i(1, 1, 1)),
                    ::testing::SizeIs(8)));


}
