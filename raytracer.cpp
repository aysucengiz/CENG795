//
// Created by vicy on 10/14/25.
//

#include "raytracer.h"

// TODO: Shadow ray hesaplamasındayız

void Raytracer::writeColorToImage()
{
    image[curr_pixel++] = clamp(final_color.r, 0, 255);
    image[curr_pixel++] = clamp(final_color.g, 0, 255);
    image[curr_pixel++] = clamp(final_color.b, 0, 255);
}


Vec3r Raytracer::sphereNormal(Sphere &s, Vertex &v)
{
    Vertex &c = scene.Vertices[s.center];
    return (v-c).normalize();
}

void Raytracer::parseScene(std::string input_path){
    Parser::parseScene(input_path, scene);
    num_cameras = scene.Cameras.size();
    numTriangle = scene.Triangles.size();
    numSphere = scene.Spheres.size();
    numMesh = scene.Meshes.size();
    numLights = scene.PointLights.size();

}

void Raytracer::drawAllScenes(){
    for(int i = 0; i < num_cameras; i++)
    {
        this->drawScene(i);
    }
}


void Raytracer::drawScene(uint32_t camID){
    Camera cam = scene.Cameras[camID];
    u = x_product(cam.Up, -cam.Gaze);
    q = cam.Position + (cam.Gaze * cam.nearDistance) + u*cam.l + cam.Up*cam.t; // TODO: near plane verileri doğru mu?
    s_u_0 = (cam.r - cam.l) / cam.width;
    s_v_0 = (cam.t - cam.b) / cam.height;
    s_v = s_v_0 * 0.5;
    viewing_ray.pos = cam.Position;

    uint32_t width = cam.width;
    uint32_t height = cam.height;

    curr_pixel = 0;
    image = new unsigned char[width * height * 3];
    for (uint32_t y = 0; y < height; y++){
        for (uint32_t x = 0; x < width; x++){
            computeViewingRay(cam, x, y);
            final_color = computeColor(0);
            writeColorToImage();
        }
    }

    PPM::write_ppm(("outputs/" + cam.ImageName).c_str(), image, width, height);
    delete[] image;
}


void Raytracer::computeViewingRay(Camera &cam, uint32_t x, uint32_t y)
{
    if (x == 0)
    {
        s_u = s_u_0 * 0.5;
        s_v += s_v_0;
    }
    else s_u += s_u_0;

    Vertex s = q + u*s_u - cam.Up * s_v;
    viewing_ray.dir = s - cam.Position;
    viewing_ray.dir = viewing_ray.dir.normalize();
}

bool Raytracer::isUnderShadow()
{
    real t_obj;

    for (int j = 0; j < numMesh; j++)
    {
        t_obj = checkMeshIntersection(shadow_ray, j);
        if(t_obj < 1 && t_obj >= 0) return  true;
    }

    for (int j = 0; j < numTriangle; j++)
    {
        t_obj = checkTriangleIntersection(shadow_ray, j);
        if(t_obj < 1 && t_obj >= 0) return  true;
    }

    for (int j = 0; j < numSphere; j++)
    {
        t_obj = checkSphereIntersection(shadow_ray, j);
        if(t_obj < 1 && t_obj >= 0) return  true;
    }

    return false;
}


