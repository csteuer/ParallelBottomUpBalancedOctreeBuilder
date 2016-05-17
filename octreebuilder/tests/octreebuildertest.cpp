#include <gmock/gmock.h>

#include <octree.h>
#include <octreebuilder.h>
#include <sequentialoctreebuilder.h>
#include <paralleloctreebuilder.h>

#include <vector_utils.h>

#include <memory>

MATCHER_P4(OctreeNodeHasProperties, llf, level, size, morton,
           std::string(negation ? "hasn't" : "has")
           + " properties { llf: " + ::testing::PrintToString(llf)
           + ", level: " + ::testing::PrintToString(level)
           + ", size: " + ::testing::PrintToString(size)
           + std::string(morton != static_cast<morton_t>(-1) ? ", morton_llf: " + ::testing::PrintToString(morton) : "")
           + " }") {

    if (!arg.isValid()) {
        *result_listener << "the node is invalid";
        return false;
    }

    if (arg.getLLF() != llf) {
        *result_listener << "has wrong llf";
        return false;
    }

    if (arg.getLevel() != static_cast<uint>(level)) {
        *result_listener << "has wrong level";
        return false;
    }

    if (arg.getSize() != size) {
        *result_listener << "has wrong size";
        return false;
    }

    if (morton != static_cast<morton_t>(-1)) {
        if (arg.getMortonEncodedLLF() != morton) {
            *result_listener << "has wrong morton encoded llf";
            return false;
        }
    }

    return true;
}

#define NodeMatches(llf, level, size) \
    OctreeNodeHasProperties(llf, static_cast<uint>(level), static_cast<coord_t>(size), static_cast<morton_t>(-1))
#define NodeMatches2(llf, level, size, morton) \
    OctreeNodeHasProperties(llf, static_cast<uint>(level), static_cast<coord_t>(size), static_cast<morton_t>(morton))

static bool sortByYXZ(const OctreeNode& ln, const OctreeNode& rn) {
    Vector3i l = ln.getLLF();
    Vector3i r = rn.getLLF();
    if (l.y() == r.y()) {
        if (l.x() == r.x()) {
            return l.z() < r.z();
        }
        return l.x() < r.x();
    }
    return l.y() < r.y();
}

using testing::Types;

template<class T>
class OctreeBuilderTest : public ::testing::Test {
public:
    OctreeBuilder& createInstance(const Vector3i& maxXYZ, uint maxLevel = std::numeric_limits<uint>::max()) {
        OctreeBuilder* newInstance = new T(maxXYZ, 0, maxLevel);
        m_instances.push_back(std::unique_ptr<OctreeBuilder>(newInstance));
        return *newInstance;
    }

protected:
    std::vector<std::unique_ptr<OctreeBuilder>> m_instances;

    virtual void TearDown() {
        m_instances.clear();
    }
};

typedef Types<SequentialOctreeBuilder, ParallelOctreeBuilder> Implementations;

TYPED_TEST_CASE(OctreeBuilderTest, Implementations);

TYPED_TEST(OctreeBuilderTest, octreeUniformRefinmentTest) {
    OctreeBuilder& builder = this->createInstance(Vector3i(1, 1, 1));
    morton_t mcode = builder.addLevelZeroLeaf(Vector3i(0, 0, 0));

    std::unique_ptr<Octree> tree(builder.finishBuilding());

    ASSERT_THAT(tree, ::testing::NotNull());

    EXPECT_THAT(tree->getMaxXYZ(), ::testing::Eq(Vector3i(1, 1, 1)));
    EXPECT_THAT(tree->getDepth(), ::testing::Eq(1));

    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < tree->getNumNodes(); i++) {
        nodes.push_back(tree->getNode(i));
    }

    for (const Vector3i v : VectorSpace(Vector3i(2))) {
        if (v == Vector3i(0)) {
            ASSERT_THAT(nodes, ::testing::Contains(NodeMatches2(v, 0, 1, mcode)));
        } else {
            ASSERT_THAT(nodes, ::testing::Contains(NodeMatches(v, 0, 1)));
        }
    }
}

