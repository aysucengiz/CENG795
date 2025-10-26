//
// Created by vicy on 10/14/25.
//

#ifndef CENG795_RAYTRACERTHREAD_H
#define CENG795_RAYTRACERTHREAD_H

#include "../dataTypes/Object.h"
#include "../dataTypes/DataTypes.h"
#include "../dataTypes/helpers.h"
#include <iostream>


class RaytracerThread
{
private:
    SceneInput &scene;
    Camera &cam;
    Color final_color;
    uint32_t curr_pixel;
    uint32_t y;
    Ray viewing_ray;
    Ray shadow_ray;
    HitRecord hit_record;
    real t_min;

public:
    RaytracerThread(SceneInput &_scene, uint32_t _y, Camera &c) : scene(_scene), y(_y), hit_record(), t_min(INFINITY),
                                                                       cam(c), curr_pixel(_y*c.width*3)
    {viewing_ray.pos = c.Position;}

    void drawRow();
    void computeViewingRay(uint32_t x);
    Color computeColor(Ray &ray, int depth);
    void writeColorToImage();
    void checkObjIntersection(Ray &ray);
    bool isUnderShadow();
    void computeHitRecord(Ray &ray);
    Color reflect(Ray &ray, int depth);
    void compute_shadow_ray(uint32_t i);
    Color diffuseTerm(real cos_theta, Color I_R_2);
    Color specularTerm(Ray &ray, real cos_theta, Color I_R_2);
};


#endif //CENG795_RAYTRACERTHREAD_H