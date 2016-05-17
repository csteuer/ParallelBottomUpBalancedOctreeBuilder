#include <gmock/gmock.h>

#include <octree_impl.h>
#include <vector_utils.h>
#include <mortoncode.h>

class OctreeTest : public ::testing::Test {
protected:
    virtual void SetUp() override
    {
        /* Should create the following 4x4x4 octree (2D cut):
           __ __ __ __
          |     |     |
          |__ __|__ __|
          |__|__|     |
          |X̲X̲|__|__ __|
        */

        std::vector<std::unordered_set<morton_t>> treeOne(3);
        for (Vector3i c : VectorSpace(Vector3i(2))) {
            treeOne.at(0).insert(getMortonCodeForCoordinate(c));

            if (c != Vector3i(0)) {
                treeOne.at(1).insert(getMortonCodeForCoordinate(c * 2));
            }
        }

        octree4x4x4 = std::unique_ptr<Octree>(new OctreeImpl(std::move(treeOne)));

        /* Should create the following 4x4x4 octree (2D cut):
           __ __ __ __
          |     |__|X̲X̲|
          |__ __|__|__|
          |     |     |
          |__ __|__ __|
        */

        std::vector<std::unordered_set<morton_t>> treeTwo(3);
        for (Vector3i c : VectorSpace(Vector3i(2))) {
            treeTwo.at(0).insert(getMortonCodeForCoordinate(Vector3i(2) + c));

            if (c != Vector3i(1)) {
                treeTwo.at(1).insert(getMortonCodeForCoordinate(c * 2));
            }
        }

        octree4x4x4_inverse = std::unique_ptr<Octree>(new OctreeImpl(std::move(treeTwo)));
    }

    std::unique_ptr<Octree> octree4x4x4;
    std::unique_ptr<Octree> octree4x4x4_inverse;
};

TEST_F(OctreeTest, getMaxXYZOctreeTest) {
    EXPECT_EQ(Vector3i(3, 3, 3), octree4x4x4->getMaxXYZ());
    EXPECT_EQ(Vector3i(3, 3, 3), octree4x4x4_inverse->getMaxXYZ());
}

TEST_F(OctreeTest, getDepthOctreeTest) {
    EXPECT_EQ(2, octree4x4x4->getDepth());
    EXPECT_EQ(2, octree4x4x4_inverse->getDepth());
}

TEST_F(OctreeTest, getMaxLevel) {
    EXPECT_EQ(1, octree4x4x4->getMaxLevel());
    EXPECT_EQ(1, octree4x4x4_inverse->getMaxLevel());
}

TEST_F(OctreeTest, getNumNodesOctreeTest) {
    EXPECT_EQ(15, octree4x4x4->getNumNodes());
    EXPECT_EQ(15, octree4x4x4_inverse->getNumNodes());
}

TEST_F(OctreeTest, nodeEqualityOctreeTest) {
    EXPECT_NE(OctreeNode(), OctreeNode()) << " an invalid node must not be equal to any other node.";
    EXPECT_EQ(octree4x4x4->getNode(0), octree4x4x4->getNode(0));
    EXPECT_EQ(octree4x4x4_inverse->getNode(0), octree4x4x4_inverse->getNode(0));
}

TEST_F(OctreeTest, tryGetNodeAtTest) {
    EXPECT_EQ(OctreeNode(0, 0), octree4x4x4->tryGetNodeAt(Vector3i(0), 0));
    EXPECT_EQ(OctreeNode(getMortonCodeForCoordinate(Vector3i(2)), 1), octree4x4x4->tryGetNodeAt(Vector3i(2), 1));

    EXPECT_FALSE(octree4x4x4->tryGetNodeAt(Vector3i(0), 1).isValid());
    EXPECT_FALSE(octree4x4x4->tryGetNodeAt(Vector3i(2), 0).isValid());
}

TEST_F(OctreeTest, nodeAtTest) {
    EXPECT_EQ(OctreeNode(0, 0), octree4x4x4->nodeAt(Vector3i(0, 0, 0)));
    EXPECT_EQ(OctreeNode(0, 0), octree4x4x4->nodeAt(Vector3i(0, 0, 0)));
}

