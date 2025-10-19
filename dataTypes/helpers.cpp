//
// Created by vicy on 10/18/25.
//

#include "helpers.h"


Vec3r Vec3r::normalize(){return (*this)/this->mag();}



int clamp(real c, int from, int to){
    if(c > to) return to;
    if(c< from) return from;
    return (int) c;
}

// overload - color

Color operator *(const Color &a, const Color &b)
{
    return Color(a.r * b.r,
                 a.g * b.g,
                 a.b * b.b);
}

Color operator +(const Color &a, const Color &b)
{
    return Color(a.r + b.r,
                 a.g + b.g,
                 a.b + b.b);
}

Color operator -(const Color &a, const Color &b)
{
    return Color(a.r - b.r,
                 a.g - b.g,
                 a.b - b.b);
}


Color operator /(const Color &a, const real b)
{
    return Color(a.r / b,
                a.g / b,
                   a.b / b);
}

Color operator *(const Color &a, const real b)
{
    return Color(a.r * b,
                a.g * b,
                   a.b * b);
}




// overload - return vertex

Vertex operator +(const Vec3r &a, const Vertex &b)
{
    return Vertex(a.i + b.x,
                  a.j + b.y,
                  a.k + b.z);
}

Vertex operator +(const Vertex &a, const Vec3r &b)
{
    return Vertex(a.x + b.i,
                  a.y + b.j,
                  a.z + b.k);
}

Vertex operator -(const Vertex &a, const Vec3r &b)
{
    return Vertex(a.x - b.i,
                  a.y - b.j,
                  a.z - b.k);
}

Vertex operator -(const Vec3r &a, const Vertex &b){
    return Vertex(a.i - b.x,
                  a.j - b.y,
                  a.k - b.z);
}

// overload - return vector

Vec3r operator +(const Vec3r &a, const Vec3r &b)
{
    return Vec3r(a.i + b.i,
                 a.j + b.j,
                 a.k + b.k);
}

Vec3r operator -(const Vec3r &a, const Vec3r &b)
{
    return Vec3r(a.i - b.i,
                 a.j - b.j,
                 a.k - b.k);
}

Vec3r operator -(const Vertex &a, const Vertex &b)
{
    return Vec3r(a.x - b.x,
                 a.y - b.y,
                 a.z - b.z);
}

Vec3r operator /(const Vec3r &a, const real denum)
{
    return Vec3r(a.i / denum,
                 a.j / denum,
                 a.k / denum);
}

Vec3r operator *(const Vec3r &a, real mult)
{
    return Vec3r(a.i * mult,
                 a.j * mult,
                 a.k * mult);
}

Vec3r x_product(const Vec3r &v, const Vec3r &w)
{
    return Vec3r(v.j * w.k - v.k * w.j,
                 v.k * w.i - v.i * w.k,
                 v.i * w.j - v.j * w.i);
}

real dot_product(const Vec3r &a, const Vec3r &b){
    return a.i*b.i + a.j*b.j + a.k*b.k;
}

real determinant(const Vec3r &first, const Vec3r &second, const Vec3r &third ){
    //  a  d  g
    //  b  e  h
    //  c  f  i
    // return a*(e*i - h*f) + b*(g*f - d*i) + c*(d*h -e*g);
    return first.i*(second.j*third.k - third.j*second.k) + first.j*(third.i*second.k - second.i*third.k) + first.k*(second.i*third.j -second.j*third.i);
}



std::ostream& operator<<(std::ostream& os, const Color& c) {
    os << c.r << " " << c.g << " " << c.b;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vec3r& v) {
    os << v.i << " " << v.j << " " << v.k;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
    os << v.x << " " << v.y << " " << v.z;
    return os;
}

std::ostream& operator<<(std::ostream& os, const PointLight& pl) {
    os << "Point light:\n\t_id:" << pl._id
                   << "\n\tPosition:" << pl.Position
                   << "\n\tIntensity:" << pl.Intensity;
    return os;
}

std::ostream& operator<<(std::ostream& os, const MaterialType& mt)
{
    if (mt == MaterialType::MIRROR) os<<  "Mirror";
    else if (mt == MaterialType::NONE) os<<  "none";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Material& m) {
    os << "Material:\n\t_id:" << m._id
       << "\n\ttype:" << m.materialType
       << "\n\tAmbientReflectance:" << m.AmbientReflectance
       << "\n\tDiffuseReflectance:" << m.DiffuseReflectance
       << "\n\tSpecularReflectance:" << m.SpecularReflectance
       << "\n\tPhongExponent:" << m.PhongExponent
       << "\n\tMirrorReflectance:" << m.MirrorReflectance;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Mesh& m) {
    os << "Mesh:\n\t_id:" << m._id
       << "\n\tshading type:" << (int) m.shadingtype
       << "\n\tmaterial:" << m.material;
    for (const auto& f : m.Faces)
        os << "  " << f << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Triangle& t) {
    os << "Triangle:\n\t_id:" << t._id
       << "\n\tindices:" << t.indices[0] << " " << t.indices[1] << " "  << t.indices[2]
       << "\n\tmaterial:" << t.material;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Sphere& s) {
    os << "Sphere:\n\t_id:" << s._id
              << "\n\tcenter:" << s.center
              << "\n\tradius:" << s.radius
              << "\n\tmaterial:" << s.material;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Camera& c) {
    os << "Camera:\n\t_id:" << c._id
       << "\n\tPosition:" << c.Position
       << "\n\tGaze:" << c.Gaze
       << "\n\tUp:" << c.Up
       << "\n\tplane:" << c.l << " "<< c.r << " "<< c.b << " "<< c.t << " "<< c.nearDistance
       << "\n\tImageResolution:" << c.width << " "<< c.height
       << "\n\tImageName:" << c.ImageName;
    return os;
}


std::ostream& operator<<(std::ostream& os, const SceneInput& s) {
    os << "MaxRecursionDepth: " << s.MaxRecursionDepth << "\n";
    os << "BackgroundColor: " << s.BackgroundColor << "\n";
    os << "ShadowRayEpsilon: " << s.ShadowRayEpsilon << "\n";
    os << "IntersectionTestEpsilon: " << s.IntersectionTestEpsilon << "\n";

    os << "AmbientLight: " << s.AmbientLight << "\n";

    os << "Cameras:\n";
    for (const auto& cam : s.Cameras)
        os << "  " << cam << "\n";

    os << "PointLights:\n";
    for (const auto& pl : s.PointLights)
        os << "  " << pl << "\n";

    os << "Materials:\n";
    for (const auto& mat : s.Materials)
        os << "  " << mat << "\n";

    os << "Vertices:\n";
    for (const auto& v : s.Vertices)
        os << "  " << v << "\n";

    os << "Triangles:\n";
    for (const auto& t : s.Triangles)
        os << "  " << t << "\n";

    os << "Spheres:\n";
    for (const auto& sph : s.Spheres)
        os << "  " << sph << "\n";

    os << "Meshes:\n";
    for (const auto& m : s.Meshes)
        os << "  " << m << "\n";

    return os;
}
