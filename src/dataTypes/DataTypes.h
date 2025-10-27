//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>


typedef float real;

enum class MaterialType{
    NONE,
    NORMAL,
    MIRROR,
    CONDUCTOR,
    DIELECTRIC
};

enum class ObjectType{
    NONE,
    TRIANGLE,
    SPHERE,
    MESH,
    PLANE
};

enum class ShadingType{
    NONE,
    SMOOTH,
    FLAT
};




struct Color{
    real r;
    real g;
    real b;

    Color(){r = 0; g = 0; b = 0;}
    Color(real r, real g, real b) : r(r), g(g), b(b) {}

    Color(std::string inp) {
        std::istringstream ss(inp);
        ss >> r >> g >> b;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid color string: " + inp);
        }
    }

    Color& operator+=(const Color& other) {
        r += other.r;
        g += other.g;
        b += other.b;
        return *this;  // Return the modified object
    }

    bool isWhite() const
    {
        if(r == 0.0 && g == 0.0 && b == 0.0) return true;
        return false;

    }
};

struct Vec3r{
    real i;
    real j;
    real k;
    Vec3r(){i=0.0;j=0.0;k=0.0;}
    Vec3r(real i, real j, real k) : i(i), j(j), k(k){}

    Vec3r(std::string inp) {
        std::istringstream ss(inp);
        ss >> i >> j >> k;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid Vec3r string: " + inp);
        }
    }

    Vec3r operator-() const {
        return Vec3r{-i, -j, -k};
    }

    Vec3r& operator+=(const Vec3r& other) {
        i += other.i;
        j += other.j;
        k += other.k;
        return *this;  // Return the modified object
    }

    Vec3r& operator/=(const uint32_t& other) {
        i /= static_cast<real>(other);
        j /= static_cast<real>(other);
        k /= static_cast<real>(other);
        return *this;  // Return the modified object
    }

    Vec3r normalize() const;
    real mag() const { return sqrt(i*i + j*j + k*k);}


};

typedef struct Vertex{
    real x;
    real y;
    real z;

    Vertex(){x=0.0; y=0.0; z=0.0;}
    Vertex(real x,real y,real z) : x(x), y(y), z(z){}
    Vertex(std::string inp) {
        std::istringstream ss(inp);
        ss >> x >> y >> z;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid Vertex string: " + inp);
        }
    }
} Vertex;

typedef struct Ray{
    Vertex pos;
    Vec3r dir;

    Ray(){}
    Ray(Ray &r) : pos(r.pos), dir(r.dir){}
    Ray& operator=(const Ray &r)
    {
        if (this != &r) {pos = r.pos; dir = r.dir;}
        return *this;
    }
} Ray;

Vec3r x_product(const Vec3r &v, const Vec3r &w);


struct Camera{
    uint32_t _id;
    Vertex Position;
    Vec3r Gaze;
    Vec3r Up;
    real l;
    real r;
    real b;
    real t;
    real nearDistance;
    uint32_t width;
    uint32_t height;
    std::string ImageName;

    Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::string locs, real nd, std::string res, std::string imname)
    : _id(id), Position(pos), nearDistance(nd), ImageName(imname)
    {
        Gaze = g.normalize();
        Up = x_product(-Gaze,x_product(u.normalize(),-Gaze)).normalize();
        std::istringstream s1(locs);
        s1 >> l >> r >> b >> t;
        std::istringstream s2(res);
        s2 >> width >> height;
    }

};

struct PointLight{
    uint32_t _id;
    Vertex Position;
    Color Intensity;

    PointLight(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}

};

struct Material{
    uint32_t _id;
    Color AmbientReflectance;
    Color DiffuseReflectance;
    Color SpecularReflectance;
    uint32_t PhongExponent;
    MaterialType materialType;
    Color MirrorReflectance;
    Color AbsorptionCoefficient;
    real RefractionIndex;
    real AbsorptionIndex;

    Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe,
        std::string type = "", Color mr = Color(), Color ac = Color(), real refrIndex = 0, real ai = 0)
    : _id(id),
    AmbientReflectance(ar),
    DiffuseReflectance(dr),
    SpecularReflectance(sr),
    PhongExponent(pe),
    MirrorReflectance(mr),
    AbsorptionCoefficient(ac),
    RefractionIndex(refrIndex),
    AbsorptionIndex(ai)
    {
        if (type == "dielectric") materialType = MaterialType::DIELECTRIC;
        else if (type == "conductor") materialType = MaterialType::CONDUCTOR;
        else if (type == "mirror") materialType = MaterialType::MIRROR;
        else if(!mr.isWhite()) materialType = MaterialType::MIRROR;
        else if (ar.isWhite() && dr.isWhite()&& sr.isWhite()&& mr.isWhite() && ac.isWhite()) materialType = MaterialType::NONE;
        else materialType = MaterialType::NORMAL;
    }

};



struct CVertex
{
    uint32_t id;
    Vertex v;
    Vec3r n;

    CVertex(uint32_t i, real x,real y,real z) : v(Vertex(x,y,z)), n(Vec3r()), id(i){}
    CVertex(uint32_t i, const std::string& inp) : n(Vec3r()), id(i){
        std::istringstream ss(inp);
        ss >> v.x >> v.y >> v.z;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid Vertex string: " + inp);
        }
    }
};



#endif //DATATYPES_H
