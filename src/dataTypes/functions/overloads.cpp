//
// Created by vicy on 11/08/25.
//

#include "overloads.h"


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

Vertex operator/(const Vertex &a, const real& other) {
    return Vertex(a.x / other,
                  a.y / other,
                  a.z / other);
}

Vertex operator+(const Vertex &a, const Vertex &b)
{
    return Vertex(a.x + b.x,
                 a.y + b.y,
                 a.z + b.z);
}


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


// overload vec4r and m4trix

Vec4r operator *(const Vec4r &a, const real &b)
{return Vec4r(a.x *b,a.y *b, a.z *b, a.w *b);}

Vec4r operator /(const Vec4r &a, const real &b)
{return Vec4r(a.x /b,a.y /b, a.z /b, a.w /b);}

Vec4r operator -(const Vec4r &a, const Vec4r &b)
{return Vec4r(a.x - b.x,a.y - b.y, a.z - b.z, a.w - b.w);}

Vec4r operator +(const Vec4r &a, const Vec4r &b)
{return Vec4r(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w);}

Vec4r operator +(const Vec3r &a, const Vec4r &b)
{return Vec4r(a.i + b.x,a.j + b.y, a.k + b.z, 0.0 + b.w);}

Vec4r operator +(const Vec4r &b, const Vec3r &a)
{return Vec4r(a.i + b.x, a.j + b.y, a.k + b.z, 0.0 + b.w);}

Vec4r operator -(const Vec3r &a, const Vec4r &b)
{return Vec4r(a.i - b.x,a.j - b.y, a.k - b.z, 0.0 - b.w);}

Vec4r operator -(const Vec4r &b, const Vec3r &a)
{return Vec4r(a.i - b.x, a.j - b.y, a.k - b.z, 0.0 - b.w);}

Vec4r operator +(const Vertex &a, const Vec4r &b)
{return Vec4r(a.x + b.x,a.y + b.y, a.z + b.z, 1.0 + b.w);}

Vec4r operator +(const Vec4r &b, const Vertex &a)
{return Vec4r(a.x + b.x, a.y + b.y, a.z + b.z, 1.0 + b.w);}

Vec4r operator -(const Vertex &a, const Vec4r &b)
{return Vec4r(a.x - b.x,a.y - b.y, a.z - b.z, 1.0 - b.w);}

Vec4r operator -(const Vec4r &b, const Vertex &a)
{return Vec4r(a.x - b.x, a.y - b.y, a.z - b.z, 1.0 - b.w);}





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

std::ostream& operator<<(std::ostream& os, const CVertex& cv) {
    os << "cvertex: " << cv.id ;
    os << " v: " << cv.v;
    os << " n:" << cv.n;
    return os;
}

std::ostream& operator<<(std::ostream& os, const PointLight& pl) {
    os << "Point light " << pl._id << ":"
                   << "\n\tPosition:" << pl.Position
                   << "\n\tIntensity:" << pl.Intensity;
    return os;
}

std::ostream& operator<<(std::ostream& os, const ObjectType& ot)
{
    if (ot == ObjectType::NONE) os<<  "none";
    else if (ot == ObjectType::SPHERE) os<<  "sphere";
    else if (ot == ObjectType::TRIANGLE) os<<  "triangle";
    else if (ot == ObjectType::MESH) os<<  "mesh";
    else os << "unknown";
    return os;
}
std::ostream& operator<<(std::ostream& os, const MaterialType& mt)
{
    if (mt == MaterialType::MIRROR) os<<  "Mirror";
    else if (mt == MaterialType::NORMAL) os<<  "normal";
    else if (mt == MaterialType::NONE) os<<  "none";
    else if (mt == MaterialType::CONDUCTOR) os<<  "conductor";
    else if (mt == MaterialType::DIELECTRIC) os<<  "dielectric";
    else os << "unknown";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Material& m) {
    os << "Material " << m._id << ":"
       << "\n\ttype: " << m.materialType
       << "\n\tAmbientReflectance: " << m.AmbientReflectance
       << "\n\tDiffuseReflectance: " << m.DiffuseReflectance
       << "\n\tSpecularReflectance: " << m.SpecularReflectance
       << "\n\tPhongExponent: " << m.PhongExponent;
    if (m.materialType == MaterialType::MIRROR || m.materialType == MaterialType::DIELECTRIC || m.materialType == MaterialType::CONDUCTOR)
        os << "\n\tMirrorReflectance: " << m.MirrorReflectance;
    if (m.materialType == MaterialType::DIELECTRIC)
    {
        os << "\n\tAbsorptionCoefficient: " << m.AbsorptionCoefficient;
        os << "\n\tRefractionIndex: " << m.RefractionIndex;
    }
    if (m.materialType == MaterialType::CONDUCTOR)
    {
        os << "\n\tAbsorptionIndex: " << m.AbsorptionIndex;
        os << "\n\tRefractionIndex: " << m.RefractionIndex;
    }
    return os;
}

