////////////////////////////////////////////////
/////////////// Light /////////////////////
////////////////////////////////////////////////

#include "Light.h"
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"
Light::Light(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}

Color Light::getIrradianceAt(Vec3r n_surf,  std::array<real, 2> sample, Ray& shadow_ray, const Vertex & intersection)
{
    Vec3r wi = shadow_ray.dir.normalize();
    real cos_theta = dot_product(wi, n_surf.normalize());
    if (cos_theta <= 0) return Color(0.0,0.0,0.0);
    Vec3r dist = Position - intersection;
    return Intensity / dot_product(dist,dist);
}


LightType Light::getLightType() {return LightType::POINT;}

Ray Light::compute_shadow_ray(const HitRecord& hit_record,  std::array<real, 2> sample, real shadowRayEpsilon) const
{
    Ray shadow_ray;
    shadow_ray.pos = hit_record.intersection_point + hit_record.normal * shadowRayEpsilon;
    shadow_ray.dir = compute_shadow_ray_dir(shadow_ray.pos, hit_record.normal, sample);
    return shadow_ray;
}

Vec3r Light::compute_shadow_ray_dir(const Vertex &pos, const Vec3r &normal, std::array<real, 2> sample) const
{
    return Position - pos;
}
////////////////////////////////////////////////
/////////////// AreaLight /////////////////////
////////////////////////////////////////////////

LightType AreaLight::getLightType() {return LightType::AREA;}


AreaLight::AreaLight(uint32_t id, Vertex pos, Color intens, Vec3r n, real s) : Light(id, pos, intens), n(n.normalize()), size(s),A(s*s)
{
    std::pair<Vec3r,Vec3r> u_v = getONB(n);
    u = u_v.first.normalize();
    v = u_v.second.normalize();
    // std::cout << "u: " <<u << std::endl;
    // std::cout << "v: " << v << std::endl;
}

Color AreaLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, const Vertex& intersection)
{
    Vec3r wi = shadow_ray.dir.normalize();
    real cos_light = dot_product(n,wi);
    if (cos_light <= 0) cos_light = -cos_light;
    Vertex light_pos = Position + (v*(sample[0]-0.5) + u*(sample[1]-0.5)) * size;
    Vec3r dist = light_pos - intersection;
    return  Intensity * A* cos_light / dot_product(dist,dist);
}
Vec3r AreaLight::compute_shadow_ray_dir(const Vertex& pos, const Vec3r& normal, std::array<real, 2> sample) const
{
    Vertex light_pos = Position + (v*(sample[0]-0.5) + u*(sample[1]-0.5)) * size;
    return light_pos - pos;
}


////////////////////////////////////////////////
///////////// DirectionalLight /////////////////
////////////////////////////////////////////////

LightType DirectionalLight::getLightType() {return LightType::DIRECTIONAL;}

DirectionalLight::DirectionalLight(uint32_t id, Color intens, Vec3r d): Light(id,Vertex(0.0,0.0,0.0),intens), dir(d.normalize())
{
    Position = -Vertex(dir.i, dir.j, dir.k);
}

Color DirectionalLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, const Vertex& intersection)
{
    return Intensity ;
}
Vec3r DirectionalLight::compute_shadow_ray_dir(const Vertex& pos, const Vec3r& normal, std::array<real, 2> sample) const
{
    return -dir;
}

////////////////////////////////////////////////
///////////////// SpotLight ////////////////////
////////////////////////////////////////////////

SpotLight::SpotLight(uint32_t id, Vertex pos, Color intens, Vec3r d, real ca, real foa) : Light(id,pos,intens), dir(d.normalize()), coverageAngle(ca * M_PI/180.0), fallOffAngle(foa * M_PI / 180.0)
{
    cos_alpha = cos(coverageAngle*0.5);
    cos_beta = cos(fallOffAngle*0.5);
}

LightType SpotLight::getLightType() {return LightType::SPOT;}

Color SpotLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, const Vertex& intersection)
{
    Vec3r normalized_shadow = shadow_ray.dir.normalize();
    real cos_theta = dot_product(-normalized_shadow, dir);
    Vec3r temp = (Position - intersection);
    real dist = dot_product(temp, temp);
    if (cos_theta > cos_alpha)
    {
        real f;
        if (cos_theta > cos_beta) // L1
        {
            f = 1.0;
        }
        else // L2
        {
            f = pow((cos_theta - cos_alpha) / (cos_beta- cos_alpha),4);
        }
        return Intensity*f/dist;
    }
    else // L3
    {
        return Color(0.0,0.0,0.0);
    }
}

////////////////////////////////////////////////
///////////////// TextureLight ////////////////////
////////////////////////////////////////////////

TextureLight::TextureLight(uint32_t id, Image* im, Sampler s, TextureLightType type) :
    texture(id, DecalMode::LIGHT, im, Interpolation::NEAREST, 1.0),
    Light(id, Vertex(0.0,0.0,0.0), Color(0.0,0.0,0.0)),
    sampler(s), type(type)
{
}

LightType TextureLight::getLightType() {return LightType::TEXTURE;}

Color TextureLight::getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, const Vertex& intersection)
{
    Texel tex = getTexel(shadow_ray.dir);
    Vertex v(0.0,0.0,0.0);
    Color intensity = texture.TextureColor(v,tex,0);
    real p_d;
    switch (sampler)
    {
    default:
    case Sampler::UNIFORM:
        p_d = 1/( 2 * M_PI);
        break;
    case Sampler::COSINE:
        {
            real theta = dot_product(n_surf.normalize(),shadow_ray.dir.normalize());
            p_d  = cos(theta)/M_PI;
        }
        break;
    }

    intensity = intensity / p_d;
    return intensity;
}

Vec3r TextureLight::getRandomVec(const Vec3r &norm) const
{
    Vec3r vec = Vec3r(1.0,1.0,1.0);
    while (vec.mag() > 1.0 || dot_product(norm,vec) < 0.0)
    {
        real e1 = getRandom()*2-1.0;
        real e2 = getRandom()*2-1.0;
        real e3 = getRandom()*2-1.0;
        vec = Vec3r(e1,e2,e3);
    }
    return vec;
}
Texel TextureLight::getTexel(const Vec3r &vec) const
{
    Texel tex(0.0,0.0);
    switch (type)
    {
    case TextureLightType::PROBE:
        {
            real r = acos(-vec.k)/(sqrt(vec.i*vec.i + vec.j*vec.j)*M_PI);
            tex.u = (r*vec.i + 1.0) * 0.5;
            tex.v = (-r*vec.j + 1.0) * 0.5;
        }
        break;
    case TextureLightType::LATLONG:
        tex.u = (1.0 + atan2(vec.i,-vec.k)/M_PI) * 0.5;
        tex.v = acos(vec.j)/M_PI;
        break;

    }
    return tex;
}

Vec3r TextureLight::compute_shadow_ray_dir(const Vertex &pos, const Vec3r &normal, std::array<real, 2> sample) const
{
    Vec3r vec = getRandomVec(normal);
    return vec;
}
