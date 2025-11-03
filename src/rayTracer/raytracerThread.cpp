//
// Created by vicy on 10/14/25.
//

#include "raytracerThread.h"

// TODO: add back face culling

void RaytracerThread::drawRow()
{
    uint32_t width = cam.width;
    for (uint32_t x = 0; x < width; x++)
    {
        Ray viewing_ray = computeViewingRay(x);
        final_color = computeColor(viewing_ray, 0);
        writeColorToImage();
    }
    /*done_threads++;
    if (done_threads % 10 == 0)
        std::cout << done_threads << " ";
    fflush(stdout);
    if (done_threads % 400 == 0 || done_threads ==cam.height)
        std::cout << std::endl;*/


}


void RaytracerThread::writeColorToImage()
{
    scene.image[curr_pixel++] = clamp(final_color.r, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.g, 0, 255);
    scene.image[curr_pixel++] = clamp(final_color.b, 0, 255);
}



Ray RaytracerThread::computeViewingRay(uint32_t x)
{
    Ray viewing_ray;
    real s_u = (x + 0.5) *(cam.r - cam.l) / cam.width;
    real s_v = (y + 0.5) * (cam.t - cam.b) / cam.height;
    Vertex s = scene.q + scene.u*s_u - cam.Up * s_v;

    viewing_ray.dir = s - cam.Position;
    viewing_ray.dir = viewing_ray.dir.normalize();
    viewing_ray.pos = cam.Position;
    return viewing_ray;
}

bool RaytracerThread::isUnderShadow(Ray &shadow_ray)
{
    real t_min = INFINITY;
    for (int j = 0; j < scene.numObjects; j++)
    {
        if (scene.objects[j]->checkIntersection(shadow_ray, t_min, true) != nullptr)
            return true;
    }
    return false;
}


