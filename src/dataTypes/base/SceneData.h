//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "typedefs.h"
#include "Vectors.h"
#include "../functions/helpers.h"

struct Color{
    real r;
    real g;
    real b;

    Color();
    Color(real r, real g, real b);
    Color(std::string inp);
    Color& operator+=(const Color& other);
    bool isWhite() const;
    Color operator-();
    Color exponent();
};


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

    Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u,
           std::string locs, real nd,
           std::string res,
           std::string imname);
};

struct PointLight{
    uint32_t _id;
    Vertex Position;
    Color Intensity;

    PointLight(uint32_t id, Vertex pos, Color intens);
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
        std::string type = "",
        Color mr = Color(),
        Color ac = Color(),
        real refrIndex = 0,
        real ai = 0);
};



struct CVertex
{
    uint32_t id;
    Vertex v;
    Vec3r n;

    CVertex(uint32_t i, real x,real y,real z);
    CVertex(uint32_t i, const std::string& inp);
};



#endif //DATATYPES_H
