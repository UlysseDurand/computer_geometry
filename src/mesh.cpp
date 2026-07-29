#include "mesh.h"

using namespace std;

#pragma region Vertex
Vertex::Vertex(Point p) : x(p.x), y(p.y), z(p.z), triFaceId(-1) {}
Vertex::Vertex(Point p, FaceId id) : x(p.x), y(p.y), z(p.z), triFaceId(id) {}

void Vertex::addFace(FaceId id) { triFaceId = id; }
Point Vertex::point() { return Point(x, y, z); }
#pragma endregion
#pragma region TriangleFace
TriangleFace::TriangleFace(array<VertexId, 3> v) : vertices(v), adj_faces({-1, -1, -1}) {}
TriangleFace::TriangleFace(array<VertexId, 3> v, array<FaceId, 3> adj_f) : vertices(v), adj_faces(adj_f) {};

LocalVertexId TriangleFace::find_vertex(VertexId A) const
{
    auto it = std::find(std::begin(vertices), std::end(vertices), A);
    if (it != vertices.end())
        return std::distance(vertices.begin(), it);
    else
        return -1;
}

std::ostream &operator<<(std::ostream &os, const TriangleFace &t)
{
    os << "{vertices: (" << t.vertices[0] << ", " << t.vertices[1] << ", " << t.vertices[2] << "), "
       << "adj_faces: (" << t.adj_faces[0] << ", " << t.adj_faces[1] << ", " << t.adj_faces[2] << ")}";
    return os;
}
#pragma endregion
#pragma region Iterators
Iterator_on_faces::Iterator_on_faces() : mesh(nullptr), faceId(0) {}

Iterator_on_faces::Iterator_on_faces(TriangleMesh *m) : mesh(m), faceId(0) {}

Iterator_on_faces Iterator_on_faces::operator++()
{
    faceId++;
    return *this;
}

FaceId Iterator_on_faces::operator*() { return faceId; }

bool Iterator_on_faces::operator==(const Iterator_on_faces &other) const { return faceId == other.faceId; }

bool Iterator_on_faces::operator!=(const Iterator_on_faces &other) const { return !(*this == other); }

Iterator_on_faces TriangleMesh::faces_begin() { return Iterator_on_faces(this); }

Iterator_on_faces TriangleMesh::faces_past_the_end()
{
    Iterator_on_faces res(this);
    res.faceId = faces.size();
    return res;
}

Iterator_on_vertices::Iterator_on_vertices() : mesh(nullptr), vertexId(0) {}

Iterator_on_vertices::Iterator_on_vertices(TriangleMesh *m) : mesh(m), vertexId(0) {}

Iterator_on_vertices Iterator_on_vertices::operator++()
{
    ++vertexId;
    return *this;
}

VertexId Iterator_on_vertices::operator*() { return vertexId; }

bool Iterator_on_vertices::operator==(const Iterator_on_vertices &other) const { return vertexId == other.vertexId; }

bool Iterator_on_vertices::operator!=(const Iterator_on_vertices &other) const { return !(*this == other); }

Iterator_on_vertices TriangleMesh::vertices_begin() { return Iterator_on_vertices(this); }

Iterator_on_vertices TriangleMesh::vertices_past_the_end()
{
    Iterator_on_vertices res(this);
    res.vertexId = vertices.size();
    return res;
}

Circulator_around_vertex::Circulator_around_vertex() : mesh(nullptr), pivotVertex(-1), face(-1), localIndex(-1) {}

Circulator_around_vertex::Circulator_around_vertex(TriangleMesh *m, VertexId pv) : mesh(m), pivotVertex(pv)
{
    face = mesh->vertices[pivotVertex].triFaceId;
    localIndex = mesh->faces[face].find_vertex(pivotVertex);
}

LocalVertexId Circulator_around_vertex::getLocalVertexId()
{
    return localIndex;
}

Circulator_around_vertex &Circulator_around_vertex::operator++()
{
    face = mesh->faces[face].adj_faces[(localIndex + 1) % 3];
    localIndex = mesh->faces[face].find_vertex(pivotVertex);
    return *this;
}

Circulator_around_vertex Circulator_around_vertex::operator++(int)
{
    Circulator_around_vertex temp = *this;
    ++(*this);
    return temp;
}

bool Circulator_around_vertex::operator==(const Circulator_around_vertex &other) const
{
    return pivotVertex == other.pivotVertex && face == other.face;
}

