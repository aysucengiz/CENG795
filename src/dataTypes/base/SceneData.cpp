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


CVertex::CVertex(uint32_t i, Vertex v, Vec3r n) : v(v), n(n),  id(i), t(0.0,0.0){}
CVertex::CVertex(uint32_t i, const std::string& inp) : n(Vec3r()), id(i), t(0.0,0.0)
{
    std::istringstream ss(inp);
    ss >> v.x >> v.y >> v.z;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Vertex string: " + inp);
    }
}