TYPED_TEST(OctreeBuilderTest, addLeafTwiceTest) {
    OctreeBuilder& builder = this->createInstance(Vector3i(1, 1, 1));
    builder.addLevelZeroLeaf(Vector3i(0, 0, 0));

    ASSERT_NO_THROW(builder.addLevelZeroLeaf(Vector3i(0, 0, 0)));

    ASSERT_NO_THROW(builder.addLevelZeroLeaf(Vector3i(0, 0, 0)));

    std::unique_ptr<Octree> tree(builder.finishBuilding());

    ASSERT_EQ(8, tree->getNumNodes());

}

TYPED_TEST(OctreeBuilderTest, octreeTwoLevelRefinmentTest) {
    OctreeBuilder& builder = this->createInstance(Vector3i(3, 3, 3));
    mode_t mcode = builder.addLevelZeroLeaf(Vector3i(0, 0, 0));

    std::unique_ptr<Octree> tree(builder.finishBuilding());

    ASSERT_THAT(tree, ::testing::NotNull());

    EXPECT_THAT(tree->getMaxXYZ(), ::testing::Eq(Vector3i(3, 3, 3)));
    EXPECT_THAT(tree->getDepth(), ::testing::Eq(2));

    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < tree->getNumNodes(); i++) {
        nodes.push_back(tree->getNode(i));
    }

    /* Expected (2D cut):
       __ __ __ __
      |     |     |
      |__ __|__ __|
      |__|__|     |
      |X̲X̲|__|__ __|


      Where bottom left is (0, 0, 0). Empty cells are of type 0 and cells filled with 'X' are original level zero nodes
    */
    for (const Vector3i v : VectorSpace(Vector3i(2))) {
        if (v == Vector3i(0)) {
            ASSERT_THAT(nodes, ::testing::Contains(NodeMatches2(v, 0, 1, mcode)));
        } else {
            ASSERT_THAT(nodes, ::testing::Contains(NodeMatches(v, 0, 1)));
            ASSERT_THAT(nodes, ::testing::Contains(NodeMatches(v * 2, 1, 2)));
        }
    }
}

