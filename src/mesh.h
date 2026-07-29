#ifndef mesh_h
#define mesh_h

#include <array>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <map>
#include <algorithm>
#include <stdexcept>

#include "color.h"
#include "geom.h"

using namespace std;

typedef int VertexId;
typedef int FaceId;
typedef int LocalVertexId;

class TriangleMesh;

class Vertex
{
public:
    Vertex(Point p);
    Vertex(Point p, FaceId id);
    void addFace(FaceId id);
    Point point();

    float x;
    float y;
    float z;

    FaceId triFaceId; // Id of a face containing the point
};

class Edge
{
public:
    Edge(int id_f, int local_id_v)
        : id_f(id_f), local_id_v(local_id_v) {}

    int id_f;
    int local_id_v;
};

class TriangleFace
{
public:
    array<VertexId, 3> vertices;
    array<FaceId, 3> adj_faces;

    TriangleFace(array<VertexId, 3> v);
    TriangleFace(array<VertexId, 3> v, array<FaceId, 3>);
    LocalVertexId find_vertex(VertexId A) const;
    friend std::ostream &operator<<(std::ostream &os, const TriangleFace &t);
};

class Iterator_on_faces
{
public:
    TriangleMesh *mesh;
    FaceId faceId;

    Iterator_on_faces();
    Iterator_on_faces(TriangleMesh *m);

    Iterator_on_faces operator++();
    FaceId operator*();
    bool operator==(const Iterator_on_faces &other) const;
    bool operator!=(const Iterator_on_faces &other) const;
};

class Iterator_on_vertices
{
public:
    TriangleMesh *mesh;
    VertexId vertexId;

    Iterator_on_vertices();
    Iterator_on_vertices(TriangleMesh *m);

    Iterator_on_vertices operator++();
    VertexId operator*();
    bool operator==(const Iterator_on_vertices &other) const;
    bool operator!=(const Iterator_on_vertices &other) const;
};

class Circulator_around_vertex
{
protected:
    TriangleMesh *mesh;
    VertexId pivotVertex;
    FaceId face;
    LocalVertexId localIndex;

public:
    LocalVertexId getLocalVertexId();

    Circulator_around_vertex();
    Circulator_around_vertex(TriangleMesh *m, VertexId pv);

    Circulator_around_vertex &operator++();
    Circulator_around_vertex operator++(int);

    bool operator==(const Circulator_around_vertex &other) const;
    bool operator!=(const Circulator_around_vertex &other) const;
};

class Circulator_on_faces : public Circulator_around_vertex
{
public:
    Circulator_on_faces();
    Circulator_on_faces(TriangleMesh *m, VertexId pv);

    FaceId operator*();
};

class Circulator_on_vertices : public Circulator_around_vertex
{
public:
    Circulator_on_vertices();
    Circulator_on_vertices(TriangleMesh *m, VertexId pv);

    VertexId operator*();
};

class TriangleMesh
{
public:
    vector<Vertex> vertices;
    vector<TriangleFace> faces;

    TriangleMesh();                            // Nothing initially
    TriangleMesh(const vector<Point> &points); // No faces initially
    TriangleMesh(vector<Vertex> vertices, vector<TriangleFace> faces);

    // Iterators
    Iterator_on_vertices vertices_begin();                        // Iterates through all vertices
    Iterator_on_vertices vertices_past_the_end();                 // End of iterator
    Iterator_on_faces faces_begin();                              // Iterates through all faces
    Iterator_on_faces faces_past_the_end();                       // End of iterator
    Circulator_on_faces incident_faces(VertexId vertexIndex);     // Iterates through all faces around a vertex
    Circulator_on_vertices vertices_around(VertexId vertexIndex); // Iterates through all vertices around a vertex

    // Construct by adding faces
    FaceId addNewFace(array<VertexId, 3> v);                // Adds a new face
    FaceId addAdjFace(FaceId f0, VertexId A, VertexId D);   // Adds a new triangle that connects the edge opposite to A in f0 to D
    void sew(FaceId f0, VertexId A, FaceId f1, VertexId D); // Sews the edge opposite from A in f0 to the one opposite from D in f1

    bool checkValidity(bool testClosed = false, bool throw_error = false); // Checks if the mesh is well built
    bool contains2D(FaceId id, Point p);

    // IO
    void writeOFF(string filename);
    void writeFunctionOFF(const std::vector<float> &u, const std::string &filename);
    void loadOFF(string filename);
};

#endif