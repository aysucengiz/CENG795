//
// Created by vicy on 11/19/25.
//

#ifndef CENG795_MESH_H
#define CENG795_MESH_H
#include "Object.h"
#include "../../acceleration/BVH.h"

class Mesh : public Object, public std::enable_shared_from_this<Mesh>
{
public:
    std::vector<Triangle> triangles;
    std::vector<Triangle*> Faces;
    ShadingType shadingtype;
    uint32_t currTri;
    BVH bvh;

    Mesh(uint32_t id, std::string st, Material& m, std::string s,
         bool read_from_file,
         std::deque<CVertex>& vertices,
         PivotType pt,
         uint32_t maxobj,
         std::vector<Texture*> ts,
         bool v = true,
         uint32_t start_index = 0,
         bool computeVNormals = true);

    ObjectType getObjectType() const override;
    Vec3r getNormal(const Vertex& v, uint32_t currTri, real time) const override;
    intersectResult checkIntersection(const Ray& r,const real& t_min, bool shadow_test, bool back_cull, real time) const override;
    ~Mesh();

    Texel getTexel(const Vertex& v, real time)  const override;
    void getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB) const override;
};

#endif //CENG795_MESH_H