std::ostream& operator<<(std::ostream& os, const Mesh& m) {
    os << "Mesh " << m._id << ":"
       << "\n\tshading type:" << (int) m.shadingtype
       << "\n\tmaterial:" << m.material._id;
    for (const auto& f : m.Faces)
        os << "  " << f << "\n"
    << "\n\tglobalBbox:" << m.globalBbox;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Triangle& t) {
    os << "Triangle " << t._id << ":"
       << "\n\tindices:" << t.a.id << " " << t.b.id << " "  << t.c.id
       << "\n\ta_b:" << t.a_b << " a_c: " << t.a_c << " "  << t.c.id
       << "\n\tmaterial:" << t.material._id
    << "\n\tglobalBbox:" << t.globalBbox;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Sphere& s) {
    os << "Sphere " << s._id << ":"
              << "\n\tcenter:" << s.center
              << "\n\tradius:" << s.radius
              << "\n\tmaterial:" << s.material._id
    << "\n\tglobalBbox:" << s.globalBbox;
    return os;
}

std::ostream& operator<<(std::ostream& os, Object *s)
{
    if (s->getObjectType() == ObjectType::SPHERE)
        os << "  " << *dynamic_cast<Sphere*>((s)) << "\n";
    else if (s->getObjectType() == ObjectType::TRIANGLE)
        os << "  " << *dynamic_cast<Triangle*>(s) << "\n";
    else if (s->getObjectType() == ObjectType::MESH)
        os << "  " << *dynamic_cast<Mesh*>(s) << "\n";
    else if (s->getObjectType() == ObjectType::PLANE)
        os << "  " << *dynamic_cast<Plane*>(s) << "\n";
    else if (s->getObjectType() == ObjectType::INSTANCE)
        os << "  " << *dynamic_cast<Instance*>(s) << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Camera& c) {
    os << "Camera " << c._id << ":"
       << "\n\tPosition:" << c.Position
       << "\n\tGaze:" << c.Gaze
       << "\n\tUp:" << c.Up
       << "\n\tplane:" << c.l << " "<< c.r << " "<< c.b << " "<< c.t << " "<< c.nearDistance
       << "\n\tImageResolution:" << c.width << " "<< c.height
       << "\n\tImageName:" << c.ImageName;
    return os;
}


std::ostream& operator<<(std::ostream& os, const Plane& p)
{
    os << "Plane " << p._id << ":"
          << "\n\tpoint:" << p.point
          << "\n\tnormal:" << p.n
          << "\n\tmaterial:" << p.material._id;
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
        os << "  " << v.v << "\n";

    os << "Objects:\n";
    for (int i=0; i<s.numObjects; i++)
    {
        os << s.objects[i];
    }


    return os;
}


std::ostream& operator<<(std::ostream& os, TransformationType t)
{
    if (t == TransformationType::NONE) os<<  "none";
    else if (t == TransformationType::ROTATE) os<<  "rotate";
    else if (t == TransformationType::TRANSLATE) os<< "translate";
    else if (t == TransformationType::SCALE) os<<  "scale";
    else if (t == TransformationType::COMPOSITE) os<<  "composite";
    else os << "unknown";
    return os;
}


