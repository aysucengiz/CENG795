//
// Created by vicy on 01/12/26.
//

#include "PathTracing.h"

real BRDF::getCosTheta(const Vec3r& normal, const Vec3r& shadow_ray_dir) const
{
    real cos_theta = dot_product(shadow_ray_dir.normalize(), normal);
    return cos_theta;
}

real BRDF::getCosAlphaH(const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const
{
    Vec3r h = (shadow_ray_dir.normalize() - ray_dir.normalize()).normalize();
    real cos_alpha = dot_product(normal, h);
    return cos_alpha;
}

real BRDF::getCosAlphaR(const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir, real cos_theta) const
{
    Vec3r reflected_ray_dir = ray_dir + normal * 2 * cos_theta;
    real cos_alpha = dot_product(shadow_ray_dir, reflected_ray_dir);
    return cos_alpha;
}


Color Phong::Guards_BRDF_This_Man(Color kd, Color ks, real phong, real refr, const Vec3r& normal, const Vec3r& ray_dir, const Vec3r& shadow_ray_dir) const
{


    real cos_theta = getCosTheta(normal,ray_dir);
    if (cos_theta <= 0) return Color(0.0, 0.0, 0.0);

    real cos_alpha;
    if (blinn) cos_alpha = getCosAlphaH(normal, ray_dir, shadow_ray_dir);
    else       cos_alpha = getCosAlphaR(normal, ray_dir, shadow_ray_dir);

    Color f;
    if(!modified)           f = kd + ks * (pow(cos_alpha, phong)/ cos_theta);
    else if(!normalized)    f = kd + ks * (pow(cos_alpha, phong));
    else
    {
        real kd_coef = one_pi;
        real ks_coef;
        if (blinn) ks_coef = (phong + 8) * one_8_pi;
        else       ks_coef = (phong + 2) * one_2_pi;
        f = kd * kd_coef + ks * ks_coef * (pow(cos_alpha, phong));
    }
    return f;
}

real getAngle(Vec3r a, Vec3r b)
{
    real cos_theta = dot_product(a.normalize(), b.normalize());
    return acos(cos_theta);
}

Color BRDF_TorranceSparrow::Guards_BRDF_This_Man(Color &kd, Color &ks, real phong,real refr,  const Vec3r& normal, const Vec3r& wi, const Vec3r& wo) const
{
    constexpr real one_pi = 1.0 / M_PI;

    real cos_theta = getCosTheta(normal, wi);
    if (cos_theta <= 0) return Color(0.0, 0.0, 0.0);

    real cos_alpha_h = getCosAlphaH(normal, wi, wo);
    real D = pow(cos_alpha_h, phong) * (phong + 2) * one_2_pi;

    // get G
    Vec3r h = (wo.normalize() - wi.normalize()).normalize();
    real n_h = getAngle(normal,h);
    real n_o = getAngle(normal,wo);
    real n_i = getAngle(normal,wi);
    real o_h = getAngle(wo,h);

    real angle1 = 2 * n_h * n_o / o_h;
    real angle2 = 2 * n_h * n_i / o_h;
    real G = std::min(1.0, std::min(angle1, angle2));

    // get F
    real R0 = pow(refr - 1,2) / pow(refr + 1,2);
    real F = R0 + (1 - R0) * pow((1 - cos(o_h)),5);

    // combine
    real f;
    if(kd_fresnel)  f = kd * one_pi * (1-F)+ ks * D * F * G / (4 * cos_theta * cos_phi);
    real cos_phi =cos(); // TODO: ne bu
    else f = kd * one_pi + ks * D * F * G / (4 * cos_theta * cos_phi);
}


Vec3r PathTracer::getBouncedRayDir(real a, real b)
{
    constexpr pi2 = 2.0 * M_PI;
    // TODO: random sampling functions go here

    if(importance_sampling)
    {
        real theta = acos(a);
        real phi = pi2 * b;
        Vec3r w = Vec3r(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        return w;
    }
    else
    {
        real theta = asin(a);
        real phi = pi2 * b;
        Vec3r w = Vec3r(sin(theta) * cos(phi), sin(theta) * sin(phi), cos(theta));
        return w;
    }

}


Vec3r PathTracer::PDF(real a)
{
    constexpr one_pi = 1/ M_PI;
    constexpr one_pi2 = 1/(2*M_PI);

    if(importance_sampling) return a * one_pi;
    else                    return one_pi2;
}