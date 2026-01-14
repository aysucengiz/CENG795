//
// Created by vicy on 01/14/26.
//

#ifndef CENG795_MATERIAL_H
#define CENG795_MATERIAL_H


#include "../../path/PathTracing.h"
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
    bool degamma;
    BRDF *brdf;

    Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe,
        std::string type,
        Color mr,
        Color ac ,
        real refrIndex ,
        real ai ,
        real r ,
        std::string degam,
        BRDF* brdf);
};


#endif //CENG795_MATERIAL_H