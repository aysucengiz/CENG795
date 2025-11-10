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
    BBox globalBbox;
    Vertex main_center;
    bool visible;
    virtual ObjectType getObjectType() = 0;
    virtual Object *checkIntersection(Ray& r, real& t_min, bool shadow_test) = 0;
    virtual Vec3r getNormal(Vertex &v) = 0;
    virtual ~Object();
    Object(Material &m, uint32_t id,Vertex vMax, Vertex vMin, bool v = true);
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
    Object *checkIntersection(Ray& r, real& t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;

    Triangle(uint32_t id, CVertex &v1, CVertex &v2, CVertex &v3, Material &material, ShadingType st = ShadingType::NONE, bool v=true);

};

class Plane : public Object
{
public:
    Vertex point;
    Vec3r n;

    Plane(uint32_t id, Vertex &v, std::string normal, Material &material, bool vis=true);

    ObjectType getObjectType() override;
    Object *checkIntersection(Ray& r, real& t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;



};


class Mesh : public Object
{
public:
    std::vector<Triangle> Faces;
    ShadingType shadingtype;

    Mesh(uint32_t id, std::string st, Material &m, std::string s,
        bool read_from_file,
        std::deque<CVertex> &vertices, bool v=true,
        uint32_t start_index=0);

    ObjectType getObjectType() override;
    Vec3r getNormal(Vertex &v) override {return Vec3r();}
    Object *checkIntersection(Ray& r, real& t_min, bool shadow_test) override;



};

class  Sphere: public Object
{
public:
    CVertex &center;
    real radius;
    real radius2;

    Sphere(uint32_t id, CVertex& c, real r, Material &m, bool v=true);

    ObjectType getObjectType() override;
    Object *checkIntersection(Ray& ray, real& t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;

};

class  Instance: public Object
{
public:
    bool orig;
    Object *original;
    Transformation *forwardTrans;
    Transformation *backwardTrans;

    Instance(uint32_t id, Object *original, Transformation *trans, bool orig, bool v=true);
    ~Instance();

    ObjectType getObjectType() override;
    Object *checkIntersection(Ray& ray, real& t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;


    void addTransformation(Transformation *trans);
    void computeGlobal();
    Ray getLocal(Ray &r);
    Vertex getLocal(Vertex &v);
    Vec3r getGlobal(Vec3r &v);
    Vertex getGlobal(Vertex &v);


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
    uint32_t numPlanes;
    uint32_t numLights;

    // transformations
    std::vector<Transformation *> transforms;

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
