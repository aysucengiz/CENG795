//
// Created by Aysu on 04-Oct-25.
//


#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include "SceneData.h"
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


Camera::Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname)
                : _id(id), Position(pos), nearDistance(nd), ImageName(imname), width(width), height(height)
{
    Gaze = g.normalize();
    Up = x_product(-Gaze,x_product(u.normalize(),-Gaze)).normalize();

    l = locs[0];
    r = locs[1];
    b = locs[2];
    t = locs[3];
}


////////////////////////////////////////////////
/////////////// PointLight /////////////////////
////////////////////////////////////////////////

PointLight::PointLight(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}

Color PointLight::getIrradianceAt(Vertex v)
{
    Vec3r vec = v - Position;
    return Intensity / dot_product(vec, vec);
}

AreaLight::AreaLight(uint32_t id, Vertex pos, Color intens, Vec3r n, real s) : PointLight(id, pos, intens), n(n), size(s),A(s*s){}

Color AreaLight::getIrradianceAt(Vertex v)
{ // TODO: sampling
    Vec3r vec = v - Position;
 return Intensity *A * dot_product(n,vec.normalize()) / dot_product(vec, vec);
}



////////////////////////////////////////////////
///////////////// MATERIAL /////////////////////
////////////////////////////////////////////////

Material::Material(uint32_t id, Color ar, Color dr, Color sr, uint32_t pe,
        std::string type, Color mr, Color ac, real refrIndex, real ai)
        : _id(id),
        AmbientReflectance(ar),
        DiffuseReflectance(dr),
        SpecularReflectance(sr),
        PhongExponent(pe),
        MirrorReflectance(mr),
        AbsorptionCoefficient(ac),
        RefractionIndex(refrIndex),
        AbsorptionIndex(ai)
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



