#include <gmock/gmock.h>

#include <vector3i.h>

using namespace octreebuilder;

TEST(VectorTest, defaultConstructorTest) {
    const Vector3i v;

    ASSERT_EQ(0, v.x());
    ASSERT_EQ(0, v.y());
    ASSERT_EQ(0, v.z());
}

TEST(VectorTest, threeComponentsConstructorTest) {
    const Vector3i v(1, -2, 3);

    ASSERT_EQ(1, v.x());
    ASSERT_EQ(-2, v.y());
    ASSERT_EQ(3, v.z());
}

TEST(VectorTest, threeComponentsConstructorTest2) {
    Vector3i v(-1, -2, -3);

    ASSERT_EQ(-1, v.x());
    ASSERT_EQ(-2, v.y());
    ASSERT_EQ(-3, v.z());
}

TEST(VectorTest, singleValueConstructorTest) {
    const Vector3i v(-1);

    ASSERT_EQ(-1, v.x());
    ASSERT_EQ(-1, v.y());
    ASSERT_EQ(-1, v.z());
}

TEST(VectorTest, initializerListConstructorTest) {
    const Vector3i v({1, -2, 3});

    ASSERT_EQ(1, v.x());
    ASSERT_EQ(-2, v.y());
    ASSERT_EQ(3, v.z());

    ASSERT_THROW(Vector3i({1, 2, 3, 4}), std::invalid_argument);
}

TEST(VectorTest, equalityOperatorTest) {
    ASSERT_TRUE(Vector3i() == Vector3i());
    ASSERT_TRUE(Vector3i(0, 0, 0) == Vector3i());

    ASSERT_TRUE(Vector3i(1) == Vector3i(1));
    ASSERT_TRUE(Vector3i(1, 1, 1) == Vector3i(1));

    ASSERT_TRUE(Vector3i(-3) == Vector3i(-3));
    ASSERT_TRUE(Vector3i(-3, -3, -3) == Vector3i(-3));

    ASSERT_TRUE(Vector3i(1, 2, 3) == Vector3i(1, 2, 3));

    ASSERT_FALSE(Vector3i(1, 2, 3) == Vector3i(3, 2, 1));
    ASSERT_FALSE(Vector3i(1, 1, 1) == Vector3i(1, -1, 1));
}

TEST(VectorTest, unEqualityOperatorTest) {
    ASSERT_FALSE(Vector3i() != Vector3i());
    ASSERT_FALSE(Vector3i(1) != Vector3i(1));
    ASSERT_FALSE(Vector3i(1, 2, 3) != Vector3i(1, 2, 3));
    ASSERT_FALSE(Vector3i(-3) != Vector3i(-3));

    ASSERT_TRUE(Vector3i(1, 2, 3) != Vector3i(3, 2, 1));
    ASSERT_TRUE(Vector3i(1, 1, 1) != Vector3i(1, -1, 1));
}

TEST(VectorTest, orderOperatorTest) {
    Vector3i a(1, 0, 0);
    Vector3i b(0, 1, 0);
    Vector3i c(0, 0, 1);

    ASSERT_TRUE(a < b || b < a);
    ASSERT_TRUE(a < c || c < a);
    ASSERT_TRUE(b < c || c < b);

    ASSERT_FALSE(a < a);
    ASSERT_FALSE(b < b);
    ASSERT_FALSE(c < c);

    if (a < b && b < c) {
        ASSERT_TRUE(a < c);
    }

    if (b < a && a < c) {
        ASSERT_TRUE(b < c);
    }

    if (a < c && c < b) {
        ASSERT_TRUE(a < b);
    }

    if (c < a && a < b) {
        ASSERT_TRUE(c < b);
    }

    if (b < c && c < a) {
        ASSERT_TRUE(b < a);
    }

    if (c < b && b < a) {
        ASSERT_TRUE(c < a);
    }
}

TEST(VectorTest, arrayAccessOperatorTest) {
    const Vector3i v({1, 2, 3});

    ASSERT_EQ(1, v[0]);
    ASSERT_EQ(2, v[1]);
    ASSERT_EQ(3, v[2]);

    ASSERT_THROW(v[4], std::out_of_range);
}

TEST(VectorTest, inverseOperatorTest) {
    ASSERT_EQ(Vector3i(-1, -2, -3), -Vector3i(1, 2, 3));
    ASSERT_EQ(Vector3i(-1, 2, -3), -Vector3i(1, -2, 3));
}

TEST(VectorTest, absTest) {
    ASSERT_EQ(Vector3i(1, 2, 3), Vector3i(-1, -2, -3).abs());
    ASSERT_EQ(Vector3i(1, 2, 3), Vector3i(1, 2, 3).abs());
}

TEST(VectorTest, dotTest) {
    ASSERT_EQ(0, Vector3i(0).dot(Vector3i(1)));
    ASSERT_EQ(3, Vector3i(1).dot(Vector3i(1)));
    ASSERT_EQ(14, Vector3i(1, 2, 3).dot(Vector3i(1, 2, 3)));
    ASSERT_EQ(10, Vector3i(1, 2, 3).dot(Vector3i(3, 2, 1)));
    ASSERT_EQ(0, Vector3i(1).dot(Vector3i(-2, 1, 1)));
    ASSERT_EQ(-3, Vector3i(-1).dot(Vector3i(1)));

    ASSERT_EQ(Vector3i(1, 2, 3).dot(Vector3i(3, 2, 1)), Vector3i(3, 2, 1).dot(Vector3i(1, 2, 3)));
}

