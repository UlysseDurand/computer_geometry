#ifndef geom_h
#define geom_h

#include <iostream>

class Point
{
public:
    float x;
    float y;
    float z;

    Point(float x, float y, float z) : x(x), y(y), z(z) {}
    Point to2D();
};

class Vector
{
public:
    float x;
    float y;
    float z;

    Vector(float x, float y, float z) : x(x), y(y), z(z) {}

    Vector(Point a, Point b) : x(b.x - a.x), y(b.y - a.y), z(b.z - a.z) {}

    void print()
    {
        std::cout << x << "\n"
                  << y << "\n"
                  << z << "\n";
    }

    float norm()
    {
        return std::sqrt(x * x + y * y + z * z);
    }
};

inline Vector cross(Vector a, Vector b)
{
    return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
}

inline float dot(Vector a, Vector b)
{
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline float norm2(Vector v)
{
    return dot(v, v);
}

inline float norm(Vector v)
{
    return sqrt(norm2(v));
}

inline float cos(Point a, Point b, Point c)
{
    return dot(Vector(a, b), Vector(a, c)) / Vector(a, b).norm() / Vector(a, c).norm();
}

inline float sin(Point a, Point b, Point c)
{
    return cross(Vector(a, b), Vector(a, c)).norm() / Vector(a, b).norm() / Vector(a, c).norm();
}

inline float cot(Point a, Point b, Point c)
{
    return cos(a, b, c) / sin(a, b, c);
}

inline float area(Point A, Point B, Point C)
{
    Vector vec1(Vector(A, B));
    Vector vec2(Vector(A, C));
    return cross(vec1, vec2).norm() / 2;
}

inline bool test_orientation2D(Point A, Point B, Point C)
{
    return (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x) > 0;
}

inline float test_orientation2D_float(Point A, Point B, Point C)
{
return (B.x - A.x)*(C.y - A.y) - (B.y - A.y)*(C.x - A.x);
}

#endif