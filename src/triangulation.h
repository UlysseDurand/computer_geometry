#ifndef triangulation_h
#define triangulation_h

#include "mesh.h"
#include "mesh_ops.h"
#include "basic_shapes.h"

#include <queue>


struct CodeEdge {
    int code;
    Edge edge;

    CodeEdge(int c, const Edge& e) : code(c), edge(e) {}
};


Point Barycentre(TriangleMesh &mesh,TriangleFace triangle){
    Vertex A=mesh.vertices[triangle.vertices[0]];
    Vertex B=mesh.vertices[triangle.vertices[1]];
    Vertex C=mesh.vertices[triangle.vertices[2]];
    return Point((A.x+B.x+C.x)/3,(A.y+B.y+C.y)/3,0);
}

bool cross_edge(TriangleMesh &mesh, FaceId id_f, LocalVertexId loc_id_v,Point Target, Point origin){
    Vertex A=mesh.vertices[mesh.faces[id_f].vertices[(loc_id_v+1)%3]];
    Vertex B=mesh.vertices[mesh.faces[id_f].vertices[(loc_id_v+2)%3]];
    float or1= test_orientation2D_float(Point (A.x,A.y,0), Point (B.x,B.y,0), Target.to2D());
    float or2= test_orientation2D_float(Point (A.x,A.y,0), Point (B.x,B.y,0), origin.to2D());
    float or3= test_orientation2D_float(Target.to2D(),origin.to2D(),Point (A.x,A.y,0));
    float or4= test_orientation2D_float(Target.to2D(),origin.to2D(),Point (B.x,B.y,0));
    return (or1*or2<0 && or3*or4<0);

}

bool in_edge(TriangleMesh &mesh, FaceId id_f, LocalVertexId loc_id_v,Point Target){
    Vertex A=mesh.vertices[mesh.faces[id_f].vertices[(loc_id_v+1)%3]];
    Vertex B=mesh.vertices[mesh.faces[id_f].vertices[(loc_id_v+2)%3]];
    return test_orientation2D_float(Point (A.x,A.y,0), Point (B.x,B.y,0),Target.to2D())==0;
}

CodeEdge out_triangle(TriangleMesh &mesh, Point target, Point origin, Edge previous_edge)
{
    int id_previous_triangle = previous_edge.id_f;

    int id_new = (id_previous_triangle == -1)
        ? 0
        : mesh.faces[id_previous_triangle].adj_faces[previous_edge.local_id_v];
    if (id_new==-1){
        return CodeEdge(1,Edge(-1,-1));
    }

    TriangleFace &triangle = mesh.faces[id_new];


    for (int i = 0; i < 3; i++)
    {
        // point exactement sur une arête
        if (in_edge(mesh, id_new, i, target))
            return CodeEdge(2, Edge(id_new, i));

        int adj = triangle.adj_faces[i];
        if (adj != id_previous_triangle &&
            cross_edge(mesh, id_new, i, target, origin))
        {
            return adj==-1?CodeEdge(3, Edge(id_new, i)):CodeEdge(0, Edge(id_new, i));
        }
    }

    // point dans le triangle
    return CodeEdge(1, Edge(id_new, -1));
}



bool flipable(TriangleMesh &mesh, Edge edge)
{
    int id_f = edge.id_f;
    int loc_id_v = edge.local_id_v;

    TriangleFace t1 = mesh.faces[id_f];
    int id_f2 = t1.adj_faces[loc_id_v];

    TriangleFace t2 = mesh.faces[id_f2];

    // Triangle 1 : C opposé à l'arête AB
    VertexId C = t1.vertices[loc_id_v];
    VertexId A = t1.vertices[(loc_id_v + 1) % 3];
    VertexId B = t1.vertices[(loc_id_v + 2) % 3];

    // Triangle 2 : trouver D opposé à AB
    int idB = t2.find_vertex(B);
    int idA = t2.find_vertex(A);

    // D est le sommet qui n'est ni A ni B
    VertexId D;
    for (int i = 0; i < 3; i++) {
        VertexId v = t2.vertices[i];
        if (v != A && v != B) {
            D = v;
            break;
        }
    }

    float ax = mesh.vertices[A].x - mesh.vertices[D].x;
    float ay = mesh.vertices[A].y - mesh.vertices[D].y;

    float bx = mesh.vertices[B].x - mesh.vertices[D].x;
    float by = mesh.vertices[B].y - mesh.vertices[D].y;

    float cx = mesh.vertices[C].x - mesh.vertices[D].x;
    float cy = mesh.vertices[C].y - mesh.vertices[D].y;

    float det =
        (ax*ax + ay*ay) * (bx*cy - by*cx) -
        (bx*bx + by*by) * (ax*cy - ay*cx) +
        (cx*cx + cy*cy) * (ax*by - ay*bx);

    return det > 0;
}