TYPED_TEST(OctreeBuilderTest, octreeFor2x2x2CubeTest) {
    OctreeBuilder& builder = this->createInstance(Vector3i(7, 7, 7));

    // Add cube leafs
    morton_t mcode_1 = builder.addLevelZeroLeaf(Vector3i(3, 3, 3));
    morton_t mcode_2 = builder.addLevelZeroLeaf(Vector3i(3, 3, 4));
    morton_t mcode_3 = builder.addLevelZeroLeaf(Vector3i(4, 3, 3));
    morton_t mcode_4 = builder.addLevelZeroLeaf(Vector3i(4, 3, 4));
    morton_t mcode_5 = builder.addLevelZeroLeaf(Vector3i(3, 4, 3));
    morton_t mcode_6 = builder.addLevelZeroLeaf(Vector3i(3, 4, 4));
    morton_t mcode_7 = builder.addLevelZeroLeaf(Vector3i(4, 4, 3));
    morton_t mcode_8 = builder.addLevelZeroLeaf(Vector3i(4, 4, 4));

    std::unique_ptr<Octree> tree(builder.finishBuilding());

    ASSERT_THAT(tree, ::testing::NotNull());

    EXPECT_THAT(tree->getMaxXYZ(), ::testing::Eq(Vector3i(7, 7, 7)));
    EXPECT_THAT(tree->getDepth(), ::testing::Eq(3));

    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < tree->getNumNodes(); i++) {
        nodes.push_back(tree->getNode(i));
    }

    /* Expected (2D cut):
       __ __ __ __ __ __ __ __
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |__|__|__|__|     |
      |__ __|__|XX|XX|__|__ __|
      |     |__|X̲X̲|X̲X̲|__|     |
      |__ __|__|__|__|__|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|

      Where bottom left is (0, 0, 0). Empty cells are of type 0 and cells filled with 'X' are original level zero nodes
    */
    EXPECT_THAT(nodes, ::testing::WhenSortedBy(&sortByYXZ, ::testing::ElementsAreArray({

                    // y: 0 - 1
                    //  x: 0 - 1
                    NodeMatches(Vector3i(0, 0, 0), 1, 2),
                    NodeMatches(Vector3i(0, 0, 2), 1, 2),
                    NodeMatches(Vector3i(0, 0, 4), 1, 2),
                    NodeMatches(Vector3i(0, 0, 6), 1, 2),

                    //  x: 2 - 3
                    NodeMatches(Vector3i(2, 0, 0), 1, 2),
                    NodeMatches(Vector3i(2, 0, 2), 1, 2),
                    NodeMatches(Vector3i(2, 0, 4), 1, 2),
                    NodeMatches(Vector3i(2, 0, 6), 1, 2),

                    //  x: 4 - 5
                    NodeMatches(Vector3i(4, 0, 0), 1, 2),
                    NodeMatches(Vector3i(4, 0, 2), 1, 2),
                    NodeMatches(Vector3i(4, 0, 4), 1, 2),
                    NodeMatches(Vector3i(4, 0, 6), 1, 2),

                    //  x: 6 - 7
                    NodeMatches(Vector3i(6, 0, 0), 1, 2),
                    NodeMatches(Vector3i(6, 0, 2), 1, 2),
                    NodeMatches(Vector3i(6, 0, 4), 1, 2),
                    NodeMatches(Vector3i(6, 0, 6), 1, 2),

                    // y: 2
                    NodeMatches(Vector3i(0, 2, 0), 1, 2),
                    NodeMatches(Vector3i(0, 2, 2), 1, 2),
                    NodeMatches(Vector3i(0, 2, 4), 1, 2),
                    NodeMatches(Vector3i(0, 2, 6), 1, 2),

                    NodeMatches(Vector3i(2, 2, 0), 1, 2),
                    NodeMatches(Vector3i(2, 2, 2), 0, 1),
                    NodeMatches(Vector3i(2, 2, 3), 0, 1),
                    NodeMatches(Vector3i(2, 2, 4), 0, 1),
                    NodeMatches(Vector3i(2, 2, 5), 0, 1),
                    NodeMatches(Vector3i(2, 2, 6), 1, 2),

                    NodeMatches(Vector3i(3, 2, 2), 0, 1),
                    NodeMatches(Vector3i(3, 2, 3), 0, 1),
                    NodeMatches(Vector3i(3, 2, 4), 0, 1),
                    NodeMatches(Vector3i(3, 2, 5), 0, 1),

                    NodeMatches(Vector3i(4, 2, 0), 1, 2),
                    NodeMatches(Vector3i(4, 2, 2), 0, 1),
                    NodeMatches(Vector3i(4, 2, 3), 0, 1),
                    NodeMatches(Vector3i(4, 2, 4), 0, 1),
                    NodeMatches(Vector3i(4, 2, 5), 0, 1),
                    NodeMatches(Vector3i(4, 2, 6), 1, 2),

                    NodeMatches(Vector3i(5, 2, 2), 0, 1),
                    NodeMatches(Vector3i(5, 2, 3), 0, 1),
                    NodeMatches(Vector3i(5, 2, 4), 0, 1),
                    NodeMatches(Vector3i(5, 2, 5), 0, 1),

                    NodeMatches(Vector3i(6, 2, 0), 1, 2),
                    NodeMatches(Vector3i(6, 2, 2), 1, 2),
                    NodeMatches(Vector3i(6, 2, 4), 1, 2),
                    NodeMatches(Vector3i(6, 2, 6), 1, 2),

                    // y: 3
                    NodeMatches(Vector3i(2, 3, 2), 0, 1),
                    NodeMatches(Vector3i(2, 3, 3), 0, 1),
                    NodeMatches(Vector3i(2, 3, 4), 0, 1),
                    NodeMatches(Vector3i(2, 3, 5), 0, 1),

                    NodeMatches(Vector3i(3, 3, 2), 0, 1),
                    NodeMatches2(Vector3i(3, 3, 3), 0, 1, mcode_1),
                    NodeMatches2(Vector3i(3, 3, 4), 0, 1, mcode_2),
                    NodeMatches(Vector3i(3, 3, 5), 0, 1),

                    NodeMatches(Vector3i(4, 3, 2), 0, 1),
                    NodeMatches2(Vector3i(4, 3, 3), 0, 1, mcode_3),
                    NodeMatches2(Vector3i(4, 3, 4), 0, 1, mcode_4),
                    NodeMatches(Vector3i(4, 3, 5), 0, 1),

                    NodeMatches(Vector3i(5, 3, 2), 0, 1),
                    NodeMatches(Vector3i(5, 3, 3), 0, 1),
                    NodeMatches(Vector3i(5, 3, 4), 0, 1),
                    NodeMatches(Vector3i(5, 3, 5), 0, 1),


                    // y: 4
                    //  x: 0 - 6
                    NodeMatches(Vector3i(0, 4, 0), 1, 2),
                    NodeMatches(Vector3i(0, 4, 2), 1, 2),
                    NodeMatches(Vector3i(0, 4, 4), 1, 2),
                    NodeMatches(Vector3i(0, 4, 6), 1, 2),

                    NodeMatches(Vector3i(2, 4, 0), 1, 2),
                    NodeMatches(Vector3i(2, 4, 2), 0, 1),
                    NodeMatches(Vector3i(2, 4, 3), 0, 1),
                    NodeMatches(Vector3i(2, 4, 4), 0, 1),
                    NodeMatches(Vector3i(2, 4, 5), 0, 1),
                    NodeMatches(Vector3i(2, 4, 6), 1, 2),

                    NodeMatches(Vector3i(3, 4, 2), 0, 1),
                    NodeMatches2(Vector3i(3, 4, 3), 0, 1, mcode_5),
                    NodeMatches2(Vector3i(3, 4, 4), 0, 1, mcode_6),
                    NodeMatches(Vector3i(3, 4, 5), 0, 1),

                    NodeMatches(Vector3i(4, 4, 0), 1, 2),
                    NodeMatches(Vector3i(4, 4, 2), 0, 1),
                    NodeMatches2(Vector3i(4, 4, 3), 0, 1, mcode_7),
                    NodeMatches2(Vector3i(4, 4, 4), 0, 1, mcode_8),
                    NodeMatches(Vector3i(4, 4, 5), 0, 1),
                    NodeMatches(Vector3i(4, 4, 6), 1, 2),

                    NodeMatches(Vector3i(5, 4, 2), 0, 1),
                    NodeMatches(Vector3i(5, 4, 3), 0, 1),
                    NodeMatches(Vector3i(5, 4, 4), 0, 1),
                    NodeMatches(Vector3i(5, 4, 5), 0, 1),

                    NodeMatches(Vector3i(6, 4, 0), 1, 2),
                    NodeMatches(Vector3i(6, 4, 2), 1, 2),
                    NodeMatches(Vector3i(6, 4, 4), 1, 2),
                    NodeMatches(Vector3i(6, 4, 6), 1, 2),

                    // y: 5
                    NodeMatches(Vector3i(2, 5, 2), 0, 1),
                    NodeMatches(Vector3i(2, 5, 3), 0, 1),
                    NodeMatches(Vector3i(2, 5, 4), 0, 1),
                    NodeMatches(Vector3i(2, 5, 5), 0, 1),

                    NodeMatches(Vector3i(3, 5, 2), 0, 1),
                    NodeMatches(Vector3i(3, 5, 3), 0, 1),
                    NodeMatches(Vector3i(3, 5, 4), 0, 1),
                    NodeMatches(Vector3i(3, 5, 5), 0, 1),

                    NodeMatches(Vector3i(4, 5, 2), 0, 1),
                    NodeMatches(Vector3i(4, 5, 3), 0, 1),
                    NodeMatches(Vector3i(4, 5, 4), 0, 1),
                    NodeMatches(Vector3i(4, 5, 5), 0, 1),

                    NodeMatches(Vector3i(5, 5, 2), 0, 1),
                    NodeMatches(Vector3i(5, 5, 3), 0, 1),
                    NodeMatches(Vector3i(5, 5, 4), 0, 1),
                    NodeMatches(Vector3i(5, 5, 5), 0, 1),

                    // y: 6
                    //  x: 0 - 1
                    NodeMatches(Vector3i(0, 6, 0), 1, 2),
                    NodeMatches(Vector3i(0, 6, 2), 1, 2),
                    NodeMatches(Vector3i(0, 6, 4), 1, 2),
                    NodeMatches(Vector3i(0, 6, 6), 1, 2),

                    //  x: 2 - 3
                    NodeMatches(Vector3i(2, 6, 0), 1, 2),
                    NodeMatches(Vector3i(2, 6, 2), 1, 2),
                    NodeMatches(Vector3i(2, 6, 4), 1, 2),
                    NodeMatches(Vector3i(2, 6, 6), 1, 2),

                    //  x: 4 - 5
                    NodeMatches(Vector3i(4, 6, 0), 1, 2),
                    NodeMatches(Vector3i(4, 6, 2), 1, 2),
                    NodeMatches(Vector3i(4, 6, 4), 1, 2),
                    NodeMatches(Vector3i(4, 6, 6), 1, 2),

                    //  x: 6 - 7
                    NodeMatches(Vector3i(6, 6, 0), 1, 2),
                    NodeMatches(Vector3i(6, 6, 2), 1, 2),
                    NodeMatches(Vector3i(6, 6, 4), 1, 2),
                    NodeMatches(Vector3i(6, 6, 6), 1, 2)})));
}

