//
// Created by Aysu on 04-Oct-25.
//

#ifndef OBJECT_H
#define OBJECT_H
#include <memory>
#include <deque>

#include "../base/SceneData.h"
#include "../matrix/Matrix.h"
#include "../../fileManagement/happly.h"
#include "../matrix/transformation.h"
#include "../object/BBox.h"

class Object
{
public:
    uint32_t _id;
    Material &material;
    BBox bbox;
    Vertex main_center;
    virtual ObjectType getObjectType() = 0;
    virtual Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) = 0;
    virtual Vec3r getNormal(Vertex &v) = 0;
    virtual ~Object();
    Object(Material &m, uint32_t id,Vertex vMax, Vertex vMin);
};

///////////////////////////////////////////////

class Triangle : public Object
{
public:
    CVertex &a, &b, &c;
    Vec3r n;
    Vec3r a_b, a_c;
    ShadingType shadingType;

    ObjectType getObjectType() override;
    Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;

    Triangle(uint32_t id, CVertex &v1, CVertex &v2, CVertex &v3, Material &material, ShadingType st = ShadingType::NONE);

};

class Plane : public Object
{
public:
    Vertex point;
    Vec3r n;

    Plane(uint32_t id, Vertex &v, std::string normal, Material &material);

    ObjectType getObjectType() override;
    Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;



};


class Mesh : public Object
{
public:
    std::vector<Triangle> Faces;
    ShadingType shadingtype;

    Mesh(uint32_t id, std::string st, Material &m, std::string s,
        bool read_from_file,
        std::deque<CVertex> &vertices,
        uint32_t start_index=0);

    ObjectType getObjectType() override;
    Vec3r getNormal(Vertex &v) override {return Vec3r();}
    Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) override;



};

class  Sphere: public Object
{
public:
    CVertex &center;
    real radius;
    real radius2;

    Sphere(uint32_t id, CVertex& c, real r, Material &m);

    ObjectType getObjectType() override;
    Object *checkIntersection(const Ray& ray, real &t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;

};

class  Instance: public Object
{
public:
    Object *original;
    Transformation *forwardTrans;
    Transformation *backwardTrans;

    Instance(uint32_t id, Object *original, Transformation *trans);
    ~Instance();

    ObjectType getObjectType() override;
    Object *checkIntersection(const Ray& ray, real &t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;


};


struct SceneInput{
    // info about image
    unsigned char *image;
    uint32_t MaxRecursionDepth;
    Color BackgroundColor;
    real ShadowRayEpsilon;
    real IntersectionTestEpsilon;

    // materials and cam  info
    std::vector<Material> Materials;
    std::vector<Camera> Cameras;

    // vertex info
    std::deque<CVertex> Vertices;

    // light info
    Color AmbientLight;
    std::vector<PointLight> PointLights;

    // object info
    std::vector<Object *> objects;

    // num info
    uint32_t numCameras;
    uint32_t numObjects;
    uint32_t numLights;

    // precomputed near plane info
    Vec3r u;
    Vertex q;
    real s_u_0;
    real s_v_0;
};


struct HitRecord{
    Vertex intersection_point;
    Vec3r normal;
    Object *obj;
};







#endif //OBJECT_H