TEST(VectorTest, setXTest) {
    Vector3i v(0);

    v.setX(1);
    ASSERT_EQ(1, v.x());

    v.setX(-1);
    ASSERT_EQ(-1, v.x());

    v.setX(0);
    ASSERT_EQ(0, v.x());

    v.setX(std::numeric_limits<coord_t>::max());
    ASSERT_EQ(std::numeric_limits<coord_t>::max(), v.x());

    v.setX(std::numeric_limits<coord_t>::min());
    ASSERT_EQ(std::numeric_limits<coord_t>::min(), v.x());
}

TEST(VectorTest, setYTest) {
    Vector3i v(0);

    v.setY(1);
    ASSERT_EQ(1, v.y());

    v.setY(-1);
    ASSERT_EQ(-1, v.y());

    v.setY(0);
    ASSERT_EQ(0, v.y());

    v.setY(std::numeric_limits<coord_t>::max());
    ASSERT_EQ(std::numeric_limits<coord_t>::max(), v.y());

    v.setY(std::numeric_limits<coord_t>::min());
    ASSERT_EQ(std::numeric_limits<coord_t>::min(), v.y());
}

TEST(VectorTest, setZTest) {
    Vector3i v(0);

    v.setZ(1);
    ASSERT_EQ(1, v.z());

    v.setZ(-1);
    ASSERT_EQ(-1, v.z());

    v.setZ(0);
    ASSERT_EQ(0, v.z());

    v.setZ(std::numeric_limits<coord_t>::max());
    ASSERT_EQ(std::numeric_limits<coord_t>::max(), v.z());

    v.setZ(std::numeric_limits<coord_t>::min());
    ASSERT_EQ(std::numeric_limits<coord_t>::min(), v.z());
}

TEST(VectorTest, vectorAdditionTest) {
    ASSERT_EQ(Vector3i(0), Vector3i(0) + Vector3i(0));
    ASSERT_EQ(Vector3i(1, 2, 3), Vector3i(0) + Vector3i(1, 2, 3));
    ASSERT_EQ(Vector3i(5, 7, 9), Vector3i(1, 2, 3) + Vector3i(4, 5, 6));
    ASSERT_EQ(Vector3i(3), Vector3i(-1, -2, -3) + Vector3i(4, 5, 6));
    ASSERT_EQ(Vector3i(-5, -7, -9), Vector3i(-1, -2, -3) + Vector3i(-4, -5, -6));
}

TEST(VectorTest, vectorSubstractionTest) {
    ASSERT_EQ(Vector3i(0), Vector3i(0) - Vector3i(0));
    ASSERT_EQ(Vector3i(-1, -2, -3), Vector3i(0) - Vector3i(1, 2, 3));
    ASSERT_EQ(Vector3i(-3), Vector3i(1, 2, 3) - Vector3i(4, 5, 6));
    ASSERT_EQ(Vector3i(-5, -7, -9), Vector3i(-1, -2, -3) - Vector3i(4, 5, 6));
    ASSERT_EQ(Vector3i(3), Vector3i(-1, -2, -3) - Vector3i(-4, -5, -6));
}

TEST(VectorTest, vectorPostScalarMultiplicationTest) {
    ASSERT_EQ(Vector3i(0), Vector3i(0) * 2);
    ASSERT_EQ(Vector3i(2), Vector3i(1) * 2);
    ASSERT_EQ(Vector3i(-2), Vector3i(1) * -2);
    ASSERT_EQ(Vector3i(2, 4, 6), Vector3i(1, 2, 3) * 2);
    ASSERT_EQ(Vector3i(3, 6, 9), Vector3i(1, 2, 3) * 3);
}

TEST(VectorTest, vectorPreScalarMultiplicationTest) {
    ASSERT_EQ(Vector3i(0), 2 * Vector3i(0));
    ASSERT_EQ(Vector3i(2), 2 * Vector3i(1));
    ASSERT_EQ(Vector3i(-2), -2 * Vector3i(1));
    ASSERT_EQ(Vector3i(2, 4, 6), 2 * Vector3i(1, 2, 3));
    ASSERT_EQ(Vector3i(3, 6, 9), 3 * Vector3i(1, 2, 3));
}

TEST(VectorTest, vectorMinTest) {
    ASSERT_EQ(Vector3i(0), min(Vector3i(0), Vector3i(1)));
    ASSERT_EQ(Vector3i(0), min(Vector3i(1, 0, 0), Vector3i(0, 1, 1)));
    ASSERT_EQ(Vector3i(0), min(Vector3i(0, 1, 0), Vector3i(1, 0, 1)));
    ASSERT_EQ(Vector3i(0), min(Vector3i(0, 0, 1), Vector3i(1, 1, 0)));

    ASSERT_EQ(Vector3i(-1), min(Vector3i(0, 0, 0), Vector3i(-1)));
    ASSERT_EQ(Vector3i(1, 3, -10), min(Vector3i(1, 4, 9), Vector3i(2, 3, -10)));
}

TEST(VectorTest, vectorMaxTest) {
    ASSERT_EQ(Vector3i(1), max(Vector3i(0), Vector3i(1)));
    ASSERT_EQ(Vector3i(1), max(Vector3i(1, 0, 0), Vector3i(0, 1, 1)));
    ASSERT_EQ(Vector3i(1), max(Vector3i(0, 1, 0), Vector3i(1, 0, 1)));
    ASSERT_EQ(Vector3i(1), max(Vector3i(0, 0, 1), Vector3i(1, 1, 0)));

    ASSERT_EQ(Vector3i(0), max(Vector3i(0, 0, 0), Vector3i(-1)));
    ASSERT_EQ(Vector3i(-1), max(Vector3i(-2), Vector3i(-1)));
    ASSERT_EQ(Vector3i(2, 4, 9), max(Vector3i(1, 4, 9), Vector3i(2, 3, -10)));
}

