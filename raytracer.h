//
// Created by vicy on 10/14/25.
//

#ifndef CENG795_RAYTRACER_H
#define CENG795_RAYTRACER_H

#include "dataTypes/DataTypes.h"
#include "dataTypes/Object.h"
#include "dataTypes/helpers.h"
#include "fileManagement/Parser.h"
#include "fileManagement/PPM.h"


class Raytracer
{
private:
	int num_cameras;
    SceneInput scene;
    unsigned char *image;
    Color final_color;
    uint32_t curr_pixel;
    Ray viewing_ray;
    Ray shadow_ray;
    HitRecord hit_record;
    real t_min;
    uint32_t t_min_index;
    uint32_t numTriangle;
    uint32_t numSphere;
    uint32_t numMesh;
    uint32_t numLights;
    Vec3r u;
    Vertex q;
    real s_u;
    real s_v;
    real s_u_0;
    real s_v_0;

public:
    void parseScene(std::string input_path);
    void drawScene(uint32_t camID);
    void drawAllScenes();
    void computeViewingRay(Camera &cam, uint32_t x, uint32_t y);
    Color computeColor(int depth);
    void writeColorToImage();
    void checkObjIntersection();
    bool isUnderShadow();
    real checkTriangleIntersection(Ray &r, uint32_t j, int32_t i=-1);
    real checkSphereIntersection(Ray &r, uint32_t i);
    bool checkMeshIntersection(Ray &r, uint32_t i);
    void computeHitRecord();
    Vec3r sphereNormal(Sphere &s, Vertex &v);
    Color reflect(int depth, Color &reflectance);
    void compute_shadow_ray(uint32_t i);
    Color diffuseTerm(real cos_theta, Color I_R_2);
    Color specularTerm(real cos_theta, Color I_R_2);
};


#endif //CENG795_RAYTRACER_H