bool Circulator_around_vertex::operator!=(const Circulator_around_vertex &other) const
{
    return !(*this == other);
}

Circulator_on_faces::Circulator_on_faces() : Circulator_around_vertex() {}

Circulator_on_faces::Circulator_on_faces(TriangleMesh *m, VertexId pv) : Circulator_around_vertex(m, pv) {}

FaceId Circulator_on_faces::operator*()
{
    return face;
}

Circulator_on_vertices::Circulator_on_vertices() : Circulator_around_vertex() {}

Circulator_on_vertices::Circulator_on_vertices(TriangleMesh *m, VertexId pv) : Circulator_around_vertex(m, pv) {}

VertexId Circulator_on_vertices::operator*()
{
    return mesh->faces[face].vertices[localIndex];
}
#pragma endregion
#pragma region TriangleMesh
TriangleMesh::TriangleMesh() {}

TriangleMesh::TriangleMesh(const vector<Point> &points)
{
    for (Point p : points)
        vertices.push_back(Vertex(p));
}

FaceId TriangleMesh::addNewFace(array<VertexId, 3> vert)
{
    TriangleFace face(vert);
    FaceId idFace = faces.size();
    for (VertexId v : vert)
        vertices[v].triFaceId = idFace;
    faces.push_back(face);
    return idFace;
}

FaceId TriangleMesh::addAdjFace(FaceId idFace0, VertexId opposVertex, VertexId D)
{
    LocalVertexId iA = faces[idFace0].find_vertex(opposVertex);
    if (iA == -1)
        throw std::runtime_error("Vertex not found in face");
    FaceId newFace = addNewFace({faces[idFace0].vertices[(iA + 1) % 3], D, faces[idFace0].vertices[(iA + 2) % 3]});
    faces[idFace0].adj_faces[iA] = newFace;
    faces[newFace].adj_faces[1] = idFace0;
    return newFace;
}

void TriangleMesh::sew(FaceId idFace0, VertexId A, FaceId idFace1, VertexId D)
{
    LocalVertexId iA = faces[idFace0].find_vertex(A);
    if (iA == -1)
        throw std::runtime_error("Vertex not found in face");
    faces[idFace0].adj_faces[iA] = idFace1;

    LocalVertexId iD = faces[idFace1].find_vertex(D);
    if (iD == -1)
        throw std::runtime_error("Vertex not found in face");
    faces[idFace1].adj_faces[iD] = idFace0;
}

void TriangleMesh::writeOFF(string filename)
{
    ofstream ofs;
    ofs.open(filename);
    if (ofs.bad())
    {
        throw runtime_error("Write impossible in " + filename);
    }
    ofs << "OFF" << endl;
    ofs << vertices.size() << " " << faces.size() << " 0" << endl;
    for (Vertex v : vertices)
        ofs << v.x << " " << v.y << " " << v.z << endl;
    for (TriangleFace f : faces)
        ofs << "3 " << f.vertices[0] << " " << f.vertices[1] << " " << f.vertices[2] << endl;
}

void TriangleMesh::writeFunctionOFF(const std::vector<float> &u, const std::string &filename)
{
    int nv = vertices.size();
    int nf = faces.size();

    // Trouver min/max pour normalisation
    float umin = *std::min_element(u.begin(), u.end());
    float umax = *std::max_element(u.begin(), u.end());
    std::cout << "[EXPORT] u_min = " << umin
              << " | u_max = " << umax
              << " | fichier = " << filename << "\n";

    std::ofstream out(filename);
    if (!out)
    {
        std::cerr << "Impossible d'ouvrir " << filename << "\n";
        return;
    }

    out << "OFF\n";
    out << nv << " " << nf << " 0\n";
    for (const auto &v : vertices)
    {
        out << v.x << " " << v.y << " " << v.z << "\n";
    }
    // Faces + couleurs
    for (int f = 0; f < nf; ++f)
    {
        const auto &tri = faces[f].vertices;
        // Moyenne sur la face
        float uf = (u[tri[0]] + u[tri[1]] + u[tri[2]]) / 3.0f;
        // Normalisation
        float t = (uf - umin) / (umax - umin + 1e-12f);
        // Couleur
        auto c = colormap(t);
        out << "3 "
            << tri[0] << " " << tri[1] << " " << tri[2] << " "
            << int(c[0]) << " " << int(c[1]) << " " << int(c[2]) << " " << int(c[3])
            << "\n";
    }

    std::cout << "Fichier OFF écrit : " << filename << "\n";
}

