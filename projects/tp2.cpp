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
#include <string>

#include "geom.h"
#include "mesh.h"
#include "basic_shapes.h"
#include "laplacian.h"

using namespace std;

void exp_heat_source(TriangleMesh &mesh, map<int, float> bound_cond, string out_dir)
{
    float dt = 1e-7;
    HeatSimulator simu(mesh, bound_cond);
    for (int i = 0; i < 1000000; i++)
    {
        simu.step(dt);
        if (i % 5000 == 0)
        {
            mesh.writeFunctionOFF(simu.u, out_dir + to_string(simu.timestep) + ".off");
        }
    }
}

void exp_curvature(TriangleMesh &mesh, string out_dir)
{
    vector<float> ux(mesh.vertices.size(), 0.0f);
    vector<float> uy(mesh.vertices.size(), 0.0f);
    vector<float> uz(mesh.vertices.size(), 0.0f);
    for (Iterator_on_vertices i = mesh.vertices_begin(); i != mesh.vertices_past_the_end(); ++i)
    {
        ux[*i] = mesh.vertices[*i].x;
        uy[*i] = mesh.vertices[*i].y;
        uz[*i] = mesh.vertices[*i].z;
    }
    LaplacianComputer lap_comp(mesh);
    const vector<float> &lap_ux = lap_comp.laplacian(ux);
    const vector<float> &lap_uy = lap_comp.laplacian(uy);
    const vector<float> &lap_uz = lap_comp.laplacian(uz);

    vector<Vector> lap_vec(mesh.vertices.size(), Vector(0, 0, 0));
    vector<float> curv(mesh.vertices.size(), 0.0f);
    for (Iterator_on_vertices i = mesh.vertices_begin(); i != mesh.vertices_past_the_end(); ++i)
    {
        lap_vec[*i] = Vector(lap_ux[*i], lap_uy[*i], lap_uz[*i]);
        curv[*i] = max(0.0f, min(sqrt(norm(lap_vec[*i])), 20.0f));
    }
    mesh.writeFunctionOFF(curv, out_dir);
}

int main()
{
    TriangleMesh queen;
    queen.loadOFF("data/queen.off");
    exp_curvature(queen, "out/curv.off");

    exp_heat_source(queen, {{44057, 15.0f}, {25501, -15.0f}}, "out/heat4/heat_queen");
}