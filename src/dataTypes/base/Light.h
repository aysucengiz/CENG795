
class PointLight{
public:
    virtual ~PointLight() = default;
    uint32_t _id;
    Vertex Position;
    Color Intensity;

    PointLight(uint32_t id, Vertex pos, Color intens);
    virtual LightType getLightType();
    virtual Color getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist);
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
    Color getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist) override;
    LightType getLightType() override;
    Vertex getPos(std::array<real, 2> sample) override;
};

class DirectionalLight : public PointLight // TODO: pointlight-> light
{
public:
    Vec3r  dir;
    DirectionalLight(uint32_t id, Color intens, Vec3r d);
    Color getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist) override;
    LightType getLightType() override;
    Vertex getPos(std::array<real, 2> sample) override;
};

class SpotLight : public PointLight // TODO: pointlight-> light
{
public:
    Vec3r  dir;
    real coverageAngle;
    real fallOffAngle;

    DirectionalLight(uint32_t id, Vertex pos, Color intens, Vec3r d, real ca, real foa);
    Color getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist) override;
    LightType getLightType() override;
    Vertex getPos(std::array<real, 2> sample) override;
};



class TextureLight : public PointLight 
{
public:
    uint32_t _id;
    TextureLightType type;
    ImageTexture image;
    Sampler sampler;

    DirectionalLight(uint32_t id, Vertex pos, Color intens, Vec3r d, real ca, real foa);
    Color getIrradianceAt(Vec3r n_surf, std::array<real, 2> sample, Ray& shadow_ray, real dist) override;
    LightType getLightType() override;
    Vertex getPos(std::array<real, 2> sample) override;
};