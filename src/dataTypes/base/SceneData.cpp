//
// Created by Aysu on 04-Oct-25.
//


#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include "SceneData.h"

#include <random>


#include "../../functions/helpers.h"
#include "../../functions/overloads.h"
#include "../matrix/Matrix.h"


////////////////////////////////////////////////
//////////////////// COLOR /////////////////////
////////////////////////////////////////////////


Color::Color(){r = 0; g = 0; b = 0;}
Color::Color(real r, real g, real b) : r(r), g(g), b(b) {}
Color::Color(std::string inp) {
    std::istringstream ss(inp);
    ss >> r >> g >> b;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid color string: " + inp);
    }
}

Color Color::c_sqrt() const
{
    return Color(sqrt(r),sqrt(g),sqrt(b));
}
Color& Color::operator+=(const Color& other) {
    r += other.r;
    g += other.g;
    b += other.b;
    return *this;  // Return the modified object
}

bool Color::isBlack() const
{
    if(r == 0.0 && g == 0.0 && b == 0.0) return true;
    return false;

}

Color Color::operator-()
{
    return Color(-r,-g,-b);
}


Color Color::exponent()
{
    return {std::exp(r), std::exp(g), std::exp(b)};
}


////////////////////////////////////////////////
/////////////////// CAMERA /////////////////////
////////////////////////////////////////////////


Camera::Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname,
    uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st)
    : _id(id), Position(pos), nearDistance(nd), ImageName(imname), width(width), height(height),
      numSamples(numSamples), FocusDistance(focusDistance), ApertureSize(apertureSize)
{
    Gaze = g.normalize();
    V = x_product(u.normalize(), -Gaze).normalize();
    Up = x_product(-Gaze, V).normalize();

    l = locs[0];
    r = locs[1];
    b = locs[2];
    t = locs[3];
    initializeSamples(st, samplesPixel);
    initializeSamples(st, samplesCamera);
    initializeSamples(st, samplesLight);
    initializeSamples(st, samplesGlossy);
    initializeSamples2D(st, samplesTime);
}


std::mt19937 gRandomGeneratorC;
void Camera::initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples)
{
    samples.clear();
    if (numSamples == 1)
    {
        samples.push_back({0.5,0.5});
        return;
    }

    samples.reserve(numSamples);
    std::pair<int,int> row_col = closestFactors(numSamples);
    switch (st)
    {
    case SamplingType::UNIFORM:
        {
            real spacing_x = 1.0 / (row_col.first+1);
            real spacing_y = 1.0 / (row_col.second+1);
            for (int y=0; y < row_col.second; y++)
                for (int x=0; x < row_col.first; x++)
                    samples.push_back({x*spacing_x, y*spacing_y});
        }break;
    case SamplingType::STRATIFIED:
        {
            // std::cout << row_col.first << " " << row_col.second << std::endl;
            real spacing_x = 1.0 / real(row_col.first);
            real spacing_y = 1.0 / real(row_col.second);
            for (int y=0; y < row_col.second; y++)
                for (int x=0; x < row_col.first; x++)
                    samples.push_back({(x+getRandom())*spacing_x, (y+getRandom())*spacing_y});
        }
        break;
    case SamplingType::N_ROOKS:
        {
            std::vector<int> cols(numSamples);
            for (int i = 0; i < numSamples; i++) cols[i] = i;
            std::shuffle(cols.begin(), cols.end(), gRandomGeneratorC);

            real spacing = 1.0 / real(numSamples);
            for (int i = 0; i < numSamples; i++)
                    samples.push_back({(cols[i]+getRandom())*spacing, (i+getRandom())*spacing});
        }
        break;
    case SamplingType::MULTI_JITTERED:
        {
            std::vector<int> cols(numSamples);
            for (int i = 0; i < numSamples; i++) cols[i] = i;
            std::shuffle(cols.begin(), cols.end(), gRandomGeneratorC);

            std::vector<int> rows(numSamples);
            for (int i = 0; i <numSamples; i++) rows[i] = i;
            std::shuffle(rows.begin(), rows.end(), gRandomGeneratorC);

            real spacing = 1.0 / real(numSamples);
            for (int i=0; i < numSamples; i++)
                    samples.push_back({(rows[i]+getRandom())*spacing, (cols[i]+getRandom())*spacing});
        }
        break;
    case SamplingType::RANDOM:
    default:
        samples.reserve(numSamples);
        for (int i = 0; i < numSamples; i++) samples.push_back({getRandom(),getRandom()});
        break;
    }

    // for (auto sample : samples) std::cout << "sample: "<< sample[0] << " " << sample[1]  << std::endl;



}


