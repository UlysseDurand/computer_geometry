#ifndef basic_shapes_h
#define basic_shapes_h

#include "mesh.h"

TriangleMesh make_tetrahedron(Point pA, Point pB, Point pC, Point pD)
{
    TriangleMesh res({pA, pB, pC, pD});
    int face0 = res.addNewFace({0, 1, 2});
    int face1 = res.addAdjFace(face0, 0, 3);
    int face2 = res.addAdjFace(face0, 1, 3);
    int face3 = res.addAdjFace(face0, 2, 3);

    res.sew(face1, 1, face2, 0);
    res.sew(face2, 2, face3, 1);
    res.sew(face1, 2, face3, 0);

    return res;
};



TriangleMesh make_pyramid(Point pO, Point pA, Point pB, Point pC, Point pD)
{
    TriangleMesh res = TriangleMesh({pO, pA, pB, pC, pD});
    int face0 = res.addNewFace({1, 3, 2});
    int face1 = res.addAdjFace(face0, 2, 4);
    int face2 = res.addAdjFace(face0, 1, 0);
    int face3 = res.addAdjFace(face0, 3, 0);
    int face4 = res.addAdjFace(face1, 1, 0);
    int face5 = res.addAdjFace(face1, 3, 0);
    res.sew(face2, 3, face3, 1);
    res.sew(face3, 2, face5, 4);
    res.sew(face5, 1, face4, 3);
    res.sew(face4, 4, face2, 2);
    return res;
}





#endif