CodeEdge target_triangle(TriangleMesh &mesh, Point target)
// Si codeEdge.code=0, on traverse un triange
// Si codeEdge.code=1, Point au milieu du triangle codeEdge.edge.id_f
// Si codeEdge.code=2, Point au milieu de l'arrete codeEdge.edge
// Si codeEdge.code=3, Point ors enveloppe convex
{
    int id = 0;
    Point origin = Barycentre(mesh,mesh.faces[0]);
    CodeEdge codeEdge=out_triangle(mesh, target, origin,Edge(-1, -1));

    while (codeEdge.code ==0) {
        codeEdge = out_triangle(mesh, target,origin , codeEdge.edge);
    }

    return codeEdge;
}

bool in_triangle(TriangleMesh &mesh ,TriangleFace triangle, Point D){

    int idA=triangle.vertices[0];
    int idB=triangle.vertices[1];
    int idC=triangle.vertices[2];

    Vertex A=mesh.vertices[idA];
    Vertex B=mesh.vertices[idB];
    Vertex C=mesh.vertices[idC];

    float o1 = test_orientation2D_float(Point (A.x,A.y,0), Point (B.x,B.y,0), D.to2D());
    float o2 = test_orientation2D_float(Point (B.x,B.y,0), Point (C.x,C.y,0), D.to2D());
    float o3 = test_orientation2D_float(Point (C.x,C.y,0), Point (A.x,A.y,0), D.to2D());

    return ((o1 >= 0 && o2 >= 0 && o3 >= 0) ||
            (o1 <= 0 && o2 <= 0 && o3 <= 0));

    

}



void multipleEdgeFlips(TriangleMesh &m, std::queue<Edge_point> &to_flip)
{
    while (!to_flip.empty())
    {
        Edge_point  edge_point = to_flip.front();
        to_flip.pop();
        

        if (face==-1){
            break;
        }


        if (edge.id_f!=-1 && edge.local_id_v!=-1 && flipable(m,edge))
        {
            //TODO peut etre pas obligé d'ajouter les 4 mais seulement les deux du bon coté
            to_flip.push(Edge(edge.id_f,(edge.local_id_v+1)%3));
            to_flip.push(Edge(edge.id_f,(edge.local_id_v+2)%3));
            int id= m.faces[edge.id_f].adj_faces[edge.local_id_v];
            int local_id=find_local_index(m,id,edge.id_f);
            to_flip.push(Edge(id,(local_id+1)%3));
            to_flip.push(Edge(id,(local_id+2)%3));
            edgeFlip(m,edge);
            
        }
    }
    
}

Edge next_edge(TriangleMesh &mesh, Edge edge, int direction)
{
    int new_local_index = (edge.local_id_v + direction + 3) % 3;
    int A=mesh.faces[ edge.id_f].vertices[(new_local_index+1)%3];
    int B=mesh.faces[ edge.id_f].vertices[(new_local_index+2)%3];
    std::cout<<"NEXT EDGE 0 : "<<A<<","<<B<<"\n";
    if (mesh.faces[edge.id_f].adj_faces[new_local_index] == -1){
        std::cout<<"NEXT EDGE 1\n";
        return Edge(edge.id_f, new_local_index);
    }

    int old_face = edge.id_f;
    int new_face = mesh.faces[edge.id_f].adj_faces[new_local_index];

    int id_local = (find_local_index(mesh, new_face, old_face)+1)%3;

    A=mesh.faces[ new_face].vertices[(id_local+1)%3];
    B=mesh.faces[ new_face].vertices[(id_local+2)%3];
    std::cout<<"NEXT EDGE 10 : "<<A<<","<<B<<"\n";
    int compteur=0;

    while (mesh.faces[new_face].adj_faces[id_local] != -1 && compteur<10)
    {
        compteur+=1;
        old_face = new_face;

        new_face = mesh.faces[new_face]
                       .adj_faces[(id_local + direction + 3) % 3];

        id_local = find_local_index(mesh, new_face, old_face);

        ///DEBUG
        A=mesh.faces[ new_face].vertices[(id_local+1)%3];
        B=mesh.faces[ new_face].vertices[(id_local+2)%3];
        
        std::cout<<"NEXT EDGE 2 : "<<A<<","<<B<<"\n";


  
    }
    std::cout<<"NEXT EDGE 3\n";
    return Edge(new_face, id_local);
}