TYPED_TEST(OctreeBuilderTest, unbalancedOctreeThreeLevelRefinmentDiagonalLevelDifferenceTest) {

    OctreeBuilder& builder = this->createInstance(Vector3i(7, 7, 7));

    // Add cube leafs
    morton_t mcode = builder.addLevelZeroLeaf(Vector3i(5, 2, 0));

    std::unique_ptr<Octree> tree(builder.finishBuilding(false));

    ASSERT_THAT(tree, ::testing::NotNull());

    EXPECT_THAT(tree->getMaxXYZ(), ::testing::Eq(Vector3i(7, 7, 7)));
    EXPECT_THAT(tree->getDepth(), ::testing::Eq(3));

    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < tree->getNumNodes(); i++) {
        nodes.push_back(tree->getNode(i));
    }

    /* Expected (2D cut):
      x-y-cut:
       __ __ __ __ __ __ __ __
      |           |           |
      |           |           |
      |           |           |
      |__ __ __ __|__ __ __ __|
      |           |__|__|     |
      |           |__|X̲X̲|__ __|
      |           |     |     |
      |__ __ __ __|__ __|__ __|

      x-z-cut:
      x-y-cut:
       __ __ __ __ __ __ __ __
      |           |           |
      |           |           |
      |           |           |
      |__ __ __ __|__ __ __ __|
      |           |     |     |
      |           |__ __|__ __|
      |           |__|__|     |
      |__ __ __ __|__|X̲X̲|__ __|

      Where bottom left is (0, 0, 0). Empty cells are of type 0 and cells filled with 'X' are original level zero nodes
    */

    EXPECT_THAT(nodes, ::testing::WhenSortedBy(&sortByYXZ, ::testing::ElementsAreArray({

                    // y: 0
                    //  x: 0
                    NodeMatches(Vector3i(0, 0, 0), 2, 4),
                    NodeMatches(Vector3i(0, 0, 4), 2, 4),

                    //  x: 4
                    NodeMatches(Vector3i(4, 0, 0), 1, 2),
                    NodeMatches(Vector3i(4, 0, 2), 1, 2),
                    NodeMatches(Vector3i(4, 0, 4), 2, 4),

                    //  x: 6
                    NodeMatches(Vector3i(6, 0, 0), 1, 2),
                    NodeMatches(Vector3i(6, 0, 2), 1, 2),

                    // y: 2
                    //  x: 4
                    NodeMatches(Vector3i(4, 2, 0), 0, 1),
                    NodeMatches(Vector3i(4, 2, 1), 0, 1),
                    NodeMatches(Vector3i(4, 2, 2), 1, 2),

                    //  x: 5
                    NodeMatches2(Vector3i(5, 2, 0), 0, 1, mcode),
                    NodeMatches(Vector3i(5, 2, 1), 0, 1),

                    //  x: 6
                    NodeMatches(Vector3i(6, 2, 0), 1, 2),
                    NodeMatches(Vector3i(6, 2, 2), 1, 2),

                    // y: 3
                    //  x: 4
                    NodeMatches(Vector3i(4, 3, 0), 0, 1),
                    NodeMatches(Vector3i(4, 3, 1), 0, 1),

                    //  x: 5
                    NodeMatches(Vector3i(5, 3, 0), 0, 1),
                    NodeMatches(Vector3i(5, 3, 1), 0, 1),

                    // y: 4
                    //  x: 0
                    NodeMatches(Vector3i(0, 4, 0), 2, 4),
                    NodeMatches(Vector3i(0, 4, 4), 2, 4),

                    //  x: 4
                    NodeMatches(Vector3i(4, 4, 0), 2, 4),
                    NodeMatches(Vector3i(4, 4, 4), 2, 4)})));
}

