//
// Created by vicy on 01/12/26.
//

#ifndef CENG795_PATHTRACING_H
#define CENG795_PATHTRACING_H
#include <cstdint>
#include "../typedefs.h"
#include "../dataTypes/base/SceneData.h"

struct BRDF
{
    // precomputed coefficients for modified normalized
    static constexpr real one_8_pi = 1.0 / (8.0 * M_PI);
    static constexpr real one_2_pi = 1.0 / (2.0 * M_PI);
    static constexpr real one_pi = 1.0 / M_PI;

    // original blinn phong by default
    real exponent;
    virtual Color Guards_BRDF_This_Man(Color kd, Color ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const = 0;
    real getCosTheta(const Vec3r& normal, const Vec3r& shadow_ray_dir) const;
    real getCosAlphaH(const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const;
    real getCosAlphaR(const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir, real cos_theta) const;


};

struct Phong : public BRDF
{
    bool blinn;
    bool modified;
    bool normalized;
    virtual Color Guards_BRDF_This_Man(Color kd, Color ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const;
};

struct BRDF_TorranceSparrow : public BRDF
{
    bool kd_fresnel;
    virtual Color Guards_BRDF_This_Man(Color kd, Color ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const;

};



struct PathTracer
{
    uint32_t splitting_factor = 1;
    bool importance_sampling = false;
    bool NEE = false;
    bool MIS_BALANCE = false;
    bool RussianRoulette = false;

    Vec3r getBouncedRayDir(real a, real b);
    real PDF(real a);
};



#endif //CENG795_PATHTRACING_H