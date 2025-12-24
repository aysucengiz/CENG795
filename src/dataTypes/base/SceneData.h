//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <array>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "../../typedefs.h"
#include "Vectors.h"

struct Color{
    real r;
    real g;
    real b;

    Color();
    Color(real r, real g, real b);
    Color(std::string inp);
    Color c_sqrt() const;
    Color& operator+=(const Color& other);
    bool isBlack() const;
    Color operator-();
    Color exponent();
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
    real Roughness;

    Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe,
        std::string type = "",
        Color mr = Color(),
        Color ac = Color(1.0,1.0,1.0),
        real refrIndex = 1.0,
        real ai = 0,
        real r = 0.0);
};



struct CVertex
{
    uint32_t id;
    Vertex v;
    Vec3r n;
    Texel t;

    CVertex(uint32_t i, Vertex v, Vec3r n);
    CVertex(uint32_t i, const std::string& inp);
};



#endif //DATATYPES_H