std::ostream& operator<<(std::ostream& os, PivotType t)
{
    if (t == PivotType::MEDIAN) os<<  "median";
    else if (t == PivotType::MIDDLE) os<<  "middle";
    else if (t == PivotType::SAH) os<< "SAH";
    else os << "unknown";
    return os;
}

std::ostream& operator<<(std::ostream& os, BVHNodeType t)
{
    if (t == BVHNodeType::LEAF) os<<  "leaf \t\t";
    else if (t == BVHNodeType::INT_W_LEFT) os<<  "w/ left \t";
    else if (t == BVHNodeType::INT_W_RIGHT) os<< "w/ right \t";
    else if (t == BVHNodeType::INT_W_BOTH) os<<  "w/ left right";
    else os << "unknown";
    return os;
}
std::ostream& operator<<(std::ostream& os, Axes x)
{
    if (x == Axes::x) os<<  "x";
    else if (x == Axes::y) os<<  "y";
    else if (x == Axes::z) os<< "z";
    else os << "unknown";
    return os;
}
std::ostream& operator<<(std::ostream& os, BVHNode &node)
{
    os << "Node: "
      << "  type:" << node.type <<"\t"
      <<  node.bbox
      << "\tobjCount: " << node.objCount;
    if (node.type == BVHNodeType::LEAF) os << "\tfirstObjID: " << node.firstObjID;
    else os << "\trightOffset: " << node.rightOffset;
    return os;
}

std::ostream& operator<<(std::ostream& os, BVH &bvh)
{
    os << "BVH: "
       << "pivotType: " << bvh.pivotType
       << "\nNodes: ";
    for (auto node : bvh.nodes) os << node << "\n";
    return os;
}
std::ostream& operator<<(std::ostream& os,const BBox &bbox)
{
    os << "BBox: Max: " << bbox.vMax << " Min: " << bbox.vMin;
    return os;
}
std::ostream& operator<<(std::ostream& os, const Instance& i)
{
    os << "Instance " << i._id << ":"
      << "\n\tObject:" << i.original->getObjectType() << " " <<i.original->_id
      << "\n\tforwardTrans:" << i.forwardTrans->getTransformationType() << " " << i.forwardTrans
      << "\tbackwardTrans:" << i.backwardTrans->getTransformationType() << " "  << i.backwardTrans
      << "\n\tglobalBbox:" << i.globalBbox;
    return os;
}


std::ostream& operator<<(std::ostream& os, Transformation *t)
{
    if (t->getTransformationType()  == TransformationType::ROTATE) os<<  *dynamic_cast<Rotate*>(t);
    else if (t->getTransformationType()  == TransformationType::TRANSLATE) os<<  *dynamic_cast<Translate*>(t);
    else if (t->getTransformationType()  == TransformationType::SCALE) os<<   *dynamic_cast<Scale*>(t);
    else if (t->getTransformationType()  == TransformationType::COMPOSITE) os<<   *dynamic_cast<Composite*>(t);
    else os << "type unknown";
    return os;
}
std::ostream& operator<<(std::ostream& os, Rotate &r)
{
    os << "Rotate: axis:" << r.axis.pos << " " << r.axis.dir  << " angle: " << r.angle
    << "\n arr: " << r.arr << "\n";
return os;
}


std::ostream& operator<<(std::ostream& os, std::array<std::array<double,4>,4> &arr)
{
    os << "\n";
    for (const auto& row : arr) {
        os << "[ ";
        for (double v : row) {
            os << v << " ";
        }
        os << "]\n";
    }
    return os;

}

std::ostream& operator<<(std::ostream& os, Translate &r)
{
    os << "Translate: " << r.x << " " << r.y  << " " << r.z
    << "\n arr:" << r.arr << "\n";
    return os;
}

std::ostream& operator<<(std::ostream& os, Scale &r)
{
    os << "Scale: " << r.x << " " << r.y  << "  " << r.z << " around: " << r.center
    << "\n arr:" << r.arr << "\n";
    return os;
}
std::ostream& operator<<(std::ostream& os, Composite &t)
{
    os << "Composite: " << "\n arr:" << t.arr << "\n";
    return os;
}


