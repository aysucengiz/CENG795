//
// Created by vicy on 11/08/25.
//

#ifndef CENG795_VECTORS_H
#define CENG795_VECTORS_H

#include <sstream>
#include <string>
#include <cmath>

#include "../../typedefs.h"

struct Vec3r{
    real i, j, k;

    Vec3r();
    Vec3r(real i, real j, real k);
    Vec3r(std::string inp);

    Vec3r operator-() const;
    Vec3r& operator+=(const Vec3r& other);
    Vec3r& operator/=(const uint32_t& other);
    real operator[](Axes a) const;

    Vec3r normalize() const;
    real mag() const;
};

struct Vertex{
    real x;
    real y;
    real z;
    Vertex();
    Vertex(real x,real y,real z);
    Vertex(std::string inp);
    Vertex operator-() const;

    real operator[](Axes a) const;

};

struct Ray{
    Vertex pos;
    Vec3r dir;

    Ray();
    Ray(const Ray &r);
    Ray(Vertex p, Vec3r d);
    Ray& operator=(const Ray &r);
};

Vec3r x_product(const Vec3r &v, const Vec3r &w);


struct Vec4r{
    real x, y, z, w;
    Vec4r(real x, real y, real z, real w);
    explicit Vec4r(Vec3r v);
    explicit Vec4r(Vertex v);

    Vec4r operator-() const;

    Vec4r& operator+=(const Vec3r& other);
    Vec4r& operator+=(const Vertex& other);
    Vec4r& operator+=(const Vec4r& other);
    Vec4r& operator/=(const uint32_t& other);
    Vec4r& operator/=(const real& other);

    Vec3r getVec3r() const;
    Vertex getVertex() const;
};



#endif //CENG795_VECTORS_H