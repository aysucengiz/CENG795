// TODO: makefilea ekle
////////////////////////////////////////////////
/////////////// PointLight /////////////////////
////////////////////////////////////////////////

PointLight::PointLight(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}

Color PointLight::getIrradianceAt(Vec3r n_surf,  std::array<real, 2> sample, Ray& shadow_ray, real dist)
{
    Vec3r wi = shadow_ray.dir.normalize();
    real cos_theta = dot_product(wi, n_surf.normalize());
    if (cos_theta <= 0) return Color(0.0,0.0,0.0);
    return Intensity / (dist*dist);
}

Vertex PointLight::getPos(std::array<real, 2> sample)
{
    return Position;
}

LightType PointLight::getLightType() {return LightType::POINT;}
LightType AreaLight::getLightType() {return LightType::AREA;}


Vertex AreaLight::getPos(std::array<real,2> sample)
{
    return Position + (v*(sample[0]-0.5) + u*(sample[1]-0.5)) * size;
}



AreaLight::AreaLight(uint32_t id, Vertex pos, Color intens, Vec3r n, real s) : PointLight(id, pos, intens), n(n.normalize()), size(s),A(s*s)
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