TYPED_TEST(OctreeBuilderTest, balancedOctreeThreeLevelRefinmentDiagonalLevelDifferenceTest) {

    OctreeBuilder& builder = this->createInstance(Vector3i(7, 7, 7));

    // Add cube leafs
    morton_t mcode = builder.addLevelZeroLeaf(Vector3i(5, 2, 0));

    std::unique_ptr<Octree> tree(builder.finishBuilding(true));

    ASSERT_THAT(tree, ::testing::NotNull());

    EXPECT_THAT(tree->getMaxXYZ(), ::testing::Eq(Vector3i(7, 7, 7)));
    EXPECT_THAT(tree->getDepth(), ::testing::Eq(3));

    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < tree->getNumNodes(); i++) {
        nodes.push_back(tree->getNode(i));
    }

    /* Expected (2D cut):
      x-y-cut:
       __ __ __ __ __ __ __ __
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |__|__|     |
      |__ __|__ __|__|X̲X̲|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|

      x-z-cut:
      x-y-cut:
       __ __ __ __ __ __ __ __
      |           |           |
      |           |           |
      |           |           |
      |__ __ __ __|__ __ __ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |__|__|     |
      |__ __|__ __|__|X̲X̲|__ __|

      Where bottom left is (0, 0, 0). Empty cells are of type 0 and cells filled with 'X' are original level zero nodes
    */

    EXPECT_THAT(nodes, ::testing::WhenSortedBy(&sortByYXZ, ::testing::ElementsAreArray({

                    // y: 0
                    //  x: 0
                    NodeMatches(Vector3i(0, 0, 0), 1, 2),
                    NodeMatches(Vector3i(0, 0, 2), 1, 2),
                    NodeMatches(Vector3i(0, 0, 4), 2, 4),

                    //  x: 2
                    NodeMatches(Vector3i(2, 0, 0), 1, 2),
                    NodeMatches(Vector3i(2, 0, 2), 1, 2),

                    //  x: 4
                    NodeMatches(Vector3i(4, 0, 0), 1, 2),
                    NodeMatches(Vector3i(4, 0, 2), 1, 2),
                    NodeMatches(Vector3i(4, 0, 4), 2, 4),

                    //  x: 6
                    NodeMatches(Vector3i(6, 0, 0), 1, 2),
                    NodeMatches(Vector3i(6, 0, 2), 1, 2),

                    // y: 2
                    //  x: 0
                    NodeMatches(Vector3i(0, 2, 0), 1, 2),
                    NodeMatches(Vector3i(0, 2, 2), 1, 2),

                    //  x: 2
                    NodeMatches(Vector3i(2, 2, 0), 1, 2),
                    NodeMatches(Vector3i(2, 2, 2), 1, 2),

                    //  x: 4
                    NodeMatches(Vector3i(4, 2, 0), 0, 1),
                    NodeMatches(Vector3i(4, 2, 1), 0, 1),
                    NodeMatches(Vector3i(4, 2, 2), 1, 2),

                    //  x: 5
                    NodeMatches2(Vector3i(5, 2, 0), 0, 1, mcode),
                    NodeMatches(Vector3i(5, 2, 1), 0, 1),

                    //  x: 6
                    NodeMatches(Vector3i(6, 2, 0), 1, 2),
                    NodeMatches(Vector3i(6, 2, 2), 1, 2),

                    // y: 3
                    //  x: 4
                    NodeMatches(Vector3i(4, 3, 0), 0, 1),
                    NodeMatches(Vector3i(4, 3, 1), 0, 1),

                    //  x: 5
                    NodeMatches(Vector3i(5, 3, 0), 0, 1),
                    NodeMatches(Vector3i(5, 3, 1), 0, 1),

                    // y: 4
                    //  x: 0
                    NodeMatches(Vector3i(0, 4, 0), 1, 2),
                    NodeMatches(Vector3i(0, 4, 2), 1, 2),
                    NodeMatches(Vector3i(0, 4, 4), 2, 4),

                    //  x: 2
                    NodeMatches(Vector3i(2, 4, 0), 1, 2),
                    NodeMatches(Vector3i(2, 4, 2), 1, 2),

                    //  x: 4
                    NodeMatches(Vector3i(4, 4, 0), 1, 2),
                    NodeMatches(Vector3i(4, 4, 2), 1, 2),
                    NodeMatches(Vector3i(4, 4, 4), 2, 4),

                    //  x: 6
                    NodeMatches(Vector3i(6, 4, 0), 1, 2),
                    NodeMatches(Vector3i(6, 4, 2), 1, 2),

                    // y: 6
                    //  x: 0
                    NodeMatches(Vector3i(0, 6, 0), 1, 2),
                    NodeMatches(Vector3i(0, 6, 2), 1, 2),

                    //  x: 2
                    NodeMatches(Vector3i(2, 6, 0), 1, 2),
                    NodeMatches(Vector3i(2, 6, 2), 1, 2),

                    //  x: 4
                    NodeMatches(Vector3i(4, 6, 0), 1, 2),
                    NodeMatches(Vector3i(4, 6, 2), 1, 2),

                    //  x: 6
                    NodeMatches(Vector3i(6, 6, 0), 1, 2),
                    NodeMatches(Vector3i(6, 6, 2), 1, 2)})));
}

