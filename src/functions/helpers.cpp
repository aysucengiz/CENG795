//
// Created by vicy on 10/18/25.
//

#include "helpers.h"
#include "overloads.h"
#include "../dataTypes/object/Object.h"


real max3(real a, real b, real c)
{
    return std::max(a, std::max(b, c));
}
real min3(real a, real b, real c)
{
    return std::min(a, std::min(b, c));
}


Vertex maxVert3(Vertex &a, Vertex &b, Vertex &c)
{
    return Vertex(
        max3(a.x, b.x, c.x),
        max3(a.y, b.y, c.y),
        max3(a.z, b.z, c.z)
    );
}

Vertex minVert3(Vertex &a, Vertex &b, Vertex &c)
{
    return Vertex(
        min3(a.x, b.x, c.x),
        min3(a.y, b.y, c.y),
        min3(a.z, b.z, c.z)
    );
}


Vertex maxVert2(Vertex a, Vertex b)
{
    return Vertex(std::max(a.x,b.x), std::max(a.y,b.y), std::max(a.z,b.z));
}
Vertex minVert2(Vertex a, Vertex b)
{
    return Vertex(std::min(a.x,b.x), std::min(a.y,b.y), std::min(a.z,b.z));
}
int clamp(const real c, const int from, const int to){
    int temp = static_cast<int>(std::round(c));
    if(temp > to) return to;
    if(temp< from) return from;
    return temp;
}

Vec3r x_product(const Vec3r &v, const Vec3r &w)
{
    return Vec3r(v.j * w.k - v.k * w.j,
                 v.k * w.i - v.i * w.k,
                 v.i * w.j - v.j * w.i);
}

real dot_product(const Vec3r &a, const Vec3r &b){
    return a.i*b.i + a.j*b.j + a.k*b.k;
}

real determinant(const Vec3r &first, const Vec3r &second, const Vec3r &third ){
    //  a  d  g
    //  b  e  h
    //  c  f  i
    // return a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h -e*g);
    return first.i*(second.j*third.k - third.j*second.k) + first.j*(third.i*second.k - second.i*third.k) + first.k*(second.i*third.j -second.j*third.i);
}

Axes next(Axes a)
{
    if (a == Axes::x) return Axes::y;
    if (a == Axes::y) return Axes::z;
    if (a == Axes::z) return Axes::x;
    else return Axes::x;
}







