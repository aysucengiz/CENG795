//
// Created by vicy on 10/14/25.
//

#ifndef CENG795_RAYTRACERTHREAD_H
#define CENG795_RAYTRACERTHREAD_H

#include "../dataTypes/Object.h"
#include "../dataTypes/DataTypes.h"
#include "../dataTypes/helpers.h"
#include <iostream>
#include <cmath>


class RaytracerThread
{
private:
    const SceneInput &scene;
    const Camera &cam;
    Color final_color;
    uint32_t curr_pixel;
    uint32_t y;
    Ray viewing_ray;
    Ray shadow_ray;
    HitRecord hit_record;
    real t_min;

    // for recursive refraction
    real n1;
    int mID;

public:
    static int done_threads;

    RaytracerThread(SceneInput &_scene, const uint32_t _y, Camera &c) :
    scene(_scene), cam(c), curr_pixel(_y*c.width*3), y(_y), hit_record(),
    t_min(INFINITY), n1(1.0), mID(-1)
    {viewing_ray.pos = c.Position;}

    RaytracerThread(const RaytracerThread &rt) : scene(rt.scene), cam(rt.cam), curr_pixel(rt.y*rt.cam.width*3), y(rt.y),
                                                                   hit_record(), t_min(INFINITY), n1(1.0)
    {viewing_ray.pos = rt.cam.Position;}

    void drawRow();
    void computeViewingRay(uint32_t x);
    Color computeColor(Ray &ray, int depth);
    void writeColorToImage();
    void checkObjIntersection(Ray &ray);
    bool isUnderShadow();
    void computeHitRecord(Ray &ray);
    void compute_shadow_ray(uint32_t i);
    Color diffuseTerm(real cos_theta, Color I_R_2);
    Color specularTerm(Ray &ray, real cos_theta, Color I_R_2);

    Color reflect(Ray &ray, int depth);
    Color refract(Ray &ray, int depth);
    Ray refractionRay(Ray &ray, Material &m2, Vec3r &n, real &Fr, real &Ft);
};


#endif //CENG795_RAYTRACERTHREAD_H