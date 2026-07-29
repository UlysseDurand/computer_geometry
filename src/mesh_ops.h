#ifndef mesh_ops_h
#define mesh_ops_h

#include "mesh.h"

inline FaceId find_face(TriangleMesh m, Point p)
{
    for (Iterator_on_faces it = m.faces_begin(); it != m.faces_past_the_end(); ++it)
        if (m.contains2D(*it, p))
            return *it;
    return -1;
}

int find_local_index(TriangleMesh &mesh,int face, int voisin){
    for (int i=0; i<3;i++){;
        if (mesh.faces[face].adj_faces[i]==voisin){
            return i;
        }
    }
    return -1;
}

void print_mesh_info2(const TriangleMesh& mesh, const std::string& name)
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

void triangleSplit(TriangleMesh& mesh,int indexFace,Point newPoint ){
    TriangleFace triangle=mesh.faces[indexFace]; //Récupération du triangle à coupé

    //Récupération de l'index du prochain point et de la prochaine face

    int indexP=mesh.vertices.size();
    int indexV=mesh.faces.size();
    std::cout<<"Face : "<<indexFace<<","<<indexP<<","<<indexV<<"\n";
    //Ajout du nouveau Point
    mesh.vertices.push_back(Vertex(newPoint,indexFace));

    //Mise a jour du triangle coupé

    int a=triangle.vertices[0];
    int b=triangle.vertices[1];
    int c=triangle.vertices[2];
    std::cout<<"Point : "<<a<<","<<b<<","<<c<<"\n";
    int va=triangle.adj_faces[0];
    int vb=triangle.adj_faces[1];
    int vc=triangle.adj_faces[2];
    std::cout<<"Voisin : "<<a<<","<<b<<","<<c<<"\n";

    mesh.faces[indexFace].vertices[2]=indexP;
    mesh.faces[indexFace].adj_faces[0]=indexV;
    mesh.faces[indexFace].adj_faces[1]=indexV+1;
    
    //Mise a jour des face de chaque "ancien" point 

    mesh.vertices[c].addFace(indexV);

    //mise à jour ancienne voisn
    if (va!=-1){
        int local_indice_a=find_local_index(mesh,va,indexFace);
        mesh.faces[va].adj_faces[local_indice_a]=indexV;
    }
    if (vb!=-1){
        int local_indice_b=find_local_index(mesh,vb,indexFace);
        mesh.faces[vb].adj_faces[local_indice_b]=indexV+1;
    }
        if (vc!=-1){
        int local_indice_c=find_local_index(mesh,vc,indexFace);
        mesh.faces[vc].adj_faces[local_indice_c]=indexFace;
    }


    //Création de 2 nouvelles face
    mesh.faces.push_back(TriangleFace({b,c,indexP},{indexV+1,indexFace,va}));
    mesh.faces.push_back(TriangleFace({c,a,indexP},{indexFace,indexV,vb}));
}

void edgeSplit(TriangleMesh& mesh,Edge edge,Point newPoint){

    int Face1=edge.id_f;
    int indicelocal=edge.local_id_v;

    TriangleFace triangle1=mesh.faces[Face1]; //Récupération du premeier triangles à coupés

    int Face2=triangle1.adj_faces[indicelocal];

    TriangleFace triangle2=mesh.faces[Face2];

    //Récupération de l'index du prochain point et de la prochaine face

    int indexP=mesh.vertices.size();
    int indexV=mesh.faces.size();

    //Ajout du nouveau Point
    mesh.vertices.push_back(Vertex(newPoint,Face1));

    //Recupération des indice locaux de A,B,C,D (on se ramene a une jonction BC entre les deux triangles)
    
    
    
    //Récupération des info

    VertexId A=triangle1.vertices[indicelocal];
    VertexId B=triangle1.vertices[(indicelocal+1)%3];
    VertexId C=triangle1.vertices[(indicelocal+2)%3];

    LocalVertexId indexD=(triangle2.find_vertex(B)+1)%3;

    VertexId D=triangle2.vertices[indexD];



    int vb1=triangle1.adj_faces[(indicelocal+1)%3];
    int vc1=triangle1.adj_faces[(indicelocal+2)%3];
    int vb2=triangle2.adj_faces[(indexD+2)%3];
    int vc2=triangle2.adj_faces[(indexD+1)%3];
    //Mise a jours des face des vertex

    mesh.vertices[A].addFace(Face1);
    mesh.vertices[B].addFace(Face2);
    mesh.vertices[C].addFace(Face1);
   
    //Mise a jour des 2 anciens triangles

    mesh.faces[Face1].vertices[0]=A;
    mesh.faces[Face1].vertices[1]=indexP;
    mesh.faces[Face1].vertices[2]=C;
    mesh.faces[Face1].adj_faces[0]=indexV+1;
    mesh.faces[Face1].adj_faces[1]=vb1;
    mesh.faces[Face1].adj_faces[2]=Face2;

    mesh.faces[Face2].vertices[0]=A;
    mesh.faces[Face2].vertices[1]=B;
    mesh.faces[Face2].vertices[2]=indexP;
    mesh.faces[Face2].adj_faces[0]=indexV;
    mesh.faces[Face2].adj_faces[1]=Face1;
    mesh.faces[Face2].adj_faces[2]=vc1;

    //Mise a jour des anciens points

    mesh.vertices[D].addFace(indexV);
    
    //Mise à jour des voisin

      if (vb1!=-1){
        int local_indice_a=find_local_index(mesh,vb1,Face1);
        mesh.faces[vb1].adj_faces[local_indice_a]=Face1;
    }
    if (vc1!=-1){
        int local_indice_b=find_local_index(mesh,vc1,Face1);
        mesh.faces[vc1].adj_faces[local_indice_b]=Face2;
    }
        if (vb2!=-1){
        int local_indice_c=find_local_index(mesh,vb2,Face2);
        mesh.faces[vb2].adj_faces[local_indice_c]=indexV+1;
    }
     if (vc2!=-1){
        int local_indice_c=find_local_index(mesh,vc2,Face2);
        mesh.faces[vc2].adj_faces[local_indice_c]=indexV;
    }

    //Création de 2 nouvelles face

    mesh.faces.push_back(TriangleFace({B,D,indexP},{indexV+1,Face2,vc2}));
    mesh.faces.push_back(TriangleFace({D,C,indexP},{Face1,indexV,vb2}));
}

