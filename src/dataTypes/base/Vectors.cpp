//
// Created by vicy on 11/08/25.
//

#include "Vectors.h"
#include "../../functions/overloads.h"


////////////////////////////////////////////////
//////////////////// VEC3R /////////////////////
////////////////////////////////////////////////

Vec3r::Vec3r(){i=0.0;j=0.0;k=0.0;}
Vec3r::Vec3r(real i, real j, real k) : i(i), j(j), k(k){}
Vec3r::Vec3r(std::string inp) {
    std::istringstream ss(inp);
    ss >> i >> j >> k;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Vec3r string: " + inp);
    }
}

Vec3r Vec3r::operator-() const {
    return Vec3r{-i, -j, -k};
}

Vec3r& Vec3r::operator+=(const Vec3r& other) {
    i += other.i;
    j += other.j;
    k += other.k;
    return *this;  // Return the modified object
}

Vec3r& Vec3r::operator/=(const uint32_t& other) {
    i /= static_cast<real>(other);
    j /= static_cast<real>(other);
    k /= static_cast<real>(other);
    return *this;  // Return the modified object
}

Vec3r Vec3r::normalize() const{return (*this)/this->mag();}
real Vec3r::mag() const { return sqrt(i*i + j*j + k*k);}


////////////////////////////////////////////////
/////////////////// VERTEX /////////////////////
////////////////////////////////////////////////


Vertex::Vertex(){x=0.0; y=0.0; z=0.0;}
Vertex::Vertex(real x,real y,real z) : x(x), y(y), z(z){}
Vertex::Vertex(std::string inp) {
    std::istringstream ss(inp);
    ss >> x >> y >> z;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Vertex string: " + inp);
    }
}

Vertex Vertex::operator-() const{
    return Vertex(-x,-y,-z);
}


////////////////////////////////////////////////
////////////////////  RAY  /////////////////////
////////////////////////////////////////////////


Ray::Ray(){}
Ray::Ray(const Ray &r) : pos(r.pos), dir(r.dir){}
Ray& Ray::operator=(const Ray &r)
{
    if (this != &r) {pos = r.pos; dir = r.dir;}
    return *this;
}

Ray::Ray(Vertex p, Vec3r d) : pos(p), dir(d){}

Vec3r x_product(const Vec3r &v, const Vec3r &w);

////////////////////////////////////////////////
//////////////////// VEC4R /////////////////////
////////////////////////////////////////////////

Vec4r::Vec4r(real x, real y, real z, real w) : x(x), y(y), z(z), w(w) {}
Vec4r::Vec4r(Vec3r v) : x(v.i), y(v.j), z(v.k), w(0.0) {}
Vec4r::Vec4r(Vertex v) : x(v.x), y(v.y), z(v.z), w(1.0) {}

Vec4r Vec4r::operator-() const {
    return Vec4r{-x, -y, -z, -w};
}

Vec4r& Vec4r::operator+=(const Vec3r& other) {
    x += other.i;
    y += other.j;
    z += other.k;
    w += 0.0;
    return *this;  // Return the modified object
}

Vec4r& Vec4r::operator+=(const Vertex& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += 1.0;
    return *this;  // Return the modified object
}

Vec4r& Vec4r::operator+=(const Vec4r& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    w += other.w;
    return *this;  // Return the modified object
}

Vec4r& Vec4r::operator/=(const uint32_t& other) {
    x /= static_cast<real>(other);
    y /= static_cast<real>(other);
    z /= static_cast<real>(other);
    w /= static_cast<real>(other);
    return *this;  // Return the modified object
}

Vec4r&  Vec4r::operator/=(const real& other) {
    x /= other;
    y /= other;
    z /= other;
    w /= other;
    return *this;  // Return the modified object
}



Vec3r Vec4r::getVec3r() const{return Vec3r(x,y,z);}
Vertex Vec4r::getVertex() const{return Vertex(x,y,z);}

real Vertex::operator[](Axes a) const
{
    switch (a)
    {
        case Axes::x: return x;
        case Axes::y: return y;
        case Axes::z: return z;
        default: throw std::invalid_argument("Invalid Vertex string: " + std::to_string((int) a));
    }
}

real Vec3r::operator[](Axes a) const
{
    switch (a)
    {
    case Axes::x: return i;
    case Axes::y: return j;
    case Axes::z: return k;
    default: throw std::invalid_argument("Invalid Vertex string: " + std::to_string((int) a));
    }
}



