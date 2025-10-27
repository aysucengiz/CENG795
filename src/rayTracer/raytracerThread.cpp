//
// Created by vicy on 10/14/25.
//

#include "raytracerThread.h"

void RaytracerThread::drawRow()
{
    uint32_t width = cam.width;
    for (uint32_t x = 0; x < width; x++)
    {
        computeViewingRay(x);
        final_color = computeColor(viewing_ray, 0);
        writeColorToImage();
    }
    done_threads++;
    std::cout << done_threads << std::endl;

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
            curr_color += reflect(ray, depth);
        }
        else if (m.materialType == MaterialType::DIELECTRIC)
        {
            curr_color += refract(ray, depth);
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
    hit_record.mesh = nullptr;
    mID = -1;
    Object *temp_obj = nullptr;

    for(int i = 0; i < scene.numObjects; i++)
    {
        temp_obj = scene.objects[i]->checkIntersection(ray, t_min,  false);

        if (temp_obj != nullptr)
        {
            hit_record.obj = temp_obj;
            if (scene.objects[i]->getObjectType() == ObjectType::MESH)
            {
                hit_record.mesh = dynamic_cast<Mesh*>(scene.objects[i]);
            }
        }
    }


}

Ray RaytracerThread::refractionRay(Ray &ray, Material &m, Vec3r &n, real &Fr, real &Ft)
{

    real cos_theta = dot_product(n, -ray.dir);
    real n1_n2 = n1 / m.RefractionIndex;
    real cos_phi2 = 1 - pow(n1_n2,2) * ( 1 - pow(cos_theta,2));
    Ray refracted_ray;
    real cos_phi = sqrt(cos_phi2);

    if (cos_phi2 < 0)
    {
        if (m.materialType == MaterialType::DIELECTRIC)
        {
            real parall = ( m.RefractionIndex*cos_theta - n1*cos_phi)/ ( m.RefractionIndex*cos_theta + n1*cos_phi);
            real perp = (n1*cos_theta -  m.RefractionIndex*cos_phi)/ (n1*cos_theta +  m.RefractionIndex*cos_phi);
            Fr = 0.5 * (pow(parall,2) + pow(perp,2));
            Ft = 1 - Fr;
        }
        else if (m.materialType == MaterialType::CONDUCTOR)
        {
            real n2_k2 = pow( m.RefractionIndex,2) + pow(m.AbsorptionIndex,2);
            real cos_theta_2 = pow(cos_theta,2);
            real n2_cos_theta_2 = 2 *  m.RefractionIndex * cos_theta;
            real n2_k2_cos_theta_2 =n2_k2 * cos_theta_2;
            real Rs = (n2_k2 - n2_cos_theta_2 + cos_theta_2) / (n2_k2 + n2_cos_theta_2 + cos_theta_2);
            real Rp = (n2_k2_cos_theta_2 - n2_cos_theta_2 + 1) / (n2_k2_cos_theta_2 + n2_cos_theta_2 + 1);
            Fr = 0.5 * (Rs + Rp);
            Ft = 0.0;
        }
    }


    refracted_ray.pos = hit_record.intersection_point - hit_record.normal * scene.ShadowRayEpsilon; //TODO: bu lazım mı
    refracted_ray.dir = (ray.dir + hit_record.normal)*n1_n2 - hit_record.normal*cos_phi; // TODO: normal yönü??
    return refracted_ray;
}


Color RaytracerThread::refract(Ray &ray, int depth, bool outOfIt)
{
    //std::cout << "Refracting" << std::endl;

    if (depth > scene.MaxRecursionDepth) return Color();

    Color reflected, refracted;
    real Fr = 0.0, Ft = 1.0;
    Ray refractedRay = refractionRay(ray, hit_record.normal, Fr, Ft);
    if (Fr != 0.0) reflected = reflect(ray, depth) * Fr;
    if (Ft != 0.0)
    {
        HitRecord temp_hit_record = hit_record;
        int mID_prev = mID;
        if (outOfIt)
        {
            refracted = computeColor(refractedRay, depth + 1);
        }
        else
        {
            checkObjIntersection(refractedRay);
            real t_min_curr = t_min; // t_mini kaydet
            if (mID != -1 && mID == mID_prev)
            { // obje içinden çıkıyoruz, computecolor ve attenuationyapılacak
                refracted = refract(refractedRay, depth, true);
            }
            else
            {// bir obje içine giriyoruz, yine refraction yapılacak
                refracted = refract(refractedRay, depth + 1, false);
                mID = mID_prev;
            }

            // attenuationı ortak ekle
            Color eCx = exponent(scene.Materials[mID].AbsorptionCoefficient * (-t_min_curr));
            refracted = refracted * Ft * eCx;
        }
    }
    return reflected + refracted;
}


/*
Color RaytracerThread::refract(Ray &ray, int depth, bool addColor)
{
    real Fr = 0.0, Ft = 1.0;
    //std::cout << "Refracting" << std::endl;
    Color reflected, refracted;
    Material &m = hit_record.obj->material;

    // phi açısına göre refract/reflect belirlenecek
    Ray refractionRay = refractionRay(refractionRay, m2, n, Fr, Ft);
    // reflect varsa reflection

    // refraction
    //      refraction rayinin açısı bulunacak
    //          reflection varsa reflection yapılacak
    //      tüm objelerle intersectionı denenecek
    real t_min_refr = INFINITY;
    HitRecord temp_hit_record = hit_record; // store values that are changed by intersection and computecolor

    refracted_ray.dir = refracted_ray.dir.normalize();
    Object *finalobj = hit_record.obj->checkIntersection(refracted_ray, t_min_refr, false); // TODO: mesh olayını kaldır

    //      t_min kadar attenuation, n1 güncellenecek (geçici)
    real temp_n1 = n1;
    n1 = n2; // new index

    //      yeni refraction rayi belirlenip compute color yollanacak
    Fr = 0.0; Ft = 1.0;
    Ray refractionRay2 = refractionRay(refractionRay, m2, n2, Fr, Ft);
    Color refracted2, reflected2;
    if (Fr != 0.0) reflected2 = reflect(refractionRay, depth);
    if (Ft != 0.0)
    {
        // girdiğin objenin materyaliyle attenuation yap
        if (addColor) // 1) attenuation
        {
            Color eCx = exponent(m.AbsorptionCoefficient * (-t_min_refr));
            refracted2 = refract(ray, depth, true) * Ft * eCx;
        }
        else         // 2) computeColor -> hem reflection hem refractionın değerlerini topla
        {
            refracted2 = computeColor(refractionRay2, depth+1); // TODO: ya da +2??
        }


    }

    // refract
    //attenuate



    // restore values, return
    hit_record = temp_hit_record;
    n1 = temp_n1;
    return refracted + reflected + (refracted2 + reflected2)*m.AbsorptionCoefficient; // TODO: ac conductorlarda yok galiba?
}*/


Color RaytracerThread::reflect(Ray &ray, int depth)
{
    //std::cout << "Reflecting" << std::endl;
    Ray reflected_ray;
    reflected_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    reflected_ray.dir = ray.dir + hit_record.normal*2*dot_product(hit_record.normal,-ray.dir);
    HitRecord temp_hit_record = hit_record;
    Color temp = computeColor(reflected_ray, depth+1);
    hit_record = temp_hit_record;
    return  temp  * hit_record.obj->material.MirrorReflectance;
}

void RaytracerThread::computeHitRecord(Ray &ray)
{
    hit_record.intersection_point = ray.pos + ray.dir * t_min;
    hit_record.normal = hit_record.obj->getNormal(hit_record.intersection_point);
}


