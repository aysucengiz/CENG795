//
// Created by vicy on 10/14/25.
//

#include "raytracerThread.h"

#include <random>

#include "functions/overloads.h"
#include "../functions/helpers.h"
#include <stack>


std::mt19937 gRandomGeneratorT;

Color RaytracerThread::Filter(std::vector<Color> &colors, const std::vector<std::array<double,2>> &locs)
{
    if (colors.size() == 1) return colors[0];
    Color result = Color(0.0,0.0,0.0);
    int size = colors.size();
    switch (scene.filter_type)
    {
    case FilterType::GAUSSIAN:
        {
            // Here x and y are distances from the pixel center
            Color mean = Mean(colors);
            Color inv_stdev_2 = InvStdDev(mean,colors) *0.5;
            Color sumG = Color(0.0, 0.0, 0.0);
            Color sumGI = Color(0.0, 0.0, 0.0);
            for (int i=0; i< size; i++)
            {
                Color g = G(locs[i], inv_stdev_2);
                sumG  += g;
                sumGI += g * colors[i];
            }
            result = sumGI / sumG;
        }
        break;
    case FilterType::BOX:
    default:
        {
            for (int i=0; i< colors.size(); i++) result += colors[i];
            result = result / size;
        }
        break;
    }
    return result;
}

void RaytracerThread::writeToImage(uint32_t &curr_pixel, Color &final_color)
{
    scene.image[curr_pixel++] = clamp(final_color.r, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.g, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.b, 0, 255);
}

void RaytracerThread::drawPixel(uint32_t &curr_pixel, uint32_t x, uint32_t y)
{
    std::vector<Color> colors;
    colors.reserve(cam.numSamples);
    Ray viewing_ray;
    // std::shuffle(sampleIdxShuffled.begin(), sampleIdxShuffled.end(), gRandomGeneratorT);
    for (int i=0; i < cam.numSamples; i++)
    {
        viewing_ray = computeViewingRay(x, y, i);
        colors.push_back(computeColor(viewing_ray, 0, air));
    }
    Color final_color = Filter(colors,cam.samples);
    writeToImage(curr_pixel, final_color);
}

void RaytracerThread::PrintProgress()
{
    #pragma omp atomic
        done_threads++;
    #pragma omp critical
        {
            if (done_threads % scene.thread_group_size == 0)
                std::cout << done_threads / scene.thread_group_size << "\t";
            fflush(stdout);
            if (done_threads % scene.thread_add_endl_after == 0 || done_threads == cam.height)
                std::cout << std::endl;
        }
}

void RaytracerThread::drawRow(uint32_t y)
{
    uint32_t curr_pixel = y * cam.width * 3;
    uint32_t width = cam.width;
    Color final_color;
    for (uint32_t x = 0; x < width; x++)
    {
        drawPixel(curr_pixel, x, y);
    }

}

void RaytracerThread::drawBatch(uint32_t start_idx, uint32_t w, uint32_t h)
{
    uint32_t width = cam.width;
    uint32_t height = cam.height;
    uint32_t start_y = start_idx / width;
    uint32_t start_x = start_idx % width;
    uint32_t curr_pixel = start_idx * 3;
    uint32_t end_y = std::min(start_y + h, height);
    uint32_t end_x = std::min(start_x + w, width);
    uint32_t allw_batchw_3 = (width - (end_x - start_x)) * 3;
    Color final_color;
    for (uint32_t y = start_y; y < end_y; y++)
    {
        for (uint32_t x = start_x; x < end_x; x++)
        {
            drawPixel(curr_pixel, x, y);

        }
        curr_pixel += allw_batchw_3;
    }

}




Ray RaytracerThread::computeViewingRay(int x, int y, int i)
{
    Ray viewing_ray;
    real s_u = (x +cam.samples[i][0]) * (cam.r - cam.l) / cam.width;
    real s_v = (y +cam.samples[i][1]) * (cam.t - cam.b) / cam.height;
    Vertex s = scene.q + scene.u * s_u - cam.Up * s_v;

    if (cam.ApertureSize >0) viewing_ray.pos = cam.Position + cam.Up *cam.samples[sampleIdxShuffled[i]][0] + cam.V * cam.samples[sampleIdxShuffled[i]][1];
    else                viewing_ray.pos = cam.Position;
    viewing_ray.dir = s - viewing_ray.pos;
    viewing_ray.dir = viewing_ray.dir.normalize();
    time = 0;//getRandom(); // TODO: motion denemesi için aç
    return viewing_ray;
}

