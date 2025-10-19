//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <sstream>
#include <string>
#include <vector>


typedef float real;

enum class MaterialType{
    NONE,
    NORMAL,
    MIRROR
};

enum class ObjectType{
    NONE,
    TRIANGLE,
    SPHERE,
    MESH
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

    bool isWhite()
    {
        if(r == 0.0 && g == 0.0 && b == 0.0) return true;
        return false;

    }
};

struct Vec3r{
    real i;
    real j;
    real k;
    Vec3r(){}
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

    Vec3r normalize();
    real mag() const { return i*i + j*j + k*k;}


};

typedef struct Vertex{
    real x;
    real y;
    real z;

    Vertex(){}
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
    : _id(id), Position(pos), Gaze(g), Up(u), nearDistance(nd), ImageName(imname)
    {
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

    Material(uint32_t id, Color ar, Color dr, Color sr, Color mr, uint32_t pe)
    : _id(id), AmbientReflectance(ar), DiffuseReflectance(dr), SpecularReflectance(sr), PhongExponent(pe), MirrorReflectance(mr)
    {
        if(!mr.isWhite()) materialType = MaterialType::MIRROR;
        else if (ar.isWhite() && dr.isWhite()&& sr.isWhite()) materialType = MaterialType::NONE;
        else materialType = MaterialType::NORMAL;
    }

};



#endif //DATATYPES_H
