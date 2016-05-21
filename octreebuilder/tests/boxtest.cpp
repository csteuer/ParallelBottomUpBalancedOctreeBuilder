#include <gmock/gmock.h>

#include <box.h>

TEST(BoxTest, equalityTest) {
    EXPECT_EQ(Box(Vector3i(0), Vector3i(3)), Box(Vector3i(0), Vector3i(3)));

    EXPECT_NE(Box(Vector3i(0), Vector3i(3)), Box(Vector3i(3), Vector3i(6)));

    EXPECT_EQ(Box(), Box());
}

TEST(BoxTest, volumeTest) {
    Box unitBox(Vector3i(0), Vector3i(1));
    EXPECT_EQ(1, unitBox.volume());

    Box nullBox;
    EXPECT_EQ(0, nullBox.volume());

    Box invalidBox(Vector3i(1), Vector3i(1));
    EXPECT_EQ(0, invalidBox.volume());

    Box squareBox(Vector3i(0), Vector3i(2));
    EXPECT_EQ(8, squareBox.volume());
}

TEST(BoxTest, validTest) {
    Box nullBox;
    EXPECT_FALSE(nullBox.valid());

    Box zeroVolumeBox(Vector3i(100), Vector3i(100));
    EXPECT_FALSE(zeroVolumeBox.valid());

    Box negativeVolumeBox(Vector3i(0), Vector3i(-1));
    EXPECT_FALSE(negativeVolumeBox.valid());

    Box unitBox(Vector3i(0), Vector3i(1));
    EXPECT_TRUE(unitBox.valid());

    Box largeBox(Vector3i(4, 2, 3), Vector3i(10, 8, 9));
    EXPECT_TRUE(largeBox.valid());
}

TEST(BoxTest, containsPointTest) {
    Box testBox(Vector3i(0), Vector3i(4));

    EXPECT_TRUE(testBox.contains(Vector3i(2)));

    EXPECT_TRUE(testBox.contains(Vector3i(0)));

    EXPECT_TRUE(testBox.contains(Vector3i(4)));

    EXPECT_TRUE(testBox.contains(Vector3i(3, 1, 0)));

    EXPECT_FALSE(testBox.contains(Vector3i(-1)));

    EXPECT_FALSE(testBox.contains(Vector3i(5)));

    EXPECT_FALSE(testBox.contains(Vector3i(4, 4, 5)));
}

TEST(BoxTest, containsBox) {
    Box testBox(Vector3i(0), Vector3i(4));

    EXPECT_TRUE(testBox.contains(Box(Vector3i(1), Vector3i(3))));

    EXPECT_TRUE(testBox.contains(testBox));

    EXPECT_TRUE(testBox.contains(Box(Vector3i(0), Vector3i(1))));

    EXPECT_TRUE(testBox.contains(Box(Vector3i(3), Vector3i(4))));

    EXPECT_FALSE(testBox.contains(Box(Vector3i(-1), Vector3i(0))));

    EXPECT_FALSE(testBox.contains(Box(Vector3i(2), Vector3i(5, 3, 3))));

    EXPECT_FALSE(testBox.contains(Box(Vector3i(3), Vector3i(5))));
}