bool RaytracerThread::isUnderShadow(Ray& shadow_ray)
{
    real t_min = INFINITY;

    if (ACCELERATE)
    {
        if (bvh.traverse(shadow_ray, t_min, scene. objects, true, false, time).obj != nullptr)
            return true;
    }
    else
    {
        for (int j = 0; j < scene.numObjects; j++)
        {
            if (scene.objects[j]->checkIntersection(shadow_ray, t_min, true,false,time).obj != nullptr)
                return true;
        }
    }

    for (int j = scene.numObjects; j < scene.numPlanes; j++)
    {
        if (scene.objects[j]->checkIntersection(shadow_ray, t_min, true,false,time).obj != nullptr)
            return true;
    }
    return false;
}


Color RaytracerThread::computeColor(Ray& ray, int depth, const Material &m1)
{
    Color curr_color;
    HitRecord hit_record;
    real t_min= INFINITY;
    // std::cout << "Computecolor" << std::endl;

    if (depth > scene.MaxRecursionDepth) return curr_color;
    bool back_cull = m1.AbsorptionCoefficient.isBlack() ? scene.back_cull : false;
    checkObjIntersection(ray, t_min, hit_record,back_cull);

    if (hit_record.obj != nullptr &&
        hit_record.obj->material.materialType != MaterialType::NONE)
    {
        Material& m = hit_record.obj->material;
        curr_color = scene.AmbientLight * m.AmbientReflectance;
        if (m.materialType == MaterialType::MIRROR || m.materialType == MaterialType::CONDUCTOR)
        {
            curr_color += reflect(ray, depth, m.materialType, hit_record, m1);
        }
        if (m.materialType == MaterialType::DIELECTRIC)
        {
            if (m1.AbsorptionCoefficient.isBlack())
                curr_color += refract(ray, depth, m1,hit_record.obj->material, hit_record);
            else
                curr_color += refract(ray, depth, m1,air, hit_record);
        }

        {
            for (int i = 0; i < scene.numLights; i++)
            {
                Ray shadow_ray = compute_shadow_ray(hit_record, i);
                if (!isUnderShadow(shadow_ray))
                {
                    real cos_theta = dot_product(shadow_ray.dir.normalize(), hit_record.normal.normalize());
                    Color irradiance = scene.PointLights[i]->getIrradianceAt(hit_record.intersection_point);
                    if (cos_theta > 0)
                    {
                        //std::cout << "Draw" << std::endl;
                        curr_color += diffuseTerm(hit_record, cos_theta, irradiance) + specularTerm(
                            hit_record, ray, cos_theta, irradiance, shadow_ray);
                        Color ac1 = m1.AbsorptionCoefficient;
                        if (!ac1.isBlack())
                        {
                            Color eCx = (-ac1 * (ray.pos - hit_record.intersection_point ).mag()).exponent();
                            curr_color = curr_color * eCx;
                            // std::cout << eCx << " " << ac1 << " " << air.AbsorptionCoefficient <<std::endl;
                        }
                    } // else std::cout << cos_theta << " " <<hit_record.normal  << std::endl;

                    //curr_color.r = hit_record.normal.i * 255.0;
                    //curr_color.g = hit_record.normal.j * 255.0;
                    //curr_color.b = hit_record.normal.k * 255.0;
                }
            }
        }
    }
    else if (depth == 0)
    {
        return scene.BackgroundColor;
    }


    return curr_color;
}

Color RaytracerThread::diffuseTerm(const HitRecord& hit_record, real cos_theta, Color I_R_2)
{
    return hit_record.obj->material.DiffuseReflectance * I_R_2 * cos_theta;
}

Color RaytracerThread::specularTerm(const HitRecord& hit_record, const Ray& ray, real cos_theta, Color I_R_2,
                                    Ray& shadow_ray) const
{
    Material& m = hit_record.obj->material;
    if (m.SpecularReflectance.isBlack()) return Color();
    Vec3r h = (shadow_ray.dir.normalize() - ray.dir.normalize()).normalize();
    real cos_alpha = dot_product(hit_record.normal, h);
    if (cos_alpha < 0) return Color();
    return m.SpecularReflectance * I_R_2 * pow(cos_alpha, m.PhongExponent);
}