int in_convex_enveloppe(TriangleMesh &m, Point p,int &face_begin)
{
   
    int local_id = find_local_index(m, face_begin, -1);

    Edge start_edge(face_begin, local_id);
    Edge e = start_edge;

    do
    {
        TriangleFace triangle = m.faces[e.id_f];

        int idA = triangle.vertices[(e.local_id_v+1)%3];
        int idB = triangle.vertices[(e.local_id_v+2)%3];

        Vertex A = m.vertices[idA];
        Vertex B = m.vertices[idB];

        if (test_orientation2D_float(Point(A.x,A.y,0), Point(B.x,B.y,0), p.to2D()) > 0)
        {
            e = next_edge(m, e, -1);
        }
        else
        {
            return e.id_f;
        }

    } while (!(e.id_f == start_edge.id_f && e.local_id_v == start_edge.local_id_v));

    return -1;
}

void print_mesh_info1(const TriangleMesh& mesh, const std::string& name)
{
    cout << name << " :" << endl;
    cout << "  nb vertices = " << mesh.vertices.size() << endl;
    cout << "  nb faces    = " << mesh.faces.size() << endl;

    for (int i = 0; i < (int)mesh.vertices.size(); i++)
    {
        cout << "  vertex[" << i << "] = ("
             << mesh.vertices[i].x << ", "
             << mesh.vertices[i].y << ", "
             << mesh.vertices[i].z << ", "
             << mesh.vertices[i].triFaceId << ")" << endl;
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

void add_point_out_convex_envelop(TriangleMesh &mesh, int face, Point Target,int &begin_face)
{
    std::queue<Edge_point> queue_edge;

    int new_vertex = mesh.vertices.size();
    int nb_face = mesh.faces.size();

    mesh.vertices.push_back(Vertex(Target, nb_face)); // ajout du point

    int local_index = find_local_index(mesh, face, -1);
    mesh.faces[face].adj_faces[local_index] = nb_face;
    begin_face=nb_face;
    int A = mesh.faces[face].vertices[(local_index + 2) % 3];
    int B = mesh.faces[face].vertices[(local_index + 1) % 3];

    // création du premier triangle
    mesh.faces.push_back(TriangleFace({A, B, new_vertex}, {-1, -1, face}));

    Edge edge_left = next_edge(mesh, Edge(face, local_index), -1);
    Edge edge_right = next_edge(mesh, Edge(face, local_index), 1);



    std::cout<<"Arrete ajouter : "<<A<<","<<B<<"\n";

    A=mesh.faces[ edge_left.id_f].vertices[( edge_left.local_id_v+1)%3];
    B=mesh.faces[ edge_left.id_f].vertices[( edge_left.local_id_v+2)%3];

    std::cout<<"Future Arrete Gauche : "<<A<<","<<B<<"\n";

    A=mesh.faces[ edge_right.id_f].vertices[( edge_right.local_id_v+1)%3];
    B=mesh.faces[ edge_right.id_f].vertices[( edge_right.local_id_v+2)%3];

    std::cout<<"Future Droite : "<<A<<","<<B<<"\n";
    while (true)
    {
        int idA = mesh.faces[edge_left.id_f].vertices[(edge_left.local_id_v + 2) % 3];
        int idB = mesh.faces[edge_left.id_f].vertices[(edge_left.local_id_v + 1) % 3];

        float left = test_orientation2D_float(
            Point(mesh.vertices[idA].x, mesh.vertices[idA].y, 0),
            Point(mesh.vertices[idB].x, mesh.vertices[idB].y, 0),
            Target.to2D()
        );

        if (left <= 0) break; // condition de sortie avant de créer une face inutile

        // push avant création de la face
        queue_edge.push(edge_left);


        int nf = mesh.faces.size();

        mesh.faces[edge_left.id_f].adj_faces[edge_left.local_id_v] = nf;

        mesh.faces[nf-1].adj_faces[0]=nf;
        begin_face=nb_face;
        mesh.faces.push_back(TriangleFace({idA, idB, new_vertex}, {- 1,nf -1, edge_left.id_f}));

        // passer à l'arête suivante après création
        edge_left = next_edge(mesh, edge_left, -1);
    }

    // Boucle droite
    while (true)
    {
        int idA = mesh.faces[edge_right.id_f].vertices[(edge_right.local_id_v + 2) % 3];
        int idB = mesh.faces[edge_right.id_f].vertices[(edge_right.local_id_v + 1) % 3];

        float right = test_orientation2D_float(
            Point(mesh.vertices[idA].x, mesh.vertices[idA].y, 0),
            Point(mesh.vertices[idB].x, mesh.vertices[idB].y, 0),
            Target.to2D()
        );

        if (right <= 0) break; // condition de sortie

        // push avant création de la face
    
        queue_edge.push(edge_right)


        int nf = mesh.faces.size();

        mesh.faces[edge_right.id_f].adj_faces[edge_right.local_id_v] = nf;
        mesh.faces[nf-1].adj_faces[1]=nf;
        begin_face=nb_face;
        mesh.faces.push_back(TriangleFace({idA, idB, new_vertex}, {nf-1, - 1, edge_right.id_f}));

        edge_right = next_edge(mesh, edge_right, 1);
    }

    // exécuter les flips après tout ajout
    //multipleEdgeFlips(mesh, queue_edge);
}

void addPoint(TriangleMesh &mesh, Point Target, int &begin_face){
    // Test si le point est dans l'envelope convex
   std::queue<Edge_point> queue_edge;
   int in_convex=in_convex_enveloppe(mesh, Target,begin_face);
    if (in_convex==-1){
        std::cout<<"[In Convex Envelop]\n";
        CodeEdge codeEdge=target_triangle(mesh, Target );

        if (codeEdge.code==1){
            std::cout<<"[Triangle Split]\n";
            int id=codeEdge.edge.id_f;

            triangleSplit(mesh, codeEdge.edge.id_f, Target);
            queue_edge.push(Edge(id,2));
            queue_edge.push(Edge(mesh.faces.size()-2,2));
            queue_edge.push(Edge(mesh.faces.size()-1,2));
            
      
 
        }else if (codeEdge.code==2)
            {
            std::cout<<"[Edge Split]\n";
            int id_local1=codeEdge.edge.local_id_v;
            int id1=codeEdge.edge.id_f;
            TriangleFace triangle1=mesh.faces[id1];
            int id2=triangle1.adj_faces[id_local1];
            int id_local2=find_local_index(mesh,id2,id1);
            if (id_local2!=-1){
            TriangleFace triangle2=mesh.faces[id2];
            
            edgeSplit(mesh, codeEdge.edge, Target);
            queue_edge.push(Edge(mesh.faces.size()-2,2));
            queue_edge.push(Edge(mesh.faces.size()-1,2));
            }
        }else{
    
            int face=mesh.faces[codeEdge.edge.id_f].adj_faces[codeEdge.edge.local_id_v];
            triangleSplit(mesh, face, Target);
            int face_inf=mesh.faces.size().1;
            for (int i=0;i<3;i++){
                if (mesh.faces[face_inf].vertices[i]==0){
                    face_inf=face;
                    break;
                }
            }
            Circulator_on_faces circ= face_to_circ_inf(mesh,face_inf);
            


        }
    //multipleEdgeFlips(mesh,queue_edge);
    }else{
        std::cout<<"[Out Convex Envelop] :"<<in_convex<<"\n";

        add_point_out_convex_envelop(mesh,in_convex,Target,begin_face);
        

        
    }
    
    
}

TriangleMesh triangulize(vector<Point> points)
{
    float test=test_orientation2D_float(points[0],points[1],points[2]);
    TriangleMesh res = TriangleMesh(
    {
        Vertex(points[0], 0),
        Vertex(test > 0 ? points[1] : points[2], 0),
        Vertex(test > 0 ? points[2] : points[1], 0)
    },
    {
        TriangleFace({0, 1, 2}, {-1, -1, -1})
    }
);
    int begin_face=0;
    
    
    for (int i = 3; i < points.size(); i++)
    {
        addPoint(res,points[i],begin_face);
        std::cout << "VALIDITY : " << res.checkValidity(false, true) << "\n";
        print_mesh_info1(res, std::to_string(i - 3) + " eme ajout");
        res.writeOFF("test" + std::to_string(i - 3) + ".off");
    }
    return res;
}

#endif

