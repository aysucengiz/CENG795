// TODO: makefilea ekle
////////////////////////////////////////////////
/////////////// Light /////////////////////
////////////////////////////////////////////////

#include "Light.h"
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"
Light::Light(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}

Color Light::getIrradianceAt(Vec3r n_surf,  std::array<real, 2> sample, Ray& shadow_ray, real dist)
{
    Vec3r wi = shadow_ray.dir.normalize();
    real cos_theta = dot_product(wi, n_surf.normalize());
    if (cos_theta <= 0) return Color(0.0,0.0,0.0);
    return Intensity / (dist*dist);
}

Vertex Light::getPos(std::array<real, 2> sample)
{
    return Position;
}

LightType Light::getLightType() {return LightType::POINT;}


////////////////////////////////////////////////
/////////////// AreaLight /////////////////////
////////////////////////////////////////////////

LightType AreaLight::getLightType() {return LightType::AREA;}

Vertex AreaLight::getPos(std::array<real,2> sample)
{
    return Position + (v*(sample[0]-0.5) + u*(sample[1]-0.5)) * size;
}

AreaLight::AreaLight(uint32_t id, Vertex pos, Color intens, Vec3r n, real s) : Light(id, pos, intens), n(n.normalize()), size(s),A(s*s)
{
    std::pair<Vec3r,Vec3r> u_v = getONB(n);
    u = u_v.first.normalize();
    v = u_v.second.normalize();
    // std::cout << "u: " <<u << std::endl;
    // std::cout << "v: " << v << std::endl;
}

Color AreaLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist)
{
    Vec3r wi = shadow_ray.dir.normalize();
    real cos_light = dot_product(n,wi);
    if (cos_light <= 0) cos_light = -cos_light;
    return  Intensity * A* cos_light / (dist*dist);
}


////////////////////////////////////////////////
///////////// DirectionalLight /////////////////
////////////////////////////////////////////////

LightType DirectionalLight::getLightType() {return LightType::DIRECTIONAL;}

DirectionalLight::DirectionalLight(uint32_t id, Color intens, Vec3r d): Light(id,Vertex(0.0,0.0,0.0),intens), dir(d.normalize())
{
    Position = -Vertex(dir.i, dir.j, dir.k);
}

Color DirectionalLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist)
{
    return Intensity ;//* dot_product(shadow_ray.dir.normalize(),n_surf.normalize());
}

Vertex DirectionalLight::getPos(std::array<real, 2> sample)
{
    return Position;
}

////////////////////////////////////////////////
///////////////// SpotLight ////////////////////
////////////////////////////////////////////////

SpotLight::SpotLight(uint32_t id, Vertex pos, Color intens, Vec3r d, real ca, real foa) : Light(id,pos,intens), dir(d), coverageAngle(ca * M_PI/360.0), fallOffAngle(foa * M_PI / 360.0)
{}

LightType SpotLight::getLightType() {return LightType::SPOT;}

Color SpotLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist)
{
    real angle = abs(dot_product(-shadow_ray.dir.normalize(),n_surf.normalize()));
    if (angle < coverageAngle) // L1
    {
        real f = 1.0;
        if (angle < coverageAngle) // L2
        {
            real cos_alpha = cos(coverageAngle);
            f = (cos(angle) - cos_alpha) / (cos(fallOffAngle) - cos_alpha);
        }
        return Intensity/(dist*dist)*f;
    }
    else // L3
    {
        return Color(0.0,0.0,0.0);
    }
}