//
// Created by Aysu on 04-Oct-25.
//

#ifndef OBJECT_H
#define OBJECT_H
#include <memory>
#include <deque>

#include "../matrix/Matrix.h"
#include "../../fileManagement/happly.h"
#include "../matrix/transformation.h"
#include "../object/BBox.h"
#include "dataTypes/texture/TextureMap.h"
#include "dataTypes/base/Material.h"

inline bool ACCELERATE = false;

class Object
{
public:

    struct intersectResult
    {
        Object const* obj;
        real t_min;
        int32_t currTri = -1;
    };
    uint32_t _id;
    Material& material;
    BBox globalBbox;
    Vertex main_center;
    Texture *DiffuseTexture;
    Texture *SpecularTexture;
    Texture *NormalTexture;
    Texture *AllTexture;
    bool visible;
    virtual ObjectType getObjectType() const = 0;
    virtual intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const = 0;
    virtual Vec3r getNormal(const Vertex& v, uint32_t currTri, real time) const = 0;
    virtual ~Object();
    virtual Texel getTexel(const Vertex& v, real time, int triID)  const = 0;
    Color diffuseTerm(Vertex& vert, Texel& t, real time, Texel rate_of_change) const;
    Color GetColourAt(Color I_R_2, const Vec3r& normal, const Ray& ray, Ray& shadow_ray, real time, int
                      triID, Texel& rate_of_change) const;
    real getMipMapLevel(Texel rate_of_change) const;
    Color specularTerm(
        Vertex& vert, Texel& t, Texel rate_of_change) const;
    Object(Material& m, uint32_t id, Vertex vMax, Vertex vMin, std::vector<Texture*> ts ,bool v = true);
    Color getTextureColorAt(Vertex& pos, real time, int triID, Texel rate_of_change) const;
    virtual void getBitan(const Vertex &v, Vec3r &pT, Vec3r &pB, int triID, bool normalize, real time) const = 0;
    real GrayScale(Color c) const;
    real h(Vertex v) const;
    Vec3r getTexturedNormal(const Vertex& v, const Vec3r& n, real time, int triID) const;
    void ComputeBitan(CVertex& b, CVertex& a, CVertex& c, Vec3r& pT, Vec3r& pB, Vec3r& n);
    virtual Vertex getLocal(const Vertex& v, real time) const { return v;}
    virtual bool isLuminous() const {return false;}
};

///////////////////////////////////////////////

class Triangle : public Object
{
public:
    CVertex &a, &b, &c;
    Vec3r n;
    Vec3r a_b, a_c;
    ShadingType shadingType;
    Vec3r T, T_norm;
    Vec3r B, B_norm;


    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri , real time) const override;
    Texel getTexel(const Vertex& v, real time, int triID)  const override;
    void BaryCentric(real &alpha, real& beta, real& gamma, const Vertex& v) const;
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const override;
    Triangle(uint32_t id, CVertex& v1, CVertex& v2, CVertex& v3, Material& material,std::vector<Texture*> ts , ShadingType st = ShadingType::NONE,
             bool v = true, bool computeVNormals = true);

};

class Plane : public Object
{
public:
    Vertex point;
    Vec3r n;
    Vec3r T;
    Vec3r B;

    Plane(uint32_t id, Vertex& v, std::string normal, Material& material,std::vector<Texture*> ts , bool vis = true);
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const override;
    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri , real time) const override;
    Texel getTexel(const Vertex& v, real time, int triID) const override;

};




class Sphere : public Object
{
public:
    CVertex& center;
    real radius;
    real radius2;

    Sphere(uint32_t id, CVertex& c, real r, Material& m, std::vector<Texture*> ts ,bool v = true);
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const override;
    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri , real time) const override;
    Texel getTexel(const Vertex& v, real time, int triID) const override;

};

class Instance : public Object
{
public:
    bool orig;
    Vec3r motion;
    bool has_motion;

    Object* original;
    std::shared_ptr<Transformation> forwardTrans;
    std::shared_ptr<Transformation> backwardTrans;

    Instance(uint32_t id, Object* original, std::shared_ptr<Transformation>  trans, Material& mat, Vec3r m,std::vector<Texture*> ts , bool orig, bool v = true);
    ~Instance();

    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time, real dist = 1.0) const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri , real time) const override;
    Vec3r getGlobalNormal(const Vec3r& res, double time) const;

    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const override;
    void computeGlobal();
    Ray getLocal(Ray& r);
    Vertex getLocal(const Vertex& v, real time) const override;
    Ray getLocal(const Ray& r, real time) const;
    Vec3r getGlobal(Vec3r& v, double time);
    Vertex getGlobal(Vertex v, real time) const;
    Vec3r getLocal(Vec3r& v, real time);
    Texel getTexel(const Vertex& v, real time, int triID) const override;
};

struct HitRecord
{
    Vertex intersection_point;
    Vec3r normal;
    Object const * obj;
    uint32_t currTri;
    Texel rate_of_change = Texel(0.0,0.0);
};




#endif //OBJECT_H