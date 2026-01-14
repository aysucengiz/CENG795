#ifndef CENG795_LIGHT_H
#define CENG795_LIGHT_H

#include "../texture/TextureMap.h"
#include "../object/Object.h"

class Light{
public:
    virtual ~Light() = default;
    uint32_t _id;
    Vertex Position;
    Color Intensity;

    Light(uint32_t id, Vertex pos, Color intens);
    virtual LightType getLightType();
    virtual Color getIrradianceAt(Vec3r n_surf, Ray& shadow_ray, const Vertex& intersection);
    virtual Vec3r compute_shadow_ray_dir(const Vertex& pos, const Vec3r& normal, std::array<real, 2> sample) const;
    Ray compute_shadow_ray(const HitRecord& hit_record,  std::array<real, 2> sample, real shadowRayEpsilon) const;
};

class AreaLight : public Light
{
public:
    Vec3r  n;
    real size;
    real A;
    std::vector<Vertex> samples;
    Vec3r u;
    Vec3r v;


    AreaLight(uint32_t id, Vertex pos, Color intens, Vec3r n, real Size);
    Color getIrradianceAt(Vec3r n_surf, Ray& shadow_ray, const Vertex& intersection) override;
    LightType getLightType() override;
    Vec3r compute_shadow_ray_dir(const Vertex& pos, const Vec3r& normal, std::array<real, 2> sample) const override;
};
class DirectionalLight : public Light
{
public:
    Vec3r  dir;
    DirectionalLight(uint32_t id, Color intens, Vec3r d);
    Color getIrradianceAt(Vec3r n_surf, Ray& shadow_ray, const Vertex& intersection) override;
    LightType getLightType() override;
    Vec3r compute_shadow_ray_dir(const Vertex& pos, const Vec3r& normal, std::array<real, 2> sample) const override;
};

class SpotLight : public Light
{
public:
    Vec3r  dir;
    real coverageAngle;
    real fallOffAngle;
    real cos_alpha;
    real cos_beta;

    SpotLight(uint32_t id, Vertex pos, Color intens, Vec3r d, real ca, real foa);
    Color getIrradianceAt(Vec3r n_surf, Ray& shadow_ray, const Vertex& intersection) override;
    LightType getLightType() override;
};



class TextureLight : public Light
{
public:
    ImageTexture texture;
    TextureLightType type;
    Sampler sampler;

    TextureLight(uint32_t id, Image* im, Sampler s, TextureLightType type);
    Color getIrradianceAt(Vec3r n_surf, Ray& shadow_ray, const Vertex& intersection) override;
    LightType getLightType() override;
    Vec3r compute_shadow_ray_dir(const Vertex& pos, const Vec3r& normal, std::array<real, 2> sample) const override;
    Vec3r getRandomVec(const Vec3r& norm) const;
    Texel getTexel(const Vec3r& vec) const;
};



class AreaLightObject : public AreaLight, public Object
{
    bool isLuminous() const override {return true;}
    intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const override;
};


class ObjectLight : public Light, public Instance
{
    ObjectLight(uint32_t i, Color r, Object* o);

    bool isLuminous() const override {return true;}
    intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const override;
    Object *object;
};

#endif // CENG795_LIGHT_H