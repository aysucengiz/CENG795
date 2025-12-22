//
// Created by vicy on 10/14/25.
//

#include "raytracerThread.h"

#include <random>

#include "functions/overloads.h"
#include "../functions/helpers.h"
#include <stack>

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

void RaytracerThread::drawRow(uint32_t ly)
{
    uint32_t curr_pixel = ly * cam.width * 3;
    uint32_t width = cam.width;
    Color final_color;
    y = ly;
    for (uint32_t lx = 0; lx < width; lx++)
    {
        x = lx;
        drawPixel(curr_pixel);
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
    for (uint32_t ly = start_y; ly < end_y; ly++)
    {
        y = ly;
        for (uint32_t lx = start_x; lx < end_x; lx++)
        {
            x = lx;
            // if (x==466 && y==457)
                drawPixel(curr_pixel);
        }
        curr_pixel += allw_batchw_3;
    }
    if (scene.print_progress) PrintProgress();
}


Color RaytracerThread::Filter(std::vector<Color>& colors, const std::vector<std::array<real, 2>>& locs)
{
    if (colors.size() == 1) return colors[0];
    Color result = Color(0.0, 0.0, 0.0);
    int size = colors.size();
    switch (scene.filter_type)
    {
    case FilterType::GAUSSIAN:
        {
            // Here x and y are distances from the pixel center
            Color mean = Mean(colors);
            real inv_stdev_2 = InvStdDev(mean, colors) * 0.5;
            real sumG = 0.0;
            Color sumGI = Color(0.0, 0.0, 0.0);
            if (inv_stdev_2 == INFINITY)
            {
                result = colors[0];
            }
            else
            {
                for (int j = 0; j < size; j++)
                {
                    real g = G(locs[j], inv_stdev_2);
                    sumG += g;
                    sumGI += colors[j] * g;
                }
                result = sumGI / sumG;
            }
        }
        break;
    case FilterType::BOX:
        {
            for (int j = 0; j < colors.size(); j++) result += colors[j];
            result = result / size;
        }
        break;
    default:
        result = colors[0];
    }
    return result;
}

void RaytracerThread::writeToImage(uint32_t& curr_pixel, Color& final_color)
{
    scene.image[curr_pixel++] = clamp(final_color.r, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.g, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.b, 0, 255);
}


void RaytracerThread::drawPixel(uint32_t& curr_pixel)
{
    std::vector<Color> colors;
    colors.reserve(cam.numSamples);
    Ray viewing_ray;

    std::mt19937 gRandomGeneratorP(919 + x + y);
    std::shuffle(sampleIdxPixel.begin(), sampleIdxPixel.end(), gRandomGeneratorP);
    std::mt19937 gRandomGeneratorL(42 + x + y);
    std::shuffle(sampleIdxLight.begin(), sampleIdxLight.end(), gRandomGeneratorL);
    std::mt19937 gRandomGeneratorT(133 + x + y);
    std::shuffle(sampleIdxTime.begin(), sampleIdxTime.end(), gRandomGeneratorT);
    std::mt19937 gRandomGeneratorG(237 + x + y);
    std::shuffle(sampleIdxGlossy.begin(), sampleIdxGlossy.end(), gRandomGeneratorG);
    for (sampleIdx = 0; sampleIdx < cam.numSamples; sampleIdx++)
    {
        viewing_ray = computeViewingRay(x, y);
        colors.push_back(followRay(viewing_ray, 0, air, cam.samplesLight[sampleIdxLight[sampleIdx]]));
    }

    Color final_color = Filter(colors, cam.samplesPixel);
    writeToImage(curr_pixel, final_color);
}


Ray RaytracerThread::computeViewingRay(int x_loc, int y_loc)
{
    Ray viewing_ray;
    real s_u, s_v;
    s_u = (x_loc + cam.samplesPixel[sampleIdxPixel[sampleIdx]][0]) * (cam.r - cam.l) / cam.width;
    s_v = (y_loc + cam.samplesPixel[sampleIdxPixel[sampleIdx]][1]) * (cam.t - cam.b) / cam.height;
    Vertex s = scene.q + scene.u * s_u - cam.Up * s_v;

    viewing_ray.pos = cam.getPos(sampleIdx);
    Vec3r dir = s - cam.Position;

    if (cam.ApertureSize > 0)
    {
        real t_fp = cam.FocusDistance / dot_product(dir, cam.Gaze);
        viewing_ray.dir = (cam.Position + dir * t_fp) - viewing_ray.pos;
    }
    else
    {
        viewing_ray.dir = dir;
    }

    viewing_ray.dir = viewing_ray.dir.normalize();
    time = cam.samplesTime[sampleIdxTime[sampleIdx]];
    return viewing_ray;
}

bool RaytracerThread::isUnderShadow(Ray& shadow_ray)
{
    real t_min = INFINITY;

    if (ACCELERATE)
    {
        if (bvh.traverse(shadow_ray, t_min, scene.objects, true, false, time).obj != nullptr)
            return true;
    }
    else
    {
        for (int j = 0; j < scene.numObjects; j++)
        {
            if (scene.objects[j]->checkIntersection(shadow_ray, t_min, true, false, time).obj != nullptr)
                return true;
        }
    }

    for (int j = scene.numObjects; j < scene.numPlanes; j++)
    {
        if (scene.objects[j]->checkIntersection(shadow_ray, t_min, true, false, time).obj != nullptr)
            return true;
    }
    return false;
}

Color RaytracerThread::computeColor(HitRecord& hit_record, Ray& ray, int depth, const Material& m1,
                                    const std::array<real, 2>& light_sample)
{
    // if ((hit_record.obj->AllTexture && hit_record.obj->AllTexture->getTextureType() == TextureType::CHECKERBOARD) ||
    //     (hit_record.obj->DiffuseTexture && hit_record.obj->DiffuseTexture->getTextureType() == TextureType::CHECKERBOARD))
    //     std::cout << hit_record.obj->_id << std::endl;
    if (hit_record.obj->AllTexture != nullptr)
    {
        return hit_record.obj->getTextureColorAt(hit_record.intersection_point, time, hit_record.currTri, hit_record.rate_of_change);
    }
    Color curr_color;
    Material& m = hit_record.obj->material;
    curr_color = scene.AmbientLight * m.AmbientReflectance;
    if (m.materialType == MaterialType::MIRROR || m.materialType == MaterialType::CONDUCTOR)
    {
        curr_color += reflect(ray, depth, m.materialType, hit_record, m1);
    }
    if (m.materialType == MaterialType::DIELECTRIC)
    {
        if (m1.AbsorptionCoefficient.isBlack())
            curr_color += refract(ray, depth, m1, hit_record.obj->material, hit_record);
        else
            curr_color += refract(ray, depth, m1, air, hit_record);
    }

    {
        for (int j = 0; j < scene.numLights; j++)
        {
            // std::array<real ,2> light_sample = {getRandom(),getRandom()};
            Ray shadow_ray = compute_shadow_ray(hit_record, j, light_sample);
            if (!isUnderShadow(shadow_ray))
            {
                Color irradiance = scene.PointLights[j]->getIrradianceAt(
                    hit_record.normal, light_sample, shadow_ray,
                    (hit_record.intersection_point - scene.PointLights[j]->getPos(light_sample)).mag());
                if (!irradiance.isBlack())
                {
                    //std::cout << "Draw" << std::endl;

                    real cos_theta = dot_product(shadow_ray.dir.normalize(), hit_record.normal.normalize());


                    curr_color += hit_record.obj->GetColourAt(irradiance, cos_theta, hit_record.normal, ray, shadow_ray,
                                                              time, hit_record.currTri, hit_record.rate_of_change);
                    Color ac1 = m1.AbsorptionCoefficient;
                    if (!ac1.isBlack())
                    {
                        Color eCx = (-ac1 * (ray.pos - hit_record.intersection_point).mag()).exponent();
                        curr_color = curr_color * eCx;
                        // std::cout << eCx << " " << ac1 << " " << air.AbsorptionCoefficient <<std::endl;
                    }
                } // else std::cout << cos_theta << " " <<hit_record.normal  << std::endl;

                // curr_color.r = hit_record.normal.i * 255.0;
                // curr_color.g = hit_record.normal.j * 255.0;
                // curr_color.b = hit_record.normal.k * 255.0;
            }
        }
    }
    return curr_color;
}


Color RaytracerThread::followRay(Ray& ray, int depth, const Material& m1, const std::array<real, 2>& light_sample)
{
    HitRecord hit_record;
    real t_min = INFINITY;

    if (depth > scene.MaxRecursionDepth) return Color(0.0, 0.0, 0.0);
    bool back_cull = m1.AbsorptionCoefficient.isBlack() ? scene.back_cull : false;
    checkObjIntersection(ray, t_min, hit_record, back_cull);

    if (hit_record.obj != nullptr &&
        hit_record.obj->material.materialType != MaterialType::NONE)
    {
        return computeColor(hit_record, ray, depth, m1, light_sample);
    }

    if (depth == 0)
    {
        if (scene.BackgroundTexture != nullptr)
        {
            Texel t((real)x / (real)cam.width, (real)y / (real)cam.height);
            Vertex v(t.u, t.v, 0.0);
            Color bg = scene.BackgroundTexture->TextureColor(v, t, 0) * 255.0;
            // std::cout << bg << std::endl;
            return bg;
        }
        return scene.BackgroundColor;
    }
    return Color(0.0, 0.0, 0.0);
}


Ray RaytracerThread::compute_shadow_ray(const HitRecord& hit_record, uint32_t lightIdx,
                                        std::array<real, 2> sample) const
{
    Ray shadow_ray;
    shadow_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    Vertex pospos = scene.PointLights[lightIdx]->getPos(sample);
    shadow_ray.dir = pospos - shadow_ray.pos;
    return shadow_ray;
}


void RaytracerThread::checkObjIntersection(Ray& ray, real& t_min, HitRecord& hit_record, bool back_cull)
{
    hit_record.obj = nullptr;
    Object::intersectResult temp_obj;
    temp_obj.obj = nullptr;
    temp_obj.t_min = INFINITY;
    t_min = INFINITY;

    for (int j = scene.numObjects; j < scene.numPlanes; j++)
    {
        temp_obj = scene.objects[j]->checkIntersection(ray, t_min, false, back_cull, time);

        if (temp_obj.obj != nullptr)
        {
            hit_record.obj = temp_obj.obj;
            t_min = temp_obj.t_min;
            hit_record.currTri = temp_obj.currTri;
        }
    }

    if (ACCELERATE)
    {
        temp_obj = bvh.traverse(ray, t_min, scene.objects, false, back_cull, time);
        if (temp_obj.obj != nullptr)
        {
            hit_record.obj = temp_obj.obj;
            hit_record.currTri = temp_obj.currTri;
            t_min = temp_obj.t_min;
        }
    }
    else
    {
        for (int j = 0; j < scene.numObjects; j++)
        {
            temp_obj = scene.objects[j]->checkIntersection(ray, t_min, false, back_cull, time);

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
        hit_record.normal = hit_record.obj->getNormal(hit_record.intersection_point, hit_record.currTri, time);

        if ((hit_record.obj->DiffuseTexture != nullptr && hit_record.obj->DiffuseTexture->IsMipMapped())
            || (hit_record.obj->SpecularTexture != nullptr && hit_record.obj->SpecularTexture->IsMipMapped())
            || (hit_record.obj->NormalTexture != nullptr && hit_record.obj->NormalTexture->IsMipMapped())
            || (hit_record.obj->AllTexture != nullptr && hit_record.obj->AllTexture->IsMipMapped()))
        {

            const Vertex& a = hit_record.intersection_point;
            const Vec3r& n = hit_record.normal;
            Vec3r T, B;
            hit_record.obj->getBitan(hit_record.intersection_point,T,B,hit_record.currTri,false, time);
            Texel dUV_i(0.0,0.0);
            Texel dUV_j(0.0,0.0);

            Ray dx = computeViewingRay(x + 1, y);
            Ray dy = computeViewingRay(x, y + 1);

            real tx = dot_product(n, a - dx.pos)/dot_product(n,dx.dir);
            real ty = dot_product(n, a - dy.pos)/dot_product(n,dy.dir);

            Vertex ax = dx.pos + dx.dir * tx;
            Vertex ay = dy.pos + dy.dir * ty;

            Vec3r dP_di = ax - a;
            Vec3r dP_dj = ay - a;

            // buralara kadar sorun yok


            real max_axis = std::max(abs(n.i), std::max(abs(n.j), abs(n.k)));
            Axes discarded = Axes::z;
            if (abs(n.i) > abs(n.j) && abs(n.i) > abs(n.k))
                discarded = Axes::x;
            else if (abs(n.j) > abs(n.k))
                discarded = Axes::y;

            std::array<std::array<real, 2>, 2> A{}, invA{};
            std::array<real, 2> dP_di_2;
            std::array<real, 2> dP_dj_2;
            std::array<Axes,2> use;

            use[0] = discarded != Axes::x ? Axes::x : Axes::y;
            use[1] = discarded != Axes::y ? Axes::y : Axes::z;

            dP_di_2[0] = dP_di[use[0]];
            dP_di_2[1] = dP_di[use[1]];

            dP_dj_2[0] =  dP_dj[use[0]];
            dP_dj_2[1] =  dP_dj[use[1]];

            A = {
                T[use[0]], B[use[0]],
                T[use[1]], B[use[1]]
            };

            real detA = 1 / (A[0][0] * A[1][1] - A[0][1] * A[1][0]);

            invA ={
                detA * A[1][1], -detA*A[1][0],
                -detA*A[0][1],  detA*A[0][0],
            };

            dUV_i.u = invA[0][0]*dP_di_2[0] + invA[0][1]*dP_di_2[1];
            dUV_i.v = invA[1][0]*dP_di_2[0] + invA[1][1]*dP_di_2[1];

            dUV_j.u = invA[0][0]*dP_dj_2[0] + invA[0][1]*dP_dj_2[1];
            dUV_j.v = invA[1][0]*dP_dj_2[0] + invA[1][1]*dP_dj_2[1];

            real mag_i = dUV_i.v*dUV_i.v + dUV_i.u*dUV_i.u;
            real mag_j = dUV_j.v*dUV_j.v + dUV_j.u*dUV_j.u;

            hit_record.rate_of_change = dUV_j;

            if (std::max(mag_i, mag_j) == mag_i)
            {
                hit_record.rate_of_change = dUV_i;
            }
        }
    }
}

Ray RaytracerThread::refractionRay(Ray& ray, real n1, real n2, Vertex point, Vec3r n, real& Fr, real& Ft,
                                   real roughness)
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
    real cos_phi2 = 1 - n1_n2 * n1_n2 * (1 - cos_theta * cos_theta);
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

    Fr = 0.5 * (parall * parall + perp * perp);
    //std::cout << Fr << " " << parall << " " << perp << std::endl;
    if (Fr > 1.0) Fr = 1.0;
    Ft = 1.0 - Fr;
    refracted_ray.dir = (ray.dir.normalize() + n_refr * cos_theta) * n1_n2 - n_refr * cos_phi;
    refracted_ray.shiftRayBy({getRandom(), getRandom()}, roughness);
    refracted_ray.dir = refracted_ray.dir.normalize();
    refracted_ray.pos = point + refracted_ray.dir * scene.IntersectionTestEpsilon;
    return refracted_ray;
}


Color RaytracerThread::refract(Ray& ray, int depth, const Material& m1, const Material& m2, HitRecord& hit_record)
{
    if (depth > scene.MaxRecursionDepth)
    {
        Color ac1 = m1.AbsorptionCoefficient;
        Color eCx = Color(1.0, 1.0, 1.0);
        if (!ac1.isBlack())
            eCx = (-ac1 * (ray.pos - hit_record.intersection_point).mag()).exponent();
        return eCx;
    }
    Color reflected, refracted;
    real Fr = 0.0, Ft = 1.0;

    // info about curr material
    real n1 = m1.RefractionIndex;
    Color ac1 = m1.AbsorptionCoefficient;

    // info about entering material
    real n2 = m2.RefractionIndex;

    Ray refractedRay = refractionRay(ray, n1, n2, hit_record.intersection_point, hit_record.normal, Fr, Ft,
                                     hit_record.obj->material.Roughness);

    if (Fr > 0.0)
    {
        if (dot_product(ray.dir, hit_record.normal) < 0)
        {
            // entering
            reflected = reflect(ray, depth, MaterialType::DIELECTRIC, hit_record, m1) * Fr;
        }
        else
        {
            // (not) leaving
            Ray reflection_ray = reflectionRay(ray, MaterialType::DIELECTRIC, hit_record);
            HitRecord temp_hit_record;
            real t_min = INFINITY;
            checkObjIntersection(reflection_ray, t_min, temp_hit_record, false);
            if (temp_hit_record.obj != nullptr)
                reflected = refract(reflection_ray, depth + 1, m1, m2, temp_hit_record)
                    * Fr;
        } // std::cout << m2.AbsorptionCoefficient << std::endl;
    }
    if (Ft > 0.0)
    {
        if (dot_product(ray.dir, hit_record.normal) < 0)
        {
            // entering
            HitRecord temp_hit_record;
            real t_min = INFINITY;
            checkObjIntersection(refractedRay, t_min, temp_hit_record, false);
            if (temp_hit_record.obj != nullptr)
                refracted = refract(refractedRay, depth + 1, m2, m1, temp_hit_record) *
                    Ft;
        }
        else
        {
            // leaving
            std::array<real, 2> sample = {getRandom(), getRandom()};
            refracted = followRay(refractedRay, depth, m2, sample) * Ft;
        }
    }

    //std::cout << "Refracted" << std::endl;
    Color eCx = Color(1.0, 1.0, 1.0);
    if (!ac1.isBlack()) eCx = (-ac1 * (ray.pos - hit_record.intersection_point).mag()).exponent();
    if ((reflected + refracted - eCx).isBlack()) return eCx;
    return (reflected + refracted) * eCx;
}

Ray RaytracerThread::reflectionRay(Ray& ray, MaterialType type, HitRecord& hit_record)
{
    Ray reflected_ray;
    Vec3r n = hit_record.normal.normalize();


    // std::cout << n << std::endl;

    real cos_theta = dot_product(n, -ray.dir.normalize());
    if (cos_theta < 0)
    {
        n = -n;
        cos_theta = -cos_theta;
    }

    real epsilon = scene.ShadowRayEpsilon;
    if (MaterialType::DIELECTRIC == type)epsilon = scene.IntersectionTestEpsilon;
    reflected_ray.pos = hit_record.intersection_point + n * epsilon;
    reflected_ray.dir = ray.dir + n * 2 * cos_theta;
    reflected_ray.shiftRayBy({getRandom(), getRandom()}, hit_record.obj->material.Roughness);
    reflected_ray.dir = reflected_ray.dir.normalize();
    return reflected_ray;
}


Color RaytracerThread::reflect(Ray& ray, int depth, MaterialType type, HitRecord& hit_record, const Material& m1)
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

    Ray reflected_ray = reflectionRay(ray, type, hit_record);
    Color temp;
    std::array<real, 2> sample = {getRandom(), getRandom()};
    temp = followRay(reflected_ray, depth + 1, m1, sample);
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