Color RaytracerThread::computeColor(Ray &ray, int depth, real n1, Color ac)
{
    Color curr_color;
    HitRecord hit_record;
    real t_min = INFINITY;

    if (depth > scene.MaxRecursionDepth) return curr_color;
    checkObjIntersection(ray, t_min, hit_record);

    if (hit_record.obj != nullptr &&
        hit_record.obj->material.materialType != MaterialType::NONE)
    {

        Material &m = hit_record.obj->material;
        curr_color = scene.AmbientLight * m.AmbientReflectance;
        //std::cout << "Before anything" << std::endl;
        if (m.materialType == MaterialType::MIRROR || m.materialType == MaterialType::CONDUCTOR)
        {
            curr_color += reflect(ray, depth, m.materialType, hit_record, n1,ac);
        }

        if (m.materialType == MaterialType::DIELECTRIC)
        {
            curr_color += refract(ray, depth, n1, hit_record,ac);
        }

        {
            for (int i=0; i < scene.numLights; i++)
            {
                Ray shadow_ray = compute_shadow_ray(hit_record, i);
                if (!isUnderShadow(shadow_ray))
                {

                    real cos_theta = dot_product(shadow_ray.dir.normalize(), hit_record.normal);
                    Color I_R_2 = scene.PointLights[i].Intensity / dot_product(shadow_ray.dir, shadow_ray.dir);
                    if ( cos_theta > 0)
                    {
                        //std::cout << "Draw" << std::endl;
                        curr_color += diffuseTerm(hit_record, cos_theta,I_R_2) + specularTerm(hit_record, ray, cos_theta,I_R_2, shadow_ray);
                        //if (!ac.isWhite()) curr_color = curr_color * exponent(ac * -(hit_record.intersection_point - ray.pos).mag());
                    }// else std::cout << cos_theta << " " <<hit_record.normal  << std::endl;

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

Color RaytracerThread::diffuseTerm(const HitRecord &hit_record, real cos_theta, Color I_R_2){
    return hit_record.obj->material.DiffuseReflectance * I_R_2 * cos_theta;
}

Color RaytracerThread::specularTerm(const HitRecord &hit_record, const Ray &ray, real cos_theta, Color I_R_2, Ray & shadow_ray) const
{
    Material &m = hit_record.obj->material;
    if (m.SpecularReflectance.isWhite()) return Color();
    Vec3r h = (shadow_ray.dir.normalize() - ray.dir).normalize();
    real cos_alpha = dot_product(hit_record.normal, h);
    if (cos_alpha < 0) return Color();
    return m.SpecularReflectance * I_R_2 * pow(cos_alpha, m.PhongExponent);
}

Ray RaytracerThread::compute_shadow_ray(const HitRecord &hit_record, uint32_t i)
{
    Ray shadow_ray;
    shadow_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    shadow_ray.dir = scene.PointLights[i].Position - shadow_ray.pos;
    return shadow_ray;
}


void RaytracerThread::checkObjIntersection(Ray &ray, real &t_min, HitRecord &hit_record)
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
            if (temp_obj->getObjectType() == ObjectType::MESH)
                hit_record.mesh = dynamic_cast<Mesh*> (scene.objects[i]);
            mID = temp_obj->material._id;
        }
    }

    if (hit_record.obj != nullptr)
    {
        hit_record.intersection_point = ray.pos + ray.dir * t_min;
        hit_record.normal = hit_record.obj->getNormal(hit_record.intersection_point);
    }


}

Ray RaytracerThread::refractionRay(Ray &ray, real n1, real n2, Vertex point, Vec3r n, real &Fr, real &Ft)
{

    real cos_theta = -dot_product(n, ray.dir);
    Vec3r n_refr = n.normalize();

    if (cos_theta < 0)
    {
        cos_theta = -cos_theta;
        n_refr = -n;

    }

    real n1_n2 = n1 / n2;
    real cos_phi2 = 1 - pow(n1_n2,2) * ( 1 - pow(cos_theta,2));
    Ray refracted_ray;


    if (cos_phi2 < 0)
    {
        Fr = 1.0; Ft = 0.0;
        return refracted_ray;
    }

    real cos_phi = sqrt(cos_phi2);

    real parall = ( n2*cos_theta - n1*cos_phi)/ ( n2*cos_theta + n1*cos_phi);
    real perp = (n1*cos_theta -  n2*cos_phi)/ (n1*cos_theta +  n2*cos_phi);

    Fr = 0.5 * (pow(parall,2) + pow(perp,2));
    if (Fr > 1.0) Fr = 1.0;
    Ft = 1.0 - Fr;

    refracted_ray.dir = (ray.dir.normalize() + n_refr*cos_theta)*n1_n2 - n_refr*cos_phi;
      refracted_ray.dir = refracted_ray.dir.normalize();
    refracted_ray.pos = point - n_refr * scene.ShadowRayEpsilon;
    return refracted_ray;
}


Color RaytracerThread::refract(Ray &ray, int depth, real n1, HitRecord &hit_record, Color ac)
{
    if (depth > scene.MaxRecursionDepth) return Color();

    Color reflected, refracted;
    real Fr = 0.0, Ft = 1.0;
    real n2 = hit_record.obj->material.RefractionIndex;
    Ray refractedRay = refractionRay(ray, n1, n2, hit_record.intersection_point, hit_record.normal, Fr, Ft);
    if (Fr > 0.0) reflected = reflect(ray, depth, MaterialType::MIRROR, hit_record, n1, ac) * Fr;
    if (Ft > 0.0)
    {   // enter the object (n2)
        HitRecord new_hit_record;
        real t_min = INFINITY;
        checkObjIntersection(refractedRay,t_min,new_hit_record);

        if (new_hit_record.obj != nullptr && hit_record.obj->material.materialType == MaterialType::DIELECTRIC)
        {

           // obje içinden çıkıyoruz, computecolor ve attenuationyapılacak
            real Fr2, Ft2;
            Color eCx = exponent(-hit_record.obj->material.AbsorptionCoefficient * (hit_record.intersection_point - new_hit_record.intersection_point).mag());
            Ray refractedRay2 = refractionRay(refractedRay, n2, n1, new_hit_record.intersection_point,new_hit_record.normal, Fr2, Ft2);
            if (Fr2 > 0.0) refracted += reflect(refractedRay, depth, MaterialType::DIELECTRIC, new_hit_record, n2, hit_record.obj->material.AbsorptionCoefficient) * Fr2;
            if (Ft2 > 0.0) refracted += computeColor(refractedRay2, depth +1, n1, ac) * Ft2 * eCx;

            // attenuation
            refracted = refracted* Ft;
        }

    }
    //std::cout << "Refracted" << std::endl;
    return reflected + refracted;
}



Color RaytracerThread::reflect(Ray &ray, int depth, MaterialType type, HitRecord &hit_record, real n1, Color ac)
{
    //std::cout << "Reflecting" << std::endl;
    Ray reflected_ray;
    Vec3r n = hit_record.normal;
    real cos_theta = dot_product(hit_record.normal,-ray.dir);
    if (cos_theta < 0){
        n = -n;
        cos_theta = -cos_theta;
    }
    reflected_ray.pos = hit_record.intersection_point + n * scene.ShadowRayEpsilon;
    reflected_ray.dir = ray.dir + n*2*cos_theta;
    Color temp = computeColor(reflected_ray, depth+1, n1, ac);

    if (type == MaterialType::CONDUCTOR)
    {
        Material &m = hit_record.obj->material;
        real n2_k2 = pow( m.RefractionIndex,2) + pow(m.AbsorptionIndex,2);
        real cos_theta_2 = pow(cos_theta,2);
        real n2_cos_theta_2 = 2 *  m.RefractionIndex * cos_theta;
        real n2_k2_cos_theta_2 =n2_k2 * cos_theta_2;
        real Rs = (n2_k2 - n2_cos_theta_2 + cos_theta_2) / (n2_k2 + n2_cos_theta_2 + cos_theta_2);
        real Rp = (n2_k2_cos_theta_2 - n2_cos_theta_2 + 1) / (n2_k2_cos_theta_2 + n2_cos_theta_2 + 1);
        real Fr = 0.5 * (Rs + Rp);
        return  temp  * hit_record.obj->material.MirrorReflectance * Fr;
    }
    //std::cout << "Reflected" << std::endl;

    if (type == MaterialType::DIELECTRIC) return temp;
    return  temp  * hit_record.obj->material.MirrorReflectance;
}


