//
// Created by vicy on 10/18/25.
//

#ifndef CENG795_HELPERS_H
#define CENG795_HELPERS_H

#include "DataTypes.h"
#include "Object.h"
#include "Matrix.h"
#include <cmath>

int clamp(real c, int from, int to);
Color exponent(Color c);

real dot_product(const Vec3r &a, const Vec3r &b);
real determinant(const Vec3r &first, const Vec3r &second, const Vec3r &third );



// overload - color
Color operator *(const Color &a, const Color &b);
Color operator -(const Color &a, const Color &b);
Color operator +(const Color &a, const Color &b);
Color operator /(const Color &a, const real b);
Color operator *(const Color &a, const real b);

// overload - vector
Vec3r operator *(const Vec3r &a, const real mult);
Vec3r operator /(const Vec3r &a, const real denum);

// overload - return Vertex
Vertex operator +(const Vec3r &a, const Vertex &b);
Vertex operator +(const Vertex &a, const Vec3r &b);
Vertex operator -(const Vec3r &a, const Vertex &b);
Vertex operator -(const Vertex &a, const Vec3r &b);

// overload - return Vector
Vec3r operator +(const Vec3r &a, const Vec3r &b);
Vec3r operator -(const Vec3r &a, const Vec3r &b);
Vec3r operator -(const Vertex &a, const Vertex &b);

// overload vec4r
Vec4r operator *(const Vec4r &a, const real &b);
Vec4r operator /(const Vec4r &a, const real &b);
Vec4r operator -(const Vec4r &a, const Vec4r &b);
Vec4r operator -(const Vertex &a, const Vec4r &b);
Vec4r operator -(const Vec4r &b, const Vertex &a);
Vec4r operator -(const Vec3r &a, const Vec4r &b);
Vec4r operator -(const Vec4r &b, const Vec3r &a);
Vec4r operator +(const Vec4r &a, const Vec4r &b);
Vec4r operator +(const Vertex &a, const Vec4r &b);
Vec4r operator +(const Vec4r &b, const Vertex &a);
Vec4r operator +(const Vec3r &a, const Vec4r &b);
Vec4r operator +(const Vec4r &b, const Vec3r &a);

Vec4r operator *(const M4trix &a, const Vec4r &b);

// print
std::ostream& operator<<(std::ostream& os, const Color& c);
std::ostream& operator<<(std::ostream& os, const Vec3r& v);
std::ostream& operator<<(std::ostream& os, const Vertex& v);
std::ostream& operator<<(std::ostream& os, const CVertex& v);
std::ostream& operator<<(std::ostream& os, const PointLight & pl);
std::ostream& operator<<(std::ostream& os, const SceneInput& s);
std::ostream& operator<<(std::ostream& os, const Camera& c);
std::ostream& operator<<(std::ostream& os, const Material& m);
std::ostream& operator<<(std::ostream& os, const Triangle& t);
std::ostream& operator<<(std::ostream& os, const Sphere& s);
std::ostream& operator<<(std::ostream& os, const Mesh& m);
std::ostream& operator<<(std::ostream& os, const Plane& m);
std::ostream& operator<<(std::ostream& os, const MaterialType& mt);
std::ostream& operator<<(std::ostream& os, const ObjectType& ot);
std::ostream& operator<<(std::ostream& os, Object *s);
#endif //CENG795_HELPERS_H