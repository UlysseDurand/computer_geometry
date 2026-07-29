#include <vector>
#include <algorithm>
#include <stdexcept>
#include <array>
#include <iostream>
#include <fstream>
#include <map>
#include <cassert>
#include <cmath>
#include "geom.h"
#include "mesh.h"
#include "basic_shapes.h"
#include "mesh_ops.h"

#include <iostream>
#include <vector>
#include <queue>

#include "triangulation.h"

using std::cout;
using std::endl;
using std::vector;

void print_separator(const std::string& title)
{
    cout << "\n==============================\n";
    cout << title << endl;
    cout << "==============================\n";
}

void print_point(const Point& p, const std::string& name)
{
    cout << name << " = (" << p.x << ", " << p.y << ", " << p.z << ")" << endl;
}

void print_edge(const Edge& e, const std::string& name)
{
    cout << name << " = { id_f = " << e.id_f
         << ", local_id_v = " << e.local_id_v << " }" << endl;
}

void print_face(const TriangleFace& f, const std::string& name)
{
    cout << name << " vertices = ["
         << f.vertices[0] << ", "
         << f.vertices[1] << ", "
         << f.vertices[2] << "] adj = ["
         << f.adj_faces[0] << ", "
         << f.adj_faces[1] << ", "
         << f.adj_faces[2] << "]" << endl;
}

void print_mesh_info(const TriangleMesh& mesh, const std::string& name)
{
    cout << name << " :" << endl;
    cout << "  nb vertices = " << mesh.vertices.size() << endl;
    cout << "  nb faces    = " << mesh.faces.size() << endl;

    for (int i = 0; i < (int)mesh.vertices.size(); i++)
    {
        cout << "  vertex[" << i << "] = ("
             << mesh.vertices[i].x << ", "
             << mesh.vertices[i].y << ", "
             << mesh.vertices[i].z << ")" << endl;
    }

    for (int i = 0; i < (int)mesh.faces.size(); i++)
    {
        cout << "  face[" << i << "] vertices = ["
             << mesh.faces[i].vertices[0] << ", "
             << mesh.faces[i].vertices[1] << ", "
             << mesh.faces[i].vertices[2] << "] adj = ["
             << mesh.faces[i].adj_faces[0] << ", "
             << mesh.faces[i].adj_faces[1] << ", "
             << mesh.faces[i].adj_faces[2] << "]" << endl;
    }
}


