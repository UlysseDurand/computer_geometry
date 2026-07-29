#include "laplacian.h"
#include "geom.h"

AlphasBetasAreas LaplacianComputer::preprocessAlphasBetas()
{
    AlphasBetasAreas tableau;

    for (int i = 0; i < mesh.vertices.size(); i++)
    {
        Circulator_on_faces c1(&mesh, i);
        Circulator_on_faces c2(&mesh, i);
        ++c2;

        float big_area = 0.0f;
        int start = *c1;
        do
        {
            TriangleFace face1 = mesh.faces[*c1];
            TriangleFace face2 = mesh.faces[*c2];

            LocalVertexId index = face1.find_vertex(i);
            LocalVertexId im1 = (index + 2) % 3;
            LocalVertexId ip1 = (index + 1) % 3;

            VertexId j = face1.vertices[ip1];
            tableau.index.push_back(j);

            Vertex vb = mesh.vertices[face1.vertices[im1]];
            Vertex vi = mesh.vertices[face1.vertices[index]];
            Vertex vj = mesh.vertices[face1.vertices[ip1]];

            LocalVertexId idx2 = mesh.faces[*c2].find_vertex(i);
            LocalVertexId jp2 = (idx2 + 1) % 3;
            Vertex va = mesh.vertices[face2.vertices[jp2]];

            float alpha = cot(vb.point(), vj.point(), vi.point());
            float beta = cot(va.point(), vi.point(), vj.point());

            tableau.AreaAlphaBeta.push_back(alpha);
            tableau.AreaAlphaBeta.push_back(beta);

            big_area += area(vb.point(), vi.point(), vj.point());
            ++c1;
            ++c2;

        } while (*c1 != start);

        tableau.index.push_back(-1);
        tableau.AreaAlphaBeta.push_back(big_area);
    }

    return tableau;
}

const vector<float> &LaplacianComputer::laplacian(const vector<float> &u)
{
    float lap_min = std::numeric_limits<float>::infinity();
    float lap_max = -std::numeric_limits<float>::infinity();

    int i = 0;  // sommet courant
    int k = 0;  // index dans tableau.index
    int k2 = 0; // index dans tableau.AreaAlphaBeta
    float lap = 0.0f;

    while (k < (int)precomp.index.size())
    {

        if (precomp.index[k] != -1)
        {
            // Voisin j du sommet i
            int j = precomp.index[k];

            float alpha = precomp.AreaAlphaBeta[k2];
            float beta = precomp.AreaAlphaBeta[k2 + 1];

            float contrib = (alpha + beta) * (u[j] - u[i]);

            lap += contrib;
            k2 += 2;
            k += 1;
        }
        else
        {
            float area = precomp.AreaAlphaBeta[k2];
            lap = lap / (2 * (area / 3));
            if (lap > lap_max)
                lap_max = lap;
            if (lap < lap_min)
                lap_min = lap;
            u_laplacian[i] = lap;
            k2 += 1;
            k += 1;
            i += 1;
            lap = 0.0f;
        }
    }

    return u_laplacian;
}