Ray RaytracerThread::compute_shadow_ray(const HitRecord& hit_record, uint32_t i)
{
    Ray shadow_ray;
    shadow_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    shadow_ray.dir = scene.PointLights[i]->Position - shadow_ray.pos;
    return shadow_ray;
}


void RaytracerThread::checkObjIntersection(Ray& ray, real& t_min, HitRecord& hit_record, bool back_cull)
{
    hit_record.obj = nullptr;
    Object::intersectResult temp_obj;
    temp_obj.obj = nullptr;
    temp_obj.t_min = INFINITY;
    t_min = INFINITY;

    for (int i = scene.numObjects; i < scene.numPlanes; i++)
    {
        temp_obj = scene.objects[i]->checkIntersection(ray, t_min, false, back_cull,time);

        if (temp_obj.obj != nullptr)
        {
            hit_record.obj = temp_obj.obj;
            t_min = temp_obj.t_min;
            hit_record.currTri = temp_obj.currTri;
        }
    }

    if (ACCELERATE)
    {
        temp_obj= bvh.traverse(ray, t_min, scene.objects, false, back_cull,time);
        if (temp_obj.obj != nullptr)
        {
            hit_record.obj = temp_obj.obj;
            hit_record.currTri = temp_obj.currTri;
            t_min = temp_obj.t_min;
        }
    }
    else
    {
        for (int i = 0; i < scene.numObjects; i++)
        {
            temp_obj = scene.objects[i]->checkIntersection(ray, t_min, false,back_cull,time);

            if (temp_obj.obj != nullptr)
            {
                hit_record.currTri = temp_obj.currTri;
                hit_record.obj = temp_obj.obj;
                t_min = temp_obj.t_min;
            }
        }
    }

    t_min = temp_obj.t_min;
    if (hit_record.obj != nullptr)
    {
        hit_record.intersection_point = ray.pos + ray.dir * temp_obj.t_min;
        hit_record.normal = hit_record.obj->getNormal(hit_record.intersection_point,hit_record.currTri, time);

    }
}

Ray RaytracerThread::refractionRay(Ray& ray, real n1, real n2, Vertex point, Vec3r n, real& Fr, real& Ft)
{
    real cos_theta = -dot_product(n, ray.dir);
    Vec3r n_refr = n.normalize();

    if (cos_theta < 0)
    {
        cos_theta = -cos_theta;
        n_refr = -n_refr;
    }

    if (cos_theta > 1) cos_theta = 1;

    real n1_n2 = n1 / n2;
    real cos_phi2 = 1 - n1_n2* n1_n2 * (1 - cos_theta*cos_theta);
    Ray refracted_ray;


    if (cos_phi2 < 0)
    {
        // std::cout << cos_phi2 << std::endl;
        Fr = 1.0;
        Ft = 0.0;
        return refracted_ray;
    }
    real cos_phi = sqrt(cos_phi2);

    real parall = (n2 * cos_theta - n1 * cos_phi) / (n2 * cos_theta + n1 * cos_phi);
    real perp = (n1 * cos_theta - n2 * cos_phi) / (n1 * cos_theta + n2 * cos_phi);

    Fr = 0.5 * (parall*parall + perp*perp);
    //std::cout << Fr << " " << parall << " " << perp << std::endl;
    if (Fr > 1.0) Fr = 1.0;
    Ft = 1.0 - Fr;
    refracted_ray.dir = (ray.dir.normalize()+ n_refr * cos_theta) * n1_n2 - n_refr * cos_phi;
    refracted_ray.dir = refracted_ray.dir.normalize();
    refracted_ray.pos = point + refracted_ray.dir * scene.IntersectionTestEpsilon;
    return refracted_ray;
}


