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
#include "dataTypes/texture/TextureMap.h"

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
    virtual intersectResult checkIntersection(const Ray& r,const real& t_min, bool shadow_test, bool back_cull, real time) const = 0;
    virtual Vec3r getNormal(const Vertex& v, uint32_t currTri, real time) const = 0;
    virtual ~Object();
    virtual Texel getTexel(const Vertex& v, real time, int triID)  const = 0;
    Color diffuseTerm(Color I_R_2, real cos_theta, Vertex& vert, Texel& t) const;
    Color GetColourAt(Color ambientLight, Color I_R_2, real cos_theta, const Vec3r& normal, const Ray& ray, Ray& shadow_ray, real time, int
                      triID) const;
    Color specularTerm(const Vec3r& normal, const Ray& ray, Color I_R_2,
                       Ray& shadow_ray, Vertex& vert, Texel& t) const;
    Object(Material& m, uint32_t id, Vertex vMax, Vertex vMin, std::vector<Texture*> ts ,bool v = true);
    Color getTextureColorAt(Vertex& pos, real time, int triID) const;
    virtual void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const = 0;
    Vec3r getTexturedNormal(const Vertex& v, const Vec3r& n, real time, int triID) const;
    void ComputeBitan(CVertex& a, CVertex& b, CVertex& c, Vec3r& pT, Vec3r& pB);
};

///////////////////////////////////////////////

class Triangle : public Object
{
public:
    CVertex &a, &b, &c;
    Vec3r n;
    Vec3r a_b, a_c;
    ShadingType shadingType;
    Vec3r T;
    Vec3r B;


    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r,const real& t_min,bool shadow_test, bool back_cull, real time) const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri , real time) const override;
    Texel getTexel(const Vertex& v, real time, int triID)  const override;
    void BaryCentric(real &alpha, real& beta, real& gamma, const Vertex& v) const;
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const override;
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
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const override;
    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r,const real& t_min, bool shadow_test, bool back_cull, real time) const override;
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
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const override;
    ObjectType getObjectType() const override;
    intersectResult checkIntersection(const Ray& r,const real& t_min, bool shadow_test, bool back_cull, real time) const override;
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
    intersectResult checkIntersection(const Ray& r,const real& t_min, bool shadow_test, bool back_cull, real time) const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri , real time) const override;
    Vec3r getGlobalNormal(const Vec3r& res, double time) const;

    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const override;
    void computeGlobal();
    Ray getLocal(Ray& r);
    Vertex getLocal(const Vertex& v, real time) const;
    Ray getLocal(const Ray& r, real time) const;
    Vec3r getGlobal(Vec3r& v, double time);
    Vertex getGlobal(Vertex v, real time) const;
    Vec3r getLocal(Vec3r& v, real time);
    Texel getTexel(const Vertex& v, real time, int triID) const override;
};


struct SceneInput
{
    bool print_progress;
    bool back_cull;
    PivotType pt;
    uint32_t MaxObjCount;
    // info about image
    unsigned char* image;
    uint32_t MaxRecursionDepth;
    Color BackgroundColor;
    double ShadowRayEpsilon;
    double IntersectionTestEpsilon;
    SamplingType sampling_type;
    FilterType filter_type;

    uint32_t thread_group_size;
    uint32_t thread_add_endl_after;

    // materials and cam  info
    std::vector<Material> Materials;
    std::vector<Camera> Cameras;

    // vertex info
    std::deque<CVertex> Vertices;
    std::deque<std::pair<int,int>> TexCoords;

    // light info
    Color AmbientLight;
    std::vector<PointLight*> PointLights;

    // object info
    std::deque<Object*> objects;

    // num info
    uint32_t numCameras;
    uint32_t numObjects;
    uint32_t numPlanes;
    uint32_t numLights;

    // transformations
    std::vector<std::shared_ptr<Transformation>> transforms;
    std::vector<Texture*> textures;
    Texture *BackgroundTexture;
    std::vector<Image*> images;

    // precomputed near plane info
    Vec3r u;
    Vertex q;
    real s_u_0;
    real s_v_0;
};


struct HitRecord
{
    Vertex intersection_point;
    Vec3r normal;
    Object const * obj;
    uint32_t currTri;
};


#endif //OBJECT_H
