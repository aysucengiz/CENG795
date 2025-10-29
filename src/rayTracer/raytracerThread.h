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

    // for recursive refraction
    //real n1;
    int mID;

public:
    static int done_threads;

    RaytracerThread(SceneInput &_scene, const uint32_t _y, Camera &c) :
    scene(_scene), cam(c), curr_pixel(_y*c.width*3), y(_y),
     mID(-1)
    {viewing_ray.pos = c.Position;}

    RaytracerThread(const RaytracerThread &rt) : scene(rt.scene), cam(rt.cam), curr_pixel(rt.y*rt.cam.width*3), y(rt.y),
                                                                     mID(-1)
    {viewing_ray.pos = rt.cam.Position;}

    void drawRow();
    void computeViewingRay(uint32_t x);
    Color computeColor(Ray &ray, int depth, real n1 = 1.0, Color ac = Color(0,0,0));
    void writeColorToImage();
    void checkObjIntersection(Ray &ray,real &t_min, HitRecord &hit_record);
    bool isUnderShadow();
    void compute_shadow_ray(const HitRecord &hit_record,uint32_t i);
    static Color diffuseTerm(const HitRecord &hit_record, real cos_theta, Color I_R_2);
    Color specularTerm(const HitRecord &hit_record, const Ray &ray, real cos_theta, Color I_R_2) const;

    Color reflect(Ray &ray, int depth, MaterialType type, HitRecord &hit_record, real n1, Color ac);
    Color refract(Ray &ray, int depth, real n1, HitRecord &hit_record, Color ac);
    Ray refractionRay(Ray &ray, real n1, real n2, Vertex point,  Vec3r n, real &Fr, real &Ft);
};


#endif //CENG795_RAYTRACERTHREAD_H