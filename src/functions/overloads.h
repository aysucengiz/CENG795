//
// Created by vicy on 11/08/25.
//

#ifndef CENG795_OVERLOADS_H
#define CENG795_OVERLOADS_H

#include "../dataTypes/base/SceneData.h"
#include "../dataTypes/base/Vectors.h"
#include "../dataTypes/object/Object.h"
#include "../dataTypes/object/Mesh.h"
#include "../dataTypes/../acceleration/BVH.h"

class RayTracer;

// overload - type assignment
FilterType getFilter(const std::string &s);
SamplingType getSampling(const std::string &s);
ThreadType getThread(const std::string &s);
AccelerationType getAcceleration(const std::string &s);
PivotType getPivot(const std::string &s);
TextureType getTextureType(const std::string &s);
DecalMode getDecalMode(const std::string &s);


Texel operator -(const Texel &a, const Texel &b);
Texel operator +(const Texel &a, const Texel &b);

// overload - color
Color operator *(const Color &a, const Color &b);
Color operator -(const Color &a, const Color &b);
Color operator +(const Color &a, const Color &b);
Color operator /(const Color &a, const real b);
Color operator /(const real b, const Color &a);
Color operator /(const Color &b, const Color& a);


Color operator *(const Color &a, const real b);
Color operator *(const real b,const Color &a);

// overload - vector
Vec3r operator *(const Vec3r &a, const real mult);
Vec3r operator /(const Vec3r &a, const real denum);
Vec3r operator /(const Vec3r &a, const Vec3r &b);
// overload - return Vertex
Vertex operator /(const Vertex &a, real other);
Vertex operator +(const Vertex &a, const Vertex &b);
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

std::ostream& operator<<(std::ostream& os, TransformationType t);
std::ostream& operator<<(std::ostream& os, PivotType t);
std::ostream& operator<<(std::ostream& os, BVHNodeType t);
std::ostream& operator<<(std::ostream& os, Axes x);
std::ostream& operator<<(std::ostream& os, BVH &bvh);
std::ostream& operator<<(std::ostream& os, BVHNode &node);
std::ostream& operator<<(std::ostream& os, const BBox &bbox);
std::ostream& operator<<(std::ostream& os, const Instance& m);

std::ostream& operator<<(std::ostream& os, Transformation *t);
std::ostream& operator<<(std::ostream& os, Rotate &t);
std::ostream& operator<<(std::ostream& os, Translate &t);
std::ostream& operator<<(std::ostream& os, Scale &t);
std::ostream& operator<<(std::ostream& os, Composite &t);

std::ostream& operator<<(std::ostream& os, std::array<std::array<double,4>,4> &arr);
std::ostream& operator<<(std::ostream& os, const RayTracer& rt);

#endif //CENG795_OVERLOADS_H