static OctreeNode findNodeWithLLF(const std::vector<OctreeNode>& nodes, const Vector3i& llf) {
    auto it = std::find_if(nodes.begin(), nodes.end(), [&llf](const OctreeNode& n) {
        return n.getLLF() == llf;
    });

    if (it == nodes.end()) {
        throw std::runtime_error("Could not find node in octree. Probably the octree builder defect and produced an invalid octree.");
    }
    return *it;
}

TEST_F(OctreeTest, getNeighbourNodeOctree4x4x4Test) {

    // get all nodes
    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < octree4x4x4->getNumNodes(); i++) {
        nodes.push_back(octree4x4x4->getNode(i));
    }

    OctreeNode nodeAt_0_0_0 = findNodeWithLLF(nodes, Vector3i(0, 0, 0));
    OctreeNode nodeAt_0_0_1 = findNodeWithLLF(nodes, Vector3i(0, 0, 1));
    OctreeNode nodeAt_0_1_0 = findNodeWithLLF(nodes, Vector3i(0, 1, 0));
    OctreeNode nodeAt_0_1_1 = findNodeWithLLF(nodes, Vector3i(0, 1, 1));
    OctreeNode nodeAt_1_0_0 = findNodeWithLLF(nodes, Vector3i(1, 0, 0));
    OctreeNode nodeAt_1_0_1 = findNodeWithLLF(nodes, Vector3i(1, 0, 1));
    OctreeNode nodeAt_1_1_0 = findNodeWithLLF(nodes, Vector3i(1, 1, 0));
    OctreeNode nodeAt_1_1_1 = findNodeWithLLF(nodes, Vector3i(1, 1, 1));

    OctreeNode nodeAt_0_0_2 = findNodeWithLLF(nodes, Vector3i(0, 0, 2));
    OctreeNode nodeAt_0_2_0 = findNodeWithLLF(nodes, Vector3i(0, 2, 0));
    OctreeNode nodeAt_0_2_2 = findNodeWithLLF(nodes, Vector3i(0, 2, 2));
    OctreeNode nodeAt_2_0_0 = findNodeWithLLF(nodes, Vector3i(2, 0, 0));
    OctreeNode nodeAt_2_0_2 = findNodeWithLLF(nodes, Vector3i(2, 0, 2));
    OctreeNode nodeAt_2_2_0 = findNodeWithLLF(nodes, Vector3i(2, 2, 0));
    OctreeNode nodeAt_2_2_2 = findNodeWithLLF(nodes, Vector3i(2, 2, 2));

    // Test neighbours for node at (0,0,0)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::RIGHT), ::testing::ElementsAre(nodeAt_1_0_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::LEFT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::BACK), ::testing::ElementsAre(nodeAt_0_1_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::FRONT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::TOP), ::testing::ElementsAre(nodeAt_0_0_1));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::BOTTOM), ::testing::IsEmpty());


    // Test neighbours for node at (1,0,0)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_0_0, OctreeNode::Face::RIGHT), ::testing::ElementsAre(nodeAt_2_0_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_0_0, OctreeNode::Face::LEFT), ::testing::ElementsAre(nodeAt_0_0_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_0_0, OctreeNode::Face::BACK), ::testing::ElementsAre(nodeAt_1_1_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_0_0, OctreeNode::Face::FRONT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_0_0, OctreeNode::Face::TOP), ::testing::ElementsAre(nodeAt_1_0_1));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_0_0, OctreeNode::Face::BOTTOM), ::testing::IsEmpty());

    // Test neighbours for node at (1,1,1)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_1_1, OctreeNode::Face::RIGHT), ::testing::ElementsAre(nodeAt_2_0_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_1_1, OctreeNode::Face::LEFT), ::testing::ElementsAre(nodeAt_0_1_1));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_1_1, OctreeNode::Face::BACK), ::testing::ElementsAre(nodeAt_0_2_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_1_1, OctreeNode::Face::FRONT), ::testing::ElementsAre(nodeAt_1_0_1));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_1_1, OctreeNode::Face::TOP), ::testing::ElementsAre(nodeAt_0_0_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_1_1_1, OctreeNode::Face::BOTTOM), ::testing::ElementsAre(nodeAt_1_1_0));

    // Test neighbours for node at (2,0,0)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_0_0, OctreeNode::Face::RIGHT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_0_0, OctreeNode::Face::LEFT), ::testing::UnorderedElementsAre(nodeAt_1_0_0, nodeAt_1_1_0, nodeAt_1_0_1, nodeAt_1_1_1));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_0_0, OctreeNode::Face::BACK), ::testing::ElementsAre(nodeAt_2_2_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_0_0, OctreeNode::Face::FRONT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_0_0, OctreeNode::Face::TOP), ::testing::ElementsAre(nodeAt_2_0_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_0_0, OctreeNode::Face::BOTTOM), ::testing::IsEmpty());

    // Test neighbours for node at (0,2,0)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_2_0, OctreeNode::Face::RIGHT), ::testing::ElementsAre(nodeAt_2_2_0));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_2_0, OctreeNode::Face::LEFT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_2_0, OctreeNode::Face::BACK), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_2_0, OctreeNode::Face::FRONT), ::testing::UnorderedElementsAre(nodeAt_0_1_0, nodeAt_0_1_1, nodeAt_1_1_0, nodeAt_1_1_1));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_2_0, OctreeNode::Face::TOP), ::testing::ElementsAre(nodeAt_0_2_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_2_0, OctreeNode::Face::BOTTOM), ::testing::IsEmpty());

    // Test neighbours for node at (0,0,2)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_2, OctreeNode::Face::RIGHT), ::testing::ElementsAre(nodeAt_2_0_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_2, OctreeNode::Face::LEFT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_2, OctreeNode::Face::BACK), ::testing::ElementsAre(nodeAt_0_2_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_2, OctreeNode::Face::FRONT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_2, OctreeNode::Face::TOP), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_0_0_2, OctreeNode::Face::BOTTOM), ::testing::UnorderedElementsAre(nodeAt_0_0_1, nodeAt_0_1_1, nodeAt_1_0_1, nodeAt_1_1_1));

    // Test neighbours for node at (2,2,2)
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_2_2, OctreeNode::Face::RIGHT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_2_2, OctreeNode::Face::LEFT), ::testing::ElementsAre(nodeAt_0_2_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_2_2, OctreeNode::Face::BACK), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_2_2, OctreeNode::Face::FRONT), ::testing::ElementsAre(nodeAt_2_0_2));
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_2_2, OctreeNode::Face::TOP), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4->getNeighbourNodes(nodeAt_2_2_2, OctreeNode::Face::BOTTOM), ::testing::ElementsAre(nodeAt_2_2_0));
}