Color RaytracerThread::refract(Ray& ray, int depth, const Material &m1, const Material &m2, HitRecord& hit_record)
{
    if (depth > scene.MaxRecursionDepth) return Color();
    Color reflected, refracted;
    real Fr = 0.0, Ft = 1.0;

    // info about curr material
    real n1 = m1.RefractionIndex;
    Color ac1 = m1.AbsorptionCoefficient;

    // info about entering material
    real n2 = m2.RefractionIndex;

    Ray refractedRay = refractionRay(ray, n1, n2, hit_record.intersection_point, hit_record.normal, Fr, Ft);

    if (Fr > 0.0)
    {
        if (dot_product(ray.dir, hit_record.normal) < 0)
        {   // entering
            reflected = reflect(ray, depth + 1,MaterialType::DIELECTRIC, hit_record,m1) * Fr;
        }
        else
        {   // (not) leaving
            Ray reflection_ray = reflectionRay(ray,MaterialType::DIELECTRIC, hit_record);
            HitRecord temp_hit_record;
            real t_min = INFINITY;
            checkObjIntersection(reflection_ray,t_min,temp_hit_record,false);
            reflected = refract(reflection_ray, depth + 1, m1,m2, temp_hit_record) * Fr;
        }// std::cout << m2.AbsorptionCoefficient << std::endl;

    }
    if (Ft > 0.0)
    {
        if (dot_product(ray.dir, hit_record.normal) < 0)
        {   // entering
            HitRecord temp_hit_record;
            real t_min = INFINITY;
            checkObjIntersection(refractedRay,t_min,temp_hit_record,false);
            refracted = refract(refractedRay, depth + 1, m2,m1, temp_hit_record) * Ft;
        }
        else
        {   // leaving
            refracted = computeColor(refractedRay,depth + 1, m2) * Ft;
        }
    }

    //std::cout << "Refracted" << std::endl;
    Color eCx = Color(1.0,1.0,1.0);
    if (!ac1.isBlack()) eCx = (-ac1 * (ray.pos - hit_record.intersection_point ).mag()).exponent();
    return (reflected + refracted)* eCx;
}

Ray RaytracerThread::reflectionRay(Ray& ray,MaterialType type, HitRecord& hit_record)
{
    Ray reflected_ray;
    Vec3r n = hit_record.normal.normalize();

    real cos_theta = dot_product(n, -ray.dir.normalize());
    if (cos_theta < 0)
    {
        n = -n;
        cos_theta = -cos_theta;
    }
    double epsilon = scene.ShadowRayEpsilon;
    if (MaterialType::DIELECTRIC == type)epsilon = scene.IntersectionTestEpsilon;
    reflected_ray.pos = hit_record.intersection_point + n * epsilon;
    reflected_ray.dir = ray.dir + n * 2 * cos_theta;
    return reflected_ray;
}



Color RaytracerThread::reflect(Ray& ray, int depth, MaterialType type, HitRecord& hit_record, const Material &m1)
{
    if (depth > scene.MaxRecursionDepth) return Color();
    //std::cout << "Reflecting" << std::endl;


    Vec3r n = hit_record.normal.normalize();
    real cos_theta = dot_product(n, -ray.dir.normalize());
    if (cos_theta < 0)
    {
        n = -n;
        cos_theta = -cos_theta;
    }

    Ray reflected_ray = reflectionRay(ray,type,hit_record);
    Color temp;
    temp = computeColor(reflected_ray, depth + 1, m1);
    // std::cout << hit_record.normal << std::endl;

    if (type == MaterialType::CONDUCTOR)
    {
        Material& m = hit_record.obj->material;
        real n2_k2 = pow(m.RefractionIndex, 2) + pow(m.AbsorptionIndex, 2);
        real cos_theta_2 = pow(cos_theta, 2);
        real n2_cos_theta_2 = 2 * m.RefractionIndex * cos_theta;
        real n2_k2_cos_theta_2 = n2_k2 * cos_theta_2;
        real Rs = (n2_k2 - n2_cos_theta_2 + cos_theta_2) / (n2_k2 + n2_cos_theta_2 + cos_theta_2);
        real Rp = (n2_k2_cos_theta_2 - n2_cos_theta_2 + 1) / (n2_k2_cos_theta_2 + n2_cos_theta_2 + 1);
        real Fr = 0.5 * (Rs + Rp);
        return temp * hit_record.obj->material.MirrorReflectance * Fr;
    }
    // std::cout << "Reflected" << std::endl;

    if (type == MaterialType::DIELECTRIC) return temp;
    return temp * hit_record.obj->material.MirrorReflectance;
}
