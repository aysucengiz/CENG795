//
// Created by Aysu on 04-Oct-25.
//

#ifndef DATATYPES_H
#define DATATYPES_H

#include <vector>

typedef struct Color{
    uint32_t r;
    uint32_t g;
    uint32_t b;
};

typedef struct Vector{
    double i;
    double j;
    double k;
};

typedef struct Vertex{
    double x;
    double y;
    double z;
};

typedef struct Camera{
    uint8_t _id;
    Vertex Position;
    Vector Gaze;
    Vector Up;
    uint32_t l;
    uint32_t r;
    uint32_t t;
    uint32_t b;
    uint32_t z;
    uint32_t ImageResolution[2];
    std::string ImageName;
};

typedef struct PointLight{
    uint8_t _id;
    Vertex Position;
    Color Intensity;
};

typedef struct Material{
    uint8_t _id;
    Color AmbientReflectance;
    Color DiffuseReflectance;
    Color SpecularReflectance;
    uint32_t PhongExponent;
    Color MirrorReflectance;
};

typedef struct SceneInput{
    unsigned int maxRecursionDepth;
    Color BackgroundColor;
    double ShadowRayEpsilon;
    double IntersectionTestEpsilon;
    std::vector<Camera> Cameras;
    Color AmbientLight;
    std::vector<PointLight> PointLights;
    std::vector<Material> Materials;
    std::vector<Vertex> Vertices;
    std::vector<Triangle> Triangles;
    std::vector<Sphere> Spheres;
};

#endif //DATATYPES_H
