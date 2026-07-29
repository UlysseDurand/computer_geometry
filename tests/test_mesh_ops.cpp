#include "mesh_ops.h"
#include "gtest/gtest.h"

TEST(TriangleMesh, triangleSplit)
{
    TriangleMesh test;
    test.loadOFF("data/test2.off");
    triangleSplit(test, 1, Point(1, 0.5, 0));
    EXPECT_TRUE(test.checkValidity());
    test.writeOFF("out/trianglesplit.off");
}

TEST(TriangleMesh, edgeSplit)
{
    TriangleMesh test;
    test.loadOFF("data/test2.off");
    edgeSplit(test, Edge(0,1), Point(0.5, 0, 0));
    EXPECT_TRUE(test.checkValidity());
    test.writeOFF("out/edgesplit.off");
}

TEST(TriangleMesh, edgeFlip)
{
    TriangleMesh test;
    test.loadOFF("data/test2.off");
    edgeFlip(test, Edge(0,1));
    EXPECT_TRUE(test.checkValidity());
    test.writeOFF("out/edgesplit.off");
}