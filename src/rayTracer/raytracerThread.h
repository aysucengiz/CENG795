//
// Created by vicy on 10/14/25.
//

#ifndef CENG795_RAYTRACERTHREAD_H
#define CENG795_RAYTRACERTHREAD_H

#include "../dataTypes/object/Object.h"
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

    // for recursive refraction
    //real n1;

public:
    static long done_threads;

    RaytracerThread(SceneInput &_scene, Camera &camID, BVH &_bvh) :
    scene(_scene), cam(camID), bvh(_bvh), air(0,Color(),Color(),Color(),0, "",Color(),Color(0.0,0.0,0.0),1.0)
    { }

    RaytracerThread(const RaytracerThread &rt) : scene(rt.scene), cam(rt.cam), bvh(rt.bvh),  air(0,Color(),Color(),Color(),0, "",Color(0.0,0.0,0.0),Color(0.0,0.0,0.0),1.0) {}
    Ray reflectionRay(const Ray& ray,MaterialType type,const HitRecord& hit_record,const Vec3r &n, real cos_theta) const;
    void drawRow(uint32_t y);
    void drawBatch(uint32_t start_idx, uint32_t w, uint32_t h);
    Ray computeViewingRay(uint32_t x, uint32_t y);
    Color computeColor(Ray &ray, int depth, const Material &m1);
    void checkObjIntersection(Ray &ray,real &t_min, HitRecord &hit_record, bool back_cull);
    bool isUnderShadow(Ray &shadow_ray);
    Ray compute_shadow_ray(const HitRecord &hit_record,uint32_t i);
    static Color diffuseTerm(const HitRecord &hit_record, real cos_theta, Color I_R_2);
    Color specularTerm(const HitRecord &hit_record, const Ray &ray, real cos_theta, Color I_R_2, Ray &shadow_ray) const;

    Color reflect(Ray &ray, int depth, MaterialType type, HitRecord &hit_record, const Material &m1);
    Color refract(Ray &ray, int depth, const Material &m1, const Material &m2, HitRecord &hit_record);
    Ray refractionRay(Ray &ray, real n1, real n2, Vertex point,  Vec3r n, real cos_theta, real &Fr, real &Ft);
    Object::intersectResult traverse(const Ray &ray, const real &t_min, const std::deque<Object *> &objects, bool shadow_test , bool back_cull) const;

};


#endif //CENG795_RAYTRACERTHREAD_H