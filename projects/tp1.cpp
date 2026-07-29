#include <vector>
#include <algorithm>
#include <stdexcept>
#include <array>
#include <iostream>
#include <fstream>
#include <map>
#include <cassert>
#include <string>
#include <stdexcept>

using namespace std;

class Point
{
public:
    float x;
    float y;
    float z;

    Point(float x, float y, float z) : x(x), y(y), z(z) {}
};

class Vertex
{
public:
    Vertex(Point p) : x(p.x), y(p.y), z(p.z), triFaceId(-1) {}
    void addFace(int id) { triFaceId = id; }

    float x;
    float y;
    float z;

    int triFaceId; // Id of a face containing the point
};

class TriangleFace
{
public:
    array<int, 3> vertices;
    array<int, 3> adj_faces;

    TriangleFace(array<int, 3> v) : vertices(v), adj_faces({-1, -1, -1}) {}
    void addFace(vector<Vertex> &mesh_vertices, int idxFace)
    {
        for (int v : vertices)
            mesh_vertices[v].addFace(idxFace);
    }
    int find_vertex(int A) const
    {
        auto it = std::find(std::begin(vertices), std::end(vertices), A);
        if (it != vertices.end())
            return std::distance(vertices.begin(), it);
        else
            return -1;
    }

    void print()
    {
        cout << "vertices " << vertices[0] << " " << vertices[1] << " " << vertices[2] << " " << endl;
        cout << "adj_faces " << adj_faces[0] << " " << adj_faces[1] << " " << adj_faces[2] << " " << endl;
    }
};

class TriangleMesh
{
public:
    vector<Vertex> vertices;
    vector<TriangleFace> faces;

    TriangleMesh(vector<Vertex> vertices, vector<TriangleFace> faces) : vertices(vertices), faces(faces) {}
    TriangleMesh(const vector<Point> &points)
    {
        for (Point p : points)
            vertices.push_back(Vertex(p));
    }

    int addNewFace(array<int, 3> v)
    {
        TriangleFace face(v);
        int idxFace = faces.size();
        face.addFace(vertices, idxFace);
        faces.push_back(face);
        return idxFace;
    }

    int addAdjFace(int idFace0, int opposVertex, int D)
    {
        int iA = faces[idFace0].find_vertex(opposVertex);
        if (iA == -1)
            throw std::runtime_error("Vertex not found in face");
        int newFace = addNewFace({faces[idFace0].vertices[(iA + 1) % 3], D, faces[idFace0].vertices[(iA + 2) % 3]});
        faces[idFace0].adj_faces[iA] = newFace;
        faces[newFace].adj_faces[1] = idFace0;
        return newFace;
    }

    void sew(int idFace0, int A, int idFace1, int D)
    {
        int iA = faces[idFace0].find_vertex(A);
        if (iA == -1)
            throw std::runtime_error("Vertex not found in face");
        faces[idFace0].adj_faces[iA] = idFace1;

        int iD = faces[idFace1].find_vertex(D);
        if (iD == -1)
            throw std::runtime_error("Vertex not found in face");
        faces[idFace1].adj_faces[iD] = idFace0;
    }
};

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

vector<int> listFacesNeighVertex(const TriangleMesh &m, int A)
{
    vector<int> res;
    int f = m.vertices[A].triFaceId;
    int B0 = m.faces[f].vertices[(m.faces[f].find_vertex(A) + 1) % 3];
    int B = B0;
    do
    {
        f = m.faces[f].adj_faces[m.faces[f].find_vertex(B)];
        B = m.faces[f].vertices[(m.faces[f].find_vertex(A) + 1) % 3];
        res.push_back(f);
    } while (B != B0);
    return res;
}

