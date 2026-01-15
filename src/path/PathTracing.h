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
    uint32_t id;
    virtual ~BRDF() = default;
    // precomputed coefficients for modified normalized
    static constexpr real one_8_pi = 1.0 / (8.0 * M_PI);
    static constexpr real one_2_pi = 1.0 / (2.0 * M_PI);
    static constexpr real one_pi = 1.0 / M_PI;

    // original blinn phong by default
    real exponent;
    virtual Color Guards_BRDF_This_Man(Color& kd, Color& ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const = 0;
    real getCosTheta(const Vec3r& normal, const Vec3r& shadow_ray_dir) const;
    real getCosAlphaH(const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const;
    real getCosAlphaR(const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir, real cos_theta) const;


};

struct Phong : public BRDF
{
    Phong(uint32_t _id, bool blinn, bool modified, bool normalized, real ex) : blinn(blinn), modified(modified), normalized(normalized) {exponent = ex; id = _id;}
    bool blinn = false;
    bool modified = false;
    bool normalized = false;
    Color Guards_BRDF_This_Man(Color& kd, Color& ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const override;
};

struct BRDF_TorranceSparrow : public BRDF
{
    BRDF_TorranceSparrow(uint32_t _id, bool kd_f, real ex) : kd_fresnel(kd_f) {exponent = ex; id = _id;}
    bool kd_fresnel;
    Color Guards_BRDF_This_Man(Color& kd, Color& ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const override;

};


// TODO: object lights ve area lights handling
struct PathTracer
{
    uint32_t splitting_factor = 1; // TODO: eklendi, renklerin nasıl toplanacağı ayarlanacak
    bool importance_sampling = false; // eklendi
    bool NEE = false; // eklendi
    bool MIS_BALANCE = false; // eklendi
    bool RussianRoulette = false; // eklendi

    Vec3r getBouncedRayDir(real a, real b, const Vec3r& normal);
    real PDF(real a);
};



#endif //CENG795_PATHTRACING_H