void TriangleMesh::loadOFF(string filename)
{
    int nbsommet, nbface, nbedge;

    ifstream file(filename);
    if (!file.is_open())
        throw runtime_error("fichier non trouvé");

    string header;
    file >> header;
    file >> nbsommet >> nbface >> nbedge;

    for (VertexId i = 0; i < nbsommet; i++)
    {
        float x, y, z;
        file >> x >> y >> z;
        vertices.push_back(Vertex(Point(x, y, z)));
    }

    for (FaceId f = 0; f < nbface; f++)
    {
        VertexId skip, a, b, c;
        file >> skip >> a >> b >> c;
        faces.push_back(TriangleFace({a, b, c}));
    }
    file.close();

    // keys are edges ((a, b) indices of vertices)
    // values are (f, local_id), the face of index f and the edge has local indices ((local_id+1)%3, (local_id+2)%3)
    map<pair<VertexId, VertexId>, pair<FaceId, LocalVertexId>> myMap;

    for (FaceId f = 0; f < nbface; f++)
    {
        for (VertexId local_id = 0; local_id < 3; local_id++)
        {
            vertices[faces[f].vertices[local_id]].triFaceId = f;
            VertexId v1 = faces[f].vertices[(local_id + 1) % 3];
            VertexId v2 = faces[f].vertices[(local_id + 2) % 3];
            pair<VertexId, VertexId> edge = (v1 < v2) ? make_pair(v1, v2) : make_pair(v2, v1);
            auto it = myMap.find(edge);
            if (it == myMap.end())
                myMap[edge] = make_pair(f, local_id);
            else
            {
                FaceId other_face = it->second.first;
                LocalVertexId other_local = it->second.second;

                faces[f].adj_faces[local_id] = other_face;
                faces[other_face].adj_faces[other_local] = f;
            }
        }
    }
}

TriangleMesh::TriangleMesh(vector<Vertex> vertices, vector<TriangleFace> faces) : vertices(vertices), faces(faces) {}

Circulator_on_faces TriangleMesh::incident_faces(VertexId vertexIndex)
{
    return Circulator_on_faces(this, vertexIndex);
}

Circulator_on_vertices TriangleMesh::vertices_around(VertexId vertexIndex)
{
    return Circulator_on_vertices(this, vertexIndex);
}

bool TriangleMesh::checkValidity(bool testClosed, bool throw_error)
{
    // Check every vertex is in the face it references
    for (int i = 0; i < vertices.size(); i++)
        if (vertices[i].triFaceId != -1 && faces[vertices[i].triFaceId].find_vertex(i) == -1)
        {
            if (throw_error)
                cout << ("Vertex " + to_string(i) + " has no attached face or its attached face doesn't contain it") <<vertices[i].triFaceId<<","<<faces[vertices[i].triFaceId].find_vertex(i) << endl;
            return false;
        }

    // Check every face neighbour of a face neighbour is the face itself
    for (int f = 0; f < faces.size(); f++)
    {
        for (int j = 0; j < 3; j++)
        {
            // One adjacent face
            int f2 = faces[f].adj_faces[j];
            if (f2==-1)break;
            // Local index j2 in f2 of vertex not in face f (then f is opposite to it)
            int j2 = (faces[f2].find_vertex(faces[f].vertices[(j + 1) % 3]) + 1) % 3;
            if (testClosed && f2 < 0)
                return false;
            if (f2 >= 0 && faces[f2].adj_faces[j2] != f)
            {
                if (throw_error)
                    cout << ("face " + to_string(f) + "has adjacent face of local index" + to_string(j) + ", face global index" + to_string(f2) + " whose local index corresponding adjacent face is " + to_string(j2) + " and global index doesnt correspond") << endl;
                return false;
            }
        }
    }
    return true;
}

bool TriangleMesh::contains2D(FaceId id, Point p)
{
    for (VertexId i = 0; i < 3; i++)
    {
        Point a = vertices[faces[id].vertices[i]].point();
        Point b = vertices[faces[id].vertices[i]].point();
        if (!test_orientation2D(a, b, p))
            return false;
    }
    return true;
}
#pragma endregion