bool checkTriangleMesh(const TriangleMesh &m, bool testClosed = false, bool throw_error = true)
{
    // Check every vertex is in the face it references
    for (int i = 0; i < m.vertices.size(); i++)
        if (m.vertices[i].triFaceId == -1 || m.faces[m.vertices[i].triFaceId].find_vertex(i) == -1)
        {
            if (throw_error)
                throw runtime_error("Vertex " + to_string(i) + " has no attached face or its attached face doesn't contain it");
            return false;
        }

    // Check every face neighbour of a face neighbour is the face itself
    for (int f = 0; f < m.faces.size(); f++)
    {
        for (int j = 0; j < 3; j++)
        {
            // One adjacent face
            int f2 = m.faces[f].adj_faces[j];

            // Local index j2 in f2 of vertex not in face f (then f is opposite to it)
            int j2 = (m.faces[f2].find_vertex(m.faces[f].vertices[(j + 1) % 3]) + 1) % 3;
            if (testClosed && f2 < 0)
                return false;
            if (f2 >= 0 && m.faces[f2].adj_faces[j2] != f)
            {
                if (throw_error)
                    throw runtime_error("face " + to_string(f) + "has adjacent face of local index" + to_string(j) + ", face global index" + to_string(f2) + " whose local index corresponding adjacent face is " + to_string(j2) + " and global index doesnt correspond");
                return false;
            }
        }
    }
    return true;
}

void writeMeshToOFF(TriangleMesh m, string filename)
{
    ofstream ofs;
    ofs.open(filename);
    if (ofs.bad())
    {
        throw runtime_error("Write impossible in " + filename);
    }
    ofs << "OFF" << endl;
    ofs << m.vertices.size() << " " << m.faces.size() << " 0" << endl;
    for (Vertex v : m.vertices)
        ofs << v.x << " " << v.y << " " << v.z << endl;
    for (TriangleFace f : m.faces)
        ofs << "3 " << f.vertices[0] << " " << f.vertices[1] << " " << f.vertices[2] << endl;
}

std::ostream &operator<<(std::ostream &os, const vector<int> &v)
{
    os << "[ ";
    for (int x : v)
    {
        os << x << " ";
    }
    os << "]";
    return os;
}

TriangleMesh loadOFF(string filename)
{
    int nbsommet, nbface, nbedge;

    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("fichier non trouvé");

    string header;
    file >> header;
    file >> nbsommet >> nbface >> nbedge;

    vector<Vertex> vertices;
    vector<TriangleFace> faces;

    for (int i = 0; i < nbsommet; i++)
    {
        float x, y, z;
        file >> x >> y >> z;
        vertices.push_back(Vertex(Point(x, y, z)));
    }

    for (int i = 0; i < nbface; i++)
    {
        int skip, a, b, c;
        file >> skip >> a >> b >> c;
        faces.push_back(TriangleFace({a, b, c}));
    }
    file.close();

    // keys are edges ((a, b) indices of vertices)
    // values are (f, local_id), the face of index f and the edge has local indices ((local_id+1)%3, (local_id+2)%3)
    map<pair<int, int>, pair<int, int>> myMap;

    for (int f = 0; f < nbface; f++)
    {
        for (int local_id = 0; local_id < 3; local_id++)
        {
            vertices[faces[f].vertices[local_id]].triFaceId = f;
            int v1 = faces[f].vertices[(local_id + 1) % 3];
            int v2 = faces[f].vertices[(local_id + 2) % 3];
            pair<int, int> edge = (v1 < v2) ? make_pair(v1, v2) : make_pair(v2, v1);
            auto it = myMap.find(edge);
            if (it == myMap.end())
            {
                myMap[edge] = make_pair(f, local_id);
            }
            else
            {
                int other_face = it->second.first;
                int other_local = it->second.second;

                faces[f].adj_faces[local_id] = other_face;
                faces[other_face].adj_faces[other_local] = f;
            }
        }
    }
    TriangleMesh res(vertices, faces);
    return res;
}

int main()
{
    TriangleMesh tetrahedron = make_tetrahedron(Point(0, 0, 0), Point(1, 0, 0), Point(0, 1, 0), Point(0, 0, 1));
    cout << "Tetrahedron is " << (checkTriangleMesh(tetrahedron) ? "valid" : "not valid") << endl;
    writeMeshToOFF(tetrahedron, "out/tetrahedron.off");

    TriangleMesh pyramid = make_pyramid(Point(1, 1, 1), Point(0, 0, 0), Point(2, 0, 0), Point(2, 0, 2), Point(0, 0, 2));
    cout << "Pyramid is " << (checkTriangleMesh(pyramid) ? "valid" : "not valid") << endl;
    writeMeshToOFF(pyramid, "out/pyramid.off");

    cout << listFacesNeighVertex(pyramid, 0) << endl;

    TriangleMesh queen = loadOFF("data/queen.off");
    cout << "Queen is " << (checkTriangleMesh(queen) ? "valid" : "not valid") << endl;
    writeMeshToOFF(queen, "out/queen_rebuilt.off");
}
