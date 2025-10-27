//
// Created by Aysu on 04-Oct-25.
//

#ifndef OBJECT_H
#define OBJECT_H
#include <memory>
#include <deque>

#include "DataTypes.h"
#include "../fileManagement/happly.h"

class Object
{
public:
    uint32_t _id;
    Material &material;
    virtual ObjectType getObjectType() = 0;
    virtual Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) = 0;
    virtual Vec3r getNormal(Vertex &v) = 0;
    virtual ~Object();
    Object(Material &m, uint32_t id) : material(m), _id(id) {}
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

    ObjectType getObjectType() override {return ObjectType::PLANE;};
    Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override { return n;}

    Plane(uint32_t id, Vertex &v, std::string normal, Material &material) : Object(material,id), point(v)
    {
        std::istringstream ss(normal);
        ss >> n.i >> n.j >> n.k;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid Normal string for plane: " + normal);
        }
    }

};


class Mesh : public Object
{
public:
    ObjectType getObjectType() override;
    Vec3r getNormal(Vertex &v) override {return Vec3r();}
    std::vector<Triangle> Faces;
    ShadingType shadingtype;
    Object *checkIntersection(const Ray& r, real &t_min, bool shadow_test) override;



    Mesh(uint32_t id, std::string st, Material &m, std::string s, bool read_from_file, std::deque<CVertex> &vertices, uint32_t start_index=0)
    : Object(m,id) {
        if (st == "smooth")    shadingtype = ShadingType::SMOOTH;
        else if (st == "flat") shadingtype = ShadingType::FLAT;
        else                   shadingtype = ShadingType::NONE;

        if (read_from_file)
        {
            happly::PLYData plyIn(s);
            std::vector<std::vector<int>> f = plyIn.getElement("face").getListProperty<int>("vertex_indices");
            std::cout << "First face indices: "
          << f[0][0] << " " << f[0][1] << " " << f[0][2] << std::endl;for (int i = 0; i < f.size(); i++)
            {
                if (vertices.size() <= start_index+f[i][0])std::cout << vertices.size() <<" : " << start_index-1+f[i][0]<< std::endl;
                if (vertices.size() <= start_index+f[i][1])std::cout << vertices.size() <<" : " << start_index-1+f[i][1]<< std::endl;
                if (vertices.size() <= start_index+f[i][2])std::cout << vertices.size() <<" : " << start_index-1+f[i][2]<< std::endl;

                if (f[i].size() == 3)
                    Faces.push_back(Triangle(Faces.size(),
                          vertices[start_index-1+f[i][0]], vertices[start_index-1+f[i][1]], vertices[start_index-1+f[i][2]],
                        m, shadingtype));
            }

        }
        else
        {
            std::istringstream verticesStream(s);
            uint32_t vert[3];

            while (verticesStream >>vert[0]>>vert[1]>> vert[2]) {
                vert[0]--; vert[1]--; vert[2]--;
                Faces.push_back(Triangle(Faces.size(),
                    vertices[vert[0]], vertices[vert[1]], vertices[vert[2]],
                    m, shadingtype));

            }
        }


    }
};

class  Sphere: public Object
{
public:
    CVertex &center;
    real radius;
    real radius2;

    ObjectType getObjectType() override;
    Object *checkIntersection(const Ray& ray, real &t_min, bool shadow_test) override;
    Vec3r getNormal(Vertex &v) override;

    Sphere(uint32_t id, CVertex& c, real r, Material &m)
        : Object(m,id), center(c), radius(r) { radius2 = radius * radius; }
};


typedef struct SceneInput{
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
    std::vector<Vec3r> VertexNormals;

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
} SceneInput;


struct HitRecord{
    Vertex intersection_point;
    Vec3r normal;
    Object *obj;
    Mesh *mesh;
};







#endif //OBJECT_H
