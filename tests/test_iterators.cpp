#include "mesh.h"
#include "gtest/gtest.h"

typedef TriangleMesh Mesh;

TEST(TriangleMesh, Iterators)
{
    testing::internal::CaptureStdout();

    Mesh titi;
    titi.loadOFF("data/cube.off");
    Iterator_on_vertices its;
    Circulator_on_faces cf;
    for (its = titi.vertices_begin();
         its != titi.vertices_past_the_end();
         ++its)
    {
        Circulator_on_faces cfbegin = titi.incident_faces(*its);
        int cmpt = 0;
        for (cf = cfbegin, ++cf;
             cf != cfbegin;
             cf++)
            cmpt++;
        std ::cout << "valence of the vertex "
                   << cmpt << std ::endl;
    }

    std::string output = testing::internal::GetCapturedStdout();
    EXPECT_EQ(output, "valence of the vertex 3\n"
                      "valence of the vertex 4\n"
                      "valence of the vertex 3\n"
                      "valence of the vertex 4\n"
                      "valence of the vertex 4\n"
                      "valence of the vertex 3\n"
                      "valence of the vertex 4\n"
                      "valence of the vertex 3\n");
}