void Camera::initializeSamples2D(SamplingType st, std::vector<real> &samples)
{
    samples.clear();
    if (numSamples == 1)
    {
        samples.push_back(0.5);
        return;
    }

    samples.reserve(numSamples);
    switch (st)
    {
    case SamplingType::UNIFORM:
        {
            real spacing = 1.0 / (numSamples+1);
            for (int x=0; x <numSamples; x++)
               samples.push_back((x+1.0)*spacing);
        }break;
    case SamplingType::STRATIFIED:
    case SamplingType::N_ROOKS:
    case SamplingType::MULTI_JITTERED:
        {

            real spacing = 1.0 / real(numSamples);
            for (int x=0; x <numSamples; x++)
                samples.push_back((x+getRandom())*spacing);
        }
        break;
    case SamplingType::RANDOM:
    default:
        samples.reserve(numSamples);
        for (int i = 0; i < numSamples; i++) samples.push_back(getRandom());
        break;
    }

    // for (auto sample : samples) std::cout << "sample: "<< sample[0] << " " << sample[1]  << std::endl;




}

Vertex Camera::getPos(int i) const
{
    if (ApertureSize > 0)  return Position + (Up *(samplesCamera[i][0]-0.5) + V * (samplesCamera[i][1]-0.5))*ApertureSize;
    else                   return Position;
}

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
    real c = std::min(fabs(n.i), std::min(fabs(n.j),fabs(n.k)));
    if      (c==fabs(n.i)){ u.i = 0; u.j = -n.k; u.k = n.j;}
    else if (c==fabs(n.j)){ u.j = 0; u.i = -n.k; u.k = n.i;}
    else if (c==fabs(n.k)){ u.k = 0; u.j = -n.i; u.i = n.j;}

    // std::cout << "n: " <<n << std::endl;
    u = u.normalize();
    v = x_product(n,u).normalize();
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
///////////////// MATERIAL /////////////////////
////////////////////////////////////////////////

Material::Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe,
        std::string type, Color mr, Color ac, real refrIndex, real ai, real r)
        : _id(id),
        AmbientReflectance(ar),
        DiffuseReflectance(dr),
        SpecularReflectance(sr),
        PhongExponent(pe),
        MirrorReflectance(mr),
        AbsorptionCoefficient(ac),
        RefractionIndex(refrIndex),
        AbsorptionIndex(ai),
        Roughness(r)
{
    if (type == "dielectric") materialType = MaterialType::DIELECTRIC;
    else if (type == "conductor") materialType = MaterialType::CONDUCTOR;
    else if (type == "mirror") materialType = MaterialType::MIRROR;
    else if (ar.isBlack() && dr.isBlack()&& sr.isBlack()&& mr.isBlack() && ac.isBlack()) materialType = MaterialType::NONE;
    else materialType = MaterialType::NORMAL;
}


////////////////////////////////////////////////
////////////////// CVERTEX /////////////////////
////////////////////////////////////////////////


CVertex::CVertex(uint32_t i, Vertex v, Vec3r n) : v(v), n(n),  id(i){}
CVertex::CVertex(uint32_t i, const std::string& inp) : n(Vec3r()), id(i)
{
    std::istringstream ss(inp);
    ss >> v.x >> v.y >> v.z;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Vertex string: " + inp);
    }
}