void edgeFlip(TriangleMesh& mesh,Edge edge){
    int Face1=edge.id_f;
    int indicelocal=edge.local_id_v;
    std::cout<<"ARRETE CONCERNE : "<<mesh.faces[Face1].vertices[(indicelocal+1)%3]<<" , "<<mesh.faces[Face1].vertices[(indicelocal+2)%3]<<"\n";

    TriangleFace triangle1=mesh.faces[Face1]; //Récupération du premeier triangles à coupés

    int Face2=triangle1.adj_faces[indicelocal];

    TriangleFace triangle2=mesh.faces[Face2];

    
    //Recupération des indice locaux de A,B,C,D (on se ramene a une jonction BC entre les deux triangles)
    
    
    
    //Récupération des info

    VertexId A=triangle1.vertices[indicelocal];
    VertexId B=triangle1.vertices[(indicelocal+1)%3];
    VertexId C=triangle1.vertices[(indicelocal+2)%3];
    
    LocalVertexId indexD=(triangle2.find_vertex(B)+1)%3;
    
    VertexId D=triangle2.vertices[indexD];

    


    int vb1=triangle1.adj_faces[(indicelocal+1)%3];
    int vc1=triangle1.adj_faces[(indicelocal+2)%3];
    int vb2=triangle2.adj_faces[(indexD+2)%3];
    int vc2=triangle2.adj_faces[(indexD+1)%3];

    //Mise a jour des voisin


    if (vc1!=-1){
        int local_indice_b=find_local_index(mesh,vc1,Face1);
        mesh.faces[vc1].adj_faces[local_indice_b]=Face2;
    }
        if (vb2!=-1){
        int local_indice_c=find_local_index(mesh,vb2,Face2);
        mesh.faces[vb2].adj_faces[local_indice_c]=Face1;
    }
    
    
    //Mise a jours des face des vertex

    mesh.vertices[A].addFace(Face2);
    mesh.vertices[B].addFace(Face2);
    mesh.vertices[C].addFace(Face1);
    mesh.vertices[D].addFace(Face2);
   
    //Mise a jour des 2 triangles

    mesh.faces[Face2].vertices[0]=A;
    mesh.faces[Face2].vertices[1]=B;
    mesh.faces[Face2].vertices[2]=D;
    mesh.faces[Face2].adj_faces[0]=vc2;
    mesh.faces[Face2].adj_faces[1]=Face1;
    mesh.faces[Face2].adj_faces[2]=vc1 ;

    mesh.faces[Face1].vertices[0]=A ;
    mesh.faces[Face1].vertices[1]=D;
    mesh.faces[Face1].vertices[2]=C;
    mesh.faces[Face1].adj_faces[0]=vb2;
    mesh.faces[Face1].adj_faces[1]=vb1;
    mesh.faces[Face1].adj_faces[2]=Face2;
    
}
    
Circulator_on_faces face_to_circ_inf(TriangleMesh &m, FaceId f) {
    res = m.incident_faces(0);
    res.face = f;
    res.localIndex = (m.faces[f].find_vertex(0) + 1)%3;
    return res;
}

#endif