Color Raytracer::computeColor(int depth)
{
    Color curr_color;
    hit_record.type = ObjectType::NONE;

    if (depth > scene.MaxRecursionDepth) return curr_color;

    checkObjIntersection();

    if (hit_record.type != ObjectType::NONE &&  scene.Materials[hit_record.matID].materialType != MaterialType::NONE)
    {

        // std::cout << " A hit!" << std::endl;
        computeHitRecord();

        Material &m = scene.Materials[hit_record.matID];
        curr_color = scene.AmbientLight * m.AmbientReflectance;
        if (m.materialType == MaterialType::MIRROR) curr_color += reflect(depth, m.MirrorReflectance);

        for (int i=0; i < numLights; i++)
        {

            compute_shadow_ray(i);
            //curr_color += Color(255, 0, 0);
            std::cout << "I can see you" << std::endl;
            if (!isUnderShadow())
            {

                std::cout << "You are under the light" << std::endl;
                real cos_theta = dot_product(shadow_ray.dir.normalize(), hit_record.normal);
                Color I_R_2 = scene.PointLights[i].Intensity / dot_product(shadow_ray.dir, shadow_ray.dir);
                if ( cos_theta > 0)
                {

                    std::cout << "You are looking at me" << std::endl;
                    curr_color += diffuseTerm(cos_theta,I_R_2) + specularTerm(cos_theta,I_R_2);
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

Color Raytracer::diffuseTerm(real cos_theta, Color I_R_2){
    return scene.Materials[hit_record.matID].DiffuseReflectance * I_R_2 * cos_theta;
}

Color Raytracer::specularTerm(real cos_theta, Color I_R_2)
{
    Material &m = scene.Materials[hit_record.matID];
    Vec3r h = (shadow_ray.dir.normalize() - viewing_ray.dir).normalize();
    real cos_alpha = dot_product(hit_record.normal, h);
    if (cos_alpha < 0) return Color();
    return m.SpecularReflectance * I_R_2 * pow(cos_alpha, m.PhongExponent);
}

void Raytracer::compute_shadow_ray(uint32_t i)
{
    shadow_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    shadow_ray.dir = scene.PointLights[i].Position - shadow_ray.pos;
}


void Raytracer::checkObjIntersection()
{
    real t_temp;
    t_min = INFINITY; // TODO: bu float doubleda istikrarlı nasıl olur

    for(int i = 0; i < numTriangle; i++)
    {
        t_temp = checkTriangleIntersection(viewing_ray,i);
        if( t_temp < t_min && t_temp>0 )
        {
            t_min = t_temp;
            hit_record.type = ObjectType::TRIANGLE;
            hit_record.objID = i;
            hit_record.matID = scene.Triangles[i].material;
        }
    }

    for(int i = 0; i < numSphere; i++)
    {
        t_temp = checkSphereIntersection(viewing_ray,i);
        if( t_temp < t_min && t_temp>0 )
        {
            t_min = t_temp;
            hit_record.type = ObjectType::SPHERE;
            hit_record.objID = i;
            hit_record.matID = scene.Spheres[i].material;
        }
    }

    for(int i = 0; i < numMesh; i++)
    {

        if (checkMeshIntersection(viewing_ray,i))
        {
            hit_record.type = ObjectType::MESH;
            hit_record.meshID = i;
            hit_record.matID = scene.Meshes[i].material;
        }
    }
}


Color Raytracer::reflect(int depth, Color &reflectance)
{
    std::cout << "Reflecting" << std::endl;
    Ray reflected_ray;
    reflected_ray.pos = hit_record.intersection_point + hit_record.normal * scene.ShadowRayEpsilon;
    reflected_ray.dir = viewing_ray.dir + hit_record.normal*2*dot_product(hit_record.normal,-viewing_ray.dir);
    Color comp = computeColor(depth+1);
    return comp * reflectance;
}

void Raytracer::computeHitRecord()
{
    hit_record.intersection_point = viewing_ray.pos + viewing_ray.dir * t_min;
    if (hit_record.type == ObjectType::TRIANGLE) hit_record.normal = scene.Triangles[hit_record.objID].n;
    else if (hit_record.type == ObjectType::MESH) hit_record.normal = scene.Meshes[hit_record.meshID].Faces[hit_record.objID].n;
    else if (hit_record.type == ObjectType::SPHERE) hit_record.normal = sphereNormal(scene.Spheres[hit_record.objID], hit_record.intersection_point);
}

real Raytracer::checkSphereIntersection(Ray &r, uint32_t i)
{
    real t_temp;
    Sphere &s = scene.Spheres[i];
    Vertex &center = scene.Vertices[s.center];
    Vec3r o_c = r.pos - center;

    real A = dot_product(r.dir, r.dir);
    real B = dot_product(r.dir, o_c);
    real C = dot_product(o_c, o_c) - s.radius*s.radius;

    real BB_AC = B*B - A*C;
    real B_A = -B/A;

    if (BB_AC > 0)
    {
        real sqrt_BB_AC_A = sqrt(BB_AC)/A;
        real res_1 = B_A + sqrt_BB_AC_A; // TODO: burası floating point farklılığına yol açabilir
        real res_2 = B_A - sqrt_BB_AC_A;

        if (res_1 <= res_2 && res_1 > 0)  t_temp = res_1;
        else  t_temp = res_2;
    }
    else if (BB_AC == 0) t_temp = B_A;
    else /*BB_AC < 0*/   t_temp = INFINITY;

    return t_temp;
}

bool Raytracer::checkMeshIntersection(Ray &r, uint32_t i)
{
    bool result = false;
    Mesh &mesh = scene.Meshes[i];
    uint32_t numTriangleMesh = mesh.Faces.size();
    for (int j = 0; j < numTriangleMesh; j++)
    {
        real t_temp = checkTriangleIntersection(r,j,i);

        if( t_temp < t_min && t_temp>0 ){
            t_min = t_temp;
            hit_record.objID = j;
            result = true;
        }

    }
    return result;
}




real Raytracer::checkTriangleIntersection(Ray &r, uint32_t j, int32_t i)
{
    real t_temp;

    Triangle& tri = i<0 ? scene.Triangles[j] : scene.Meshes[i].Faces[j];

    Vec3r a_o = scene.Vertices[tri.indices[0]] - r.pos;
    real det_A = determinant(tri.a_b,tri.a_c,r.dir);

    real beta = determinant(a_o,tri.a_c,r.dir) / det_A;
    real gamma = determinant(tri.a_b,a_o,r.dir) / det_A;
    real t = determinant(tri.a_b,tri.a_c,a_o) / det_A;

    if(beta >= 0 && gamma >= 0 && beta+gamma <= 1){ // there is an intersection
        t_temp = t;
    }else{
        t_temp = INFINITY;
    }

    return t_temp;


}