TYPED_TEST(OctreeBuilderTest, maxLevelTest) {
    OctreeBuilder& builder = this->createInstance(Vector3i(7, 7, 7), 1);

    // Add cube leafs
    builder.addLevelZeroLeaf(Vector3i(5, 2, 0));

    std::unique_ptr<Octree> tree(builder.finishBuilding());

    ASSERT_THAT(tree, ::testing::NotNull());

    EXPECT_THAT(tree->getMaxXYZ(), ::testing::Eq(Vector3i(7, 7, 7)));
    EXPECT_THAT(tree->getDepth(), ::testing::Eq(3));

    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < tree->getNumNodes(); i++) {
        nodes.push_back(tree->getNode(i));
    }

    /* Expected (2D cut):
      x-y-cut:
       __ __ __ __ __ __ __ __
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |__|__|     |
      |__ __|__ __|__|X̲X̲|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|

      x-z-cut:
      x-y-cut:
       __ __ __ __ __ __ __ __
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |     |     |
      |__ __|__ __|__ __|__ __|
      |     |     |__|__|     |
      |__ __|__ __|__|X̲X̲|__ __|

      Where bottom left is (0, 0, 0). Empty cells are of type 0 and cells filled with 'X' are original level zero nodes
    */

    EXPECT_THAT(nodes, ::testing::Not(::testing::Contains(::testing::AllOf(
                                                              ::testing::Property(&OctreeNode::getLevel, 1),
                                                              ::testing::Property(&OctreeNode::getLLF, Vector3i(4, 2, 0))))));

    EXPECT_THAT(nodes, ::testing::AllOf(::testing::SizeIs(71), ::testing::Each(::testing::Property(&OctreeNode::getLevel, ::testing::Le(1)))));
}
