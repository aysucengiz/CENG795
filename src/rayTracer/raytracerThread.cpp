//
// Created by vicy on 10/14/25.
//

#include "raytracerThread.h"

void RaytracerThread::drawRow()
{
    for (uint32_t x = 0; x < cam.width; x++)
    {
        computeViewingRay(x);
        final_color = computeColor(viewing_ray, 0);
        writeColorToImage();
    }

}


void RaytracerThread::writeColorToImage()
{
    scene.image[curr_pixel++] = clamp(final_color.r, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.g, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.b, 0, 255);
}



void RaytracerThread::computeViewingRay(uint32_t x)
{

    real s_u = (x + 0.5) *(cam.r - cam.l) / cam.width;
    real s_v = (y + 0.5) * (cam.t - cam.b) / cam.height;
    Vertex s = scene.q + scene.u*s_u - cam.Up * s_v;

    viewing_ray.dir = s - cam.Position;
    viewing_ray.dir = viewing_ray.dir.normalize();
}

bool RaytracerThread::isUnderShadow()
{
    for (int j = 0; j < scene.numObjects; j++)
    {
        if (scene.objects[j]->checkIntersection(shadow_ray, t_min, true) != nullptr)
            return true;
    }
    return false;
}


Color RaytracerThread::computeColor(Ray &ray, int depth)
{
    Color curr_color;

    if (depth > scene.MaxRecursionDepth) return curr_color;

    checkObjIntersection(ray);

    if (hit_record.obj != nullptr &&  hit_record.obj->material.materialType != MaterialType::NONE)
    {

        computeHitRecord(ray);

        Material &m = hit_record.obj->material;
        curr_color = scene.AmbientLight * m.AmbientReflectance;
        if (m.materialType == MaterialType::MIRROR)
        {
            curr_color += reflect(ray, depth) * m.MirrorReflectance;
        }
        for (int i=0; i < scene.numLights; i++)
        {
            compute_shadow_ray(i);
            if (!isUnderShadow())
            {
                real cos_theta = dot_product(shadow_ray.dir.normalize(), hit_record.normal);
                Color I_R_2 = scene.PointLights[i].Intensity / dot_product(shadow_ray.dir, shadow_ray.dir);
                if ( cos_theta > 0)
                {
                    //std::cout << "Draw" << std::endl;
                    curr_color += diffuseTerm(cos_theta,I_R_2) + specularTerm(ray, cos_theta,I_R_2);
                }// else std::cout << cos_theta << " " <<hit_record.normal  << std::endl;

            }
        }

    }
    else if (depth == 0)
    {
        return scene.BackgroundColor;
    }


    return curr_color;
}

Color RaytracerThread::diffuseTerm(real cos_theta, Color I_R_2){
    return hit_record.obj->material.DiffuseReflectance * I_R_2 * cos_theta;
}

Color RaytracerThread::specularTerm(Ray &ray, real cos_theta, Color I_R_2)
{
    Material &m = hit_record.obj->material;
    Vec3r h = (shadow_ray.dir.normalize() - ray.dir).normalize();
    real cos_alpha = dot_product(hit_record.normal, h);
    if (cos_alpha < 0) return Color();
    return m.SpecularReflectance * I_R_2 * pow(cos_alpha, m.PhongExponent);
}

void RaytracerThread::compute_shadow_ray(uint32_t i)
{
    shadow_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    shadow_ray.dir = scene.PointLights[i].Position - shadow_ray.pos;
}


void RaytracerThread::checkObjIntersection(Ray &ray)
{
    t_min = INFINITY;
    hit_record.obj = nullptr;
    Object *temp_obj = nullptr;

    for(int i = 0; i < scene.numObjects; i++)
    {
        temp_obj = scene.objects[i]->checkIntersection(ray, t_min,  false);
        if (temp_obj != nullptr)
        {
            hit_record.obj = temp_obj;
        }
    }


}


Color RaytracerThread::reflect(Ray &ray, int depth)
{
    //std::cout << "Reflecting" << std::endl;
    Ray reflected_ray;
    reflected_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    reflected_ray.dir = ray.dir + hit_record.normal*2*dot_product(hit_record.normal,-ray.dir);
    HitRecord temp_hit_record = hit_record;
    Color temp = computeColor(reflected_ray, depth+1);
    hit_record = temp_hit_record;
    return  temp;
}

void RaytracerThread::computeHitRecord(Ray &ray)
{
    hit_record.intersection_point = ray.pos + ray.dir * t_min;
    hit_record.normal = hit_record.obj->getNormal(hit_record.intersection_point);
}


