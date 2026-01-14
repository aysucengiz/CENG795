//
// Created by vicy on 01/14/26.
//
#include "Material.h"
////////////////////////////////////////////////
///////////////// MATERIAL /////////////////////
////////////////////////////////////////////////

Material::Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe,
        std::string type, Color mr, Color ac, real refrIndex, real ai, real r, std::string degam, BRDF* brd)
        : _id(id),
        AmbientReflectance(ar),
        DiffuseReflectance(dr),
        SpecularReflectance(sr),
        PhongExponent(pe),
        MirrorReflectance(mr),
        AbsorptionCoefficient(ac),
        RefractionIndex(refrIndex),
        AbsorptionIndex(ai),
        Roughness(r),
        degamma(degam == "true"),
        brdf(brd)
{
    if (type == "dielectric") materialType = MaterialType::DIELECTRIC;
    else if (type == "conductor") materialType = MaterialType::CONDUCTOR;
    else if (type == "mirror") materialType = MaterialType::MIRROR;
    else if (ar.isBlack() && dr.isBlack()&& sr.isBlack()&& mr.isBlack() && ac.isBlack()) materialType = MaterialType::NONE;
    else materialType = MaterialType::NORMAL;
}
