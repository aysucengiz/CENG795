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


struct Camera{
    uint32_t _id;
    Vertex Position;
    Vec3r Gaze;
    Vec3r Up;
    Vec3r V;
    real l;
    real r;
    real b;
    real t;
    real nearDistance;
    uint32_t width;
    uint32_t height;
    std::string ImageName;
    uint32_t numSamples;
    real FocusDistance;
    real ApertureSize;
    std::vector<std::array<real, 2>> samplesCamera;
    std::vector<std::array<real, 2>> samplesPixel;

    Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname,
        uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st);


    void initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples);
    Vertex getPos(int i) const;
};

class PointLight{
public:
    virtual ~PointLight() = default;
    uint32_t _id;
    Vertex Position;
    Color Intensity;

    PointLight(uint32_t id, Vertex pos, Color intens);
    virtual LightType getLightType();
    virtual Color getIrradianceAt(Vec3r n, std::array<real, 2> sample, Ray& shadow_ray);
    virtual Vertex getPos(std::array<real, 2> sample);
};

class AreaLight : public PointLight
{
public:
    Vec3r  n;
    real size;
    real A;
    std::vector<Vertex> samples;
    Vec3r u;
    Vec3r v;


    AreaLight(uint32_t id, Vertex pos, Color intens, Vec3r n, real Size);
    Color getIrradianceAt(Vec3r n,  std::array<real, 2> sample, Ray& shadow_ray) override;
    LightType getLightType() override;
    Vertex getPos(std::array<real, 2> sample) override;
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

    CVertex(uint32_t i, Vertex v, Vec3r n);
    CVertex(uint32_t i, const std::string& inp);
};



#endif //DATATYPES_H
