//
// Created by vicy on 10/14/25.
//

#ifndef CENG795_RAYTRACERTHREAD_H
#define CENG795_RAYTRACERTHREAD_H

#include "../dataTypes/object/Object.h"
#include "../dataTypes/object/Mesh.h"
#include "../dataTypes/base/SceneData.h"
#include "../functions/helpers.h"
#include <iostream>
#include <cmath>

#include "acceleration/BVH.h"


class RaytracerThread
{
private:
    const SceneInput &scene;
    const Camera &cam;
    const BVH &bvh;
    const Material air;
    std::vector<int> sampleIdxPixel;
    real time;

    // for recursive refraction
    //real n1;

public:
    static long done_threads;


    RaytracerThread(SceneInput &_scene, Camera &camID, BVH &_bvh) :
    scene(_scene), cam(camID), bvh(_bvh), air(0,Color(),Color(),Color(),0, "",Color(),Color(0.0,0.0,0.0),1.0)
    {
        sampleIdxPixel.reserve(cam.numSamples);
        for (int i = 0; i < cam.numSamples; i++)
        {
            sampleIdxPixel.push_back(i);
        }
    }

    RaytracerThread(const RaytracerThread &rt) : scene(rt.scene), cam(rt.cam), bvh(rt.bvh),  air(0,Color(),Color(),Color(),0, "",Color(0.0,0.0,0.0),Color(0.0,0.0,0.0),1.0)
    {
        sampleIdxPixel.reserve(cam.numSamples);
        for (int i = 0; i < cam.numSamples; i++)
        {
            sampleIdxPixel.push_back(i);
        }
    }
    Ray reflectionRay(Ray& ray,MaterialType type, HitRecord& hit_record);
    Color Filter(std::vector<Color>& colors, const std::vector<std::array<real,2>> &locs);
    void writeToImage(uint32_t& curr_pixel, Color& final_color);
    void drawPixel(uint32_t& curr_pixel, uint32_t x, uint32_t y);
    void PrintProgress();
    void drawRow(uint32_t y);
    void drawBatch(uint32_t start_idx, uint32_t w, uint32_t h);
    Ray computeViewingRay(int x, int y, int i);
    Color computeColor(Ray &ray, int depth, const Material &m1);
    void checkObjIntersection(Ray &ray,real &t_min, HitRecord &hit_record, bool back_cull);
    bool isUnderShadow(Ray &shadow_ray);
    Ray compute_shadow_ray(const HitRecord& hit_record, uint32_t i, std::array<real, 2> sample) const;
    static Color diffuseTerm(const HitRecord &hit_record, Color I_R_2);
    static Color specularTerm(const HitRecord &hit_record, const Ray &ray,Color I_R_2, Ray &shadow_ray);

    Color reflect(Ray &ray, int depth, MaterialType type, HitRecord &hit_record, const Material &m1);
    Color refract(Ray &ray, int depth, const Material &m1, const Material &m2, HitRecord &hit_record);
    Ray refractionRay(Ray &ray, real n1, real n2, Vertex point,  Vec3r n, real &Fr, real &Ft);
    Object::intersectResult traverse(const Ray &ray, const real &t_min, const std::deque<Object *> &objects, bool shadow_test , bool back_cull) const;

};


#endif //CENG795_RAYTRACERTHREAD_H