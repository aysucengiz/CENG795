//
// Created by Aysu on 04-Oct-25.
//


#include <sstream>
#include <string>
#include <vector>
#include <cmath>
#include <iostream>
#include "SceneData.h"


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

bool Color::isWhite() const
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


Camera::Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::string locs, real nd, std::string res, std::string imname)
                : _id(id), Position(pos), nearDistance(nd), ImageName(imname)
{
    Gaze = g.normalize();
    Up = x_product(-Gaze,x_product(u.normalize(),-Gaze)).normalize();
    std::istringstream s1(locs);
    s1 >> l >> r >> b >> t;
    std::istringstream s2(res);
    s2 >> width >> height;
}


////////////////////////////////////////////////
/////////////// PointLight /////////////////////
////////////////////////////////////////////////

PointLight::PointLight(uint32_t id, Vertex pos, Color intens) : _id(id), Position(pos), Intensity(intens) {}


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
    else if (ar.isWhite() && dr.isWhite()&& sr.isWhite()&& mr.isWhite() && ac.isWhite()) materialType = MaterialType::NONE;
    else materialType = MaterialType::NORMAL;
}


////////////////////////////////////////////////
////////////////// CVERTEX /////////////////////
////////////////////////////////////////////////


CVertex::CVertex(uint32_t i, real x,real y,real z) : v(Vertex(x,y,z)), n(Vec3r()), id(i){}
CVertex::CVertex(uint32_t i, const std::string& inp) : n(Vec3r()), id(i)
{
    std::istringstream ss(inp);
    ss >> v.x >> v.y >> v.z;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Vertex string: " + inp);
    }
}