int main()
{
    print_separator("CREATION DES POINTS");

    TriangleMesh test2=TriangleMesh(
        {
        Vertex(Point(0.0f, 0.0f, 0.0f),0),
        Vertex(Point(2.0f, 0.0f, 0.0f),0),
        Vertex(Point(0.5f, 3.0f, 0.0f),0),
        Vertex(Point(0.5f, -3.0f, 0.0f),1)
        }, {
        TriangleFace({0,1,2},{-1,-1,1}),
        TriangleFace({0,3,1},{-1,0,-1}),
        });

    TriangleMesh test3=TriangleMesh(
        {
        Vertex(Point(0.0f, 0.0f, 0.0f),0),
        Vertex(Point(2.0f, 0.0f, 0.0f),1),
        Vertex(Point(0.5f, 3.0f, 0.0f),0),
        Vertex(Point(0.5f, -3.0f, 0.0f),1)
        }, {
        TriangleFace({0,3,2},{1,-1,-1}),
        TriangleFace({1,2,3},{0,-1,-1}),
        });

    TriangleMesh test4=TriangleMesh(
        {
        Vertex(Point(0.0f, 0.0f, 0.0f),0),
        Vertex(Point(1.0f, 0.0f, 0.0f),1),
        Vertex(Point(2.0f, 0.0f, 0.0f),3),
        Vertex(Point(3.0f, 0.0f, 0.0f),5),
        Vertex(Point(4.0f, 0.0f, 0.0f),7),
        Vertex(Point(4.0f, 1.0f, 0.0f),7),
        Vertex(Point(3.0f, 1.0f, 0.0f),6),
        Vertex(Point(2.0f, 1.0f, 0.0f),4),
        Vertex(Point(1.0f, 1.0f, 0.0f),2),
        Vertex(Point(0.0f, 1.0f, 0.0f),0),

        }, {
        TriangleFace({0,8,9},{-1,-1,1}),
        TriangleFace({0,1,8},{2,0,-1}),
        TriangleFace({1,7,8},{-1,1,3}),
        TriangleFace({1,2,7},{4,2,-1}),
        TriangleFace({2,6,7},{-1,3,5}),
        TriangleFace({2,3,6},{6,4,-1}),
        TriangleFace({3,5,6},{-1,5,7}),
        TriangleFace({3,4,5},{-1,6,-1}),
        });
    
    
    // std::cout<<"[DEBUG] : in_convex (oui) :"<<in_convex_enveloppe(test2,Point(0.3f, 0.3f, 0.0f))<<"\n";
    // std::cout<<"[DEBUG] : int_convex (non ) :"<<in_convex_enveloppe(test2,Point(-1.0f, -1.0f, 0.0f))<<"\n";
    // std::cout<<"[DEBUG] : flippable (oui): "<<flipable(test2,Edge(0,2))<<"\n";
    // std::cout<<"[DEBUG] : flippable (oui): "<<flipable(test3,Edge(0,0))<<"\n";
    
    // CodeEdge code1=out_triangle(test4, Point(3.9,0.9,0), Point(0.1,0.2,0), Edge(-1,-1));
    // std::cout<<"[DEBUG] : out_triangle (0,0,2) : "<<code1.code<<","<<code1.edge.id_f<<","<<code1.edge.local_id_v<<"\n";
    
    // CodeEdge code2=out_triangle(test4, Point(3.9,0.9,0), Point(0.1,0.2,0), Edge(1,0));
    // std::cout<<"[DEBUG] : out_triangle (0,2,2): "<<code2.code<<","<<code2.edge.id_f<<","<<code2.edge.local_id_v<<"\n";

    // CodeEdge code3=target_triangle(test4, Point(3.9,0.9,0));
    // std::cout<<"[DEBUG] : out_triangle (0,2,2): "<<code3.code<<","<<code3.edge.id_f<<","<<code3.edge.local_id_v<<"\n";
    
    
    // std::cout<<"VALIDITY : "<<test3.checkValidity()<<"\n";
    // print_mesh_info1(test3,"avant");
    // std::cout<<"[DEBUG] : addpoint (split triangle)\n";
    // addPoint(test3,Point(0.4,0.35,0));
    // std::cout<<"VALIDITY : "<<test3.checkValidity(false,true)<<"\n";
    // print_mesh_info1(test3,"apres");
    // test3.writeOFF("test3.off");
    // std::cout<<"[Maillage OK] "<<test3.checkValidity()<<"\n";
    // std::cout<<"\n[DEBUG] : addpoint (split edge)\n";
    // addPoint(test3,Point(0.5,-1,0));
    // std::cout<<"VALIDITY : "<<test3.checkValidity(false,true)<<"\n";
    // print_mesh_info1(test3,"apres2");
    // test3.writeOFF("test4.off");
    // addPoint(test3,Point(-1,1,0));
    // std::cout<<"VALIDITY : "<<test3.checkValidity(false,true)<<"\n";
    // print_mesh_info1(test3,"apres3");
    // test3.writeOFF("test5.off");
    // edgeFlip(test3,Edge(0,0));
    // std::cout<<"VALIDITY : "<<test3.checkValidity(false,true)<<"\n";
    // print_mesh_info1(test3,"apres4");
    // test3.writeOFF("test6.off");
    // edgeFlip(test3,Edge(0,0));
    // std::cout<<"VALIDITY : "<<test3.checkValidity(false,true)<<"\n";
    // print_mesh_info1(test3,"apres5");
    // test3.writeOFF("test7.off");
    // std::cout<<"\n[DEBUG] : addpoint (out convexe)\n";
    // addPoint(test3,Point(-1,1,0));
    // test3.writeOFF("test5.off");

    // std::cout<<"[DEBUG] : Converion Point->edge :"<<find_face_with_vertices(test4, 2, 7)<<"\n";

    std::vector<Point> points = {
    {0.1f, 0.1f, 0},
    {0.2f, 0.5f, 0},
    {0.4f, 0.2f, 0},
    {0.3f, 0.8f, 0},
    {0.7f, 0.1f, 0},
    {0.6f, 0.5f, 0},
    {0.5f, 0.9f, 0},
    {0.9f, 0.2f, 0},
    {0.8f, 0.6f, 0},
    {0.95f, 0.8f, 0},
    {0.15f, 0.9f, 0},
    {0.35f, 0.6f, 0},
    {0.55f, 0.3f, 0},
    {0.75f, 0.4f, 0},
    {0.85f, 0.9f, 0}
};
    TriangleMesh test5=triangulize(points);
//     test5.writeOFF("test6.off");

    std::cout<<"[DEBUG]: 1";
    return 0;
}