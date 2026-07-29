#ifndef laplacian_h
#define laplacian_h

#include <vector>
#include <map>
#include <optional>
#include <functional>

#include "mesh.h"

struct AlphasBetasAreas
{
    std::vector<int> index;
    std::vector<float> AreaAlphaBeta;
};

class LaplacianComputer
{
public:
    TriangleMesh &mesh;
    AlphasBetasAreas precomp;
    vector<float> u_laplacian;

    LaplacianComputer(TriangleMesh &m)
        : mesh(m),
          precomp(preprocessAlphasBetas()),
          u_laplacian(m.vertices.size(), 0.0f) {}

    AlphasBetasAreas preprocessAlphasBetas();

    const vector<float> &laplacian(const vector<float> &u);
};

class HeatSimulator
{
public:
    float t;
    map<int, float> dirichlet_boundary_condition;
    LaplacianComputer lapl_comp;
    int timestep = 0;

    vector<float> u;

    void step(float dt)
    {
        const auto &lap = lapl_comp.laplacian(u);
        for (const auto &pair : dirichlet_boundary_condition)
            u[pair.first] = pair.second;
        for (int i = 0; i < u.size(); i++)
            u[i] += lap[i] * dt;
        for (const auto &pair : dirichlet_boundary_condition)
            u[pair.first] = pair.second;
        t += dt;
        timestep++;
    }

    HeatSimulator(TriangleMesh &m, map<int, float> dir_cond)
        : t(0),
          dirichlet_boundary_condition(dir_cond),
          lapl_comp(m),
          u(m.vertices.size()) {}
};

#endif