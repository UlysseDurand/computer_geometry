#include "basic_shapes.h"
#include "gtest/gtest.h"

TEST(TriangleMesh, TetrahedronMaking)
{
    TriangleMesh tetrahedron = make_tetrahedron(Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0), Point(0, 0, 1));
    EXPECT_TRUE(tetrahedron.checkValidity());
    tetrahedron.writeOFF("out/tetrahedron.off");
}

TEST(TriangleMesh, PyramidMaking)
{
    TriangleMesh pyramid = make_pyramid(Point(1, 1, 1), Point(0, 0, 0), Point(2, 0, 0), Point(2, 0, 2), Point(0, 0, 2));
    EXPECT_TRUE(pyramid.checkValidity());
    pyramid.writeOFF("out/pyramid.off");
}

TEST(TriangleMesh, LoadOFF)
{
    TriangleMesh queen;
    queen.loadOFF("data/queen.off");
    EXPECT_TRUE(queen.checkValidity());
    queen.writeOFF("out/queen_rebuilt.off");
}