//
// Created by Aysu on 04-Oct-25.
//

#ifndef OBJECT_H
#define OBJECT_H
#include "DataTypes.h"

struct Triangle{
    uint32_t _id;
    uint32_t indices[3];
    uint32_t material;
    Vec3r n;
    Vec3r a_b, a_c;
    real detA;

    Triangle(uint32_t id, std::string ind, uint32_t material) :
            _id(id), material(material){
        std::istringstream ss(ind);
        ss >> indices[0] >> indices[1] >> indices[2];
        indices[0]--; indices[1]--; indices[2]--;
        if (ss.fail()) {
            throw std::invalid_argument("Invalid triangle indices string: " + ind);
        }
    }

    Triangle(uint32_t id, uint32_t ind[3], uint32_t material) :
            _id(id), material(material) {indices[0] = ind[0]; indices[1] = ind[1]; indices[2] = ind[2];}

};

struct Mesh{
    uint32_t _id;
    ShadingType shadingtype;
    uint32_t material;
    std::vector<Triangle> Faces;

    Mesh(uint32_t id, std::string st, uint32_t m, std::string s, bool read_from_file)
    : _id(id), material(m) {
        if (st == "smooth")    shadingtype = ShadingType::SMOOTH;
        else if (st == "flat") shadingtype = ShadingType::FLAT;
        else                   shadingtype = ShadingType::NONE;

        if (read_from_file)
        {

        }
        else
        {
            std::istringstream verticesStream(s);
            uint32_t vert[3];

            while (verticesStream >>vert[0]>>vert[1]>> vert[2]) {
                vert[0]--; vert[1]--; vert[2]--;
                Faces.push_back(Triangle(Faces.size(), vert, m));
            }
        }

    }
};

struct  Sphere{
    uint32_t _id;
    uint32_t center;
    real radius;
    uint32_t material;
    Sphere(uint32_t id, uint32_t c, real r, uint32_t m)
    : _id(id), center(c), radius(r), material(m) {}
};

typedef struct SceneInput{
    unsigned int MaxRecursionDepth;
    Color BackgroundColor;
    real ShadowRayEpsilon;
    real IntersectionTestEpsilon;
    std::vector<Camera> Cameras;
    Color AmbientLight;
    std::vector<PointLight> PointLights;
    std::vector<Material> Materials;
    std::vector<Vertex> Vertices;
    std::vector<Triangle> Triangles;
    std::vector<Sphere> Spheres;
    std::vector<Mesh> Meshes;
} SceneInput;


struct HitRecord{
    Vertex intersection_point;
    Vec3r normal;
    uint32_t objID;
    uint32_t meshID;
    ObjectType type;
    uint32_t matID;
};







#endif //OBJECT_H