TEST_F(OctreeTest, tryGetNodeAtOctree4x4x4Test) {

    for (size_t i = 0; i < octree4x4x4->getNumNodes(); i++) {

        const OctreeNode n = octree4x4x4->getNode(i);

        EXPECT_THAT(octree4x4x4->tryGetNodeAt(n.getLLF(), n.getLevel()), ::testing::Eq(n));
    }

    EXPECT_THAT(octree4x4x4->tryGetNodeAt(Vector3i(2, 0, 0), 0), ::testing::Property(&OctreeNode::isValid, false));
    EXPECT_THAT(octree4x4x4->tryGetNodeAt(Vector3i(0, 0, 0), 1), ::testing::Property(&OctreeNode::isValid, false));
}

TEST_F(OctreeTest, getNeighbourNodeOctree4x4x4InverseTest) {

    // get all nodes
    std::vector<OctreeNode> nodes;
    for (size_t i = 0; i < octree4x4x4_inverse->getNumNodes(); i++) {
        nodes.push_back(octree4x4x4_inverse->getNode(i));
    }

    OctreeNode nodeAt_0_0_0 = findNodeWithLLF(nodes, Vector3i(0, 0, 0));
    OctreeNode nodeAt_0_0_2 = findNodeWithLLF(nodes, Vector3i(0, 0, 2));
    OctreeNode nodeAt_0_2_0 = findNodeWithLLF(nodes, Vector3i(0, 2, 0));
    OctreeNode nodeAt_0_2_2 = findNodeWithLLF(nodes, Vector3i(0, 2, 2));
    OctreeNode nodeAt_2_0_0 = findNodeWithLLF(nodes, Vector3i(2, 0, 0));
    OctreeNode nodeAt_2_0_2 = findNodeWithLLF(nodes, Vector3i(2, 0, 2));
    OctreeNode nodeAt_2_2_0 = findNodeWithLLF(nodes, Vector3i(2, 2, 0));

    OctreeNode nodeAt_2_2_2 = findNodeWithLLF(nodes, Vector3i(2, 2, 2));
    OctreeNode nodeAt_2_2_3 = findNodeWithLLF(nodes, Vector3i(2, 2, 3));
    OctreeNode nodeAt_2_3_2 = findNodeWithLLF(nodes, Vector3i(2, 3, 2));
    OctreeNode nodeAt_2_3_3 = findNodeWithLLF(nodes, Vector3i(2, 3, 3));
    OctreeNode nodeAt_3_2_2 = findNodeWithLLF(nodes, Vector3i(3, 2, 2));
    OctreeNode nodeAt_3_2_3 = findNodeWithLLF(nodes, Vector3i(3, 2, 3));
    OctreeNode nodeAt_3_3_2 = findNodeWithLLF(nodes, Vector3i(3, 3, 2));
    OctreeNode nodeAt_3_3_3 = findNodeWithLLF(nodes, Vector3i(3, 3, 3));

    // Test neighbours for node at (0,0,0)
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::RIGHT), ::testing::ElementsAre(nodeAt_2_0_0));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::LEFT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::BACK), ::testing::ElementsAre(nodeAt_0_2_0));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::FRONT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::TOP), ::testing::ElementsAre(nodeAt_0_0_2));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_0_0, OctreeNode::Face::BOTTOM), ::testing::IsEmpty());

    // Test neighbours for node at (3,3,3)
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_3_3_3, OctreeNode::Face::RIGHT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_3_3_3, OctreeNode::Face::LEFT), ::testing::ElementsAre(nodeAt_2_3_3));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_3_3_3, OctreeNode::Face::BACK), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_3_3_3, OctreeNode::Face::FRONT), ::testing::ElementsAre(nodeAt_3_2_3));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_3_3_3, OctreeNode:: Face::TOP), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_3_3_3, OctreeNode::Face::BOTTOM), ::testing::ElementsAre(nodeAt_3_3_2));

    // Test neighbours for node at (0,2,2)
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_2_2, OctreeNode::Face::RIGHT), ::testing::UnorderedElementsAre(nodeAt_2_2_2, nodeAt_2_2_3, nodeAt_2_3_2, nodeAt_2_3_3));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_2_2, OctreeNode::Face::LEFT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_2_2, OctreeNode::Face::BACK), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_2_2, OctreeNode::Face::FRONT), ::testing::ElementsAre(nodeAt_0_0_2));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_2_2, OctreeNode::Face::TOP), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_0_2_2, OctreeNode::Face::BOTTOM), ::testing::ElementsAre(nodeAt_0_2_0));

    // Test neighbours for node at (2,2,0)
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_2_0, OctreeNode::Face::RIGHT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_2_0, OctreeNode::Face::LEFT), ::testing::ElementsAre(nodeAt_0_2_0));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_2_0, OctreeNode::Face::BACK), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_2_0, OctreeNode::Face::FRONT), ::testing::ElementsAre(nodeAt_2_0_0));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_2_0, OctreeNode::Face::TOP), ::testing::UnorderedElementsAre(nodeAt_2_2_2, nodeAt_2_3_2, nodeAt_3_2_2, nodeAt_3_3_2));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_2_0, OctreeNode::Face::BOTTOM), ::testing::IsEmpty());

    // Test neighbours for node at (2,0,2)
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_0_2, OctreeNode::Face::RIGHT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_0_2, OctreeNode::Face::LEFT), ::testing::ElementsAre(nodeAt_0_0_2));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_0_2, OctreeNode::Face::BACK), ::testing::UnorderedElementsAre(nodeAt_2_2_2, nodeAt_2_2_3, nodeAt_3_2_2, nodeAt_3_2_3));
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_0_2, OctreeNode::Face::FRONT), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_0_2, OctreeNode::Face::TOP), ::testing::IsEmpty());
    EXPECT_THAT(octree4x4x4_inverse->getNeighbourNodes(nodeAt_2_0_2, OctreeNode::Face::BOTTOM), ::testing::ElementsAre(nodeAt_2_0_0));
}
