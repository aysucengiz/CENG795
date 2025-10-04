//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <sstream>
#include <string>
#include <vector>
#include "Object.h"

typedef enum MaterialType{
    NONE,
    MIRROR
}MaterialType;


typedef struct Color{
    double r;
    double g;
    double b;

    Color(){r = 0; g = 0; b = 0;}

    Color(std::string inp) {
        std::istringstream ss(inp);
        ss >> r >> g >> b;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid color string: " + inp);
        }
    }
} Color;

typedef struct Vector{
    double i;
    double j;
    double k;
    Vector(){}

    Vector(std::string inp) {
        std::istringstream ss(inp);
        ss >> i >> j >> k;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid Vector string: " + inp);
        }
    }
} Vector;

typedef struct Vertex{
    double x;
    double y;
    double z;
    Vertex(double x,double y,double z) : x(x), y(y), z(z){}

    Vertex(std::string inp) {
        std::istringstream ss(inp);
        ss >> x >> y >> z;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid Vertex string: " + inp);
        }
    }
} Vertex;

typedef struct Camera{
    uint32_t _id;
    Vertex Position;
    Vector Gaze;
    Vector Up;
    int l;
    int r;
    int t;
    int b;
    int z;
    uint32_t ImageResolution[2];
    std::string ImageName;

    Camera(uint32_t id, Vertex pos, Vector g, Vector u, std::string locs, std::string res, std::string imname)
    : _id(id), Position(pos), Gaze(g), Up(u), ImageName(imname)
    {
        std::istringstream s1(locs);
        s1 >> l >> r >> t >> b >> z;

        std::istringstream s2(res);
        s2 >> ImageResolution[0] >> ImageResolution[1];
    }

} Camera;

typedef struct PointLight{
    uint32_t _id;
    Vertex Position;
    Color Intensity;

    PointLight(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}

} PointLight;

typedef struct Material{
    uint32_t _id;
    Color AmbientReflectance;
    Color DiffuseReflectance;
    Color SpecularReflectance;
    uint32_t PhongExponent;
    MaterialType materialType;
    Color MirrorReflectance;

    Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe, std::string t, Color mr)
    : _id(id), AmbientReflectance(ar), DiffuseReflectance(dr), SpecularReflectance(sr), PhongExponent(pe), MirrorReflectance(mr)
    {
        if(t == "mirror") materialType = MIRROR;
        else materialType = NONE;
    }

} Material;

typedef struct SceneInput{
    unsigned int MaxRecursionDepth;
    Color BackgroundColor;
    double ShadowRayEpsilon;
    double IntersectionTestEpsilon;
    std::vector<Camera> Cameras;
    Color AmbientLight;
    std::vector<PointLight> PointLights;
    std::vector<Material> Materials;
    std::vector<Vertex> Vertices;
    std::vector<Triangle> Triangles;
    std::vector<Sphere> Spheres;
} SceneInput;




#endif //DATATYPES_H
