//
// Created by vicy on 10/21/25.
//

#include "Object.h"

#include "helpers.h"

Object::~Object() = default;

ObjectType Triangle::getObjectType() { return ObjectType::TRIANGLE; }

Object *Triangle::checkIntersection(const Ray& r, real &t_min, bool shadow_test)
{
    Vec3r a_o = a.v - r.pos;
    real det_A = determinant(a_b,a_c,r.dir);
    if (det_A == 0)
    {
        //std::cout << "a_b: " << a_b << " a_c: " << a_c << " a: " << a.v << " b: " << b.v << " c: " << c.v  <<  std::endl;
        //std::cout << "rdir: " << r.dir  <<  std::endl;
        return nullptr;
    }

    real beta = determinant(a_o,a_c,r.dir) / det_A;
    if (beta > 1)  return nullptr;

    real gamma = determinant(a_b,a_o,r.dir) / det_A;
    if (gamma > 1) return nullptr;

    if(beta >= 0 && gamma >= 0 && beta+gamma <= 1)
    { // there is an intersection
        real t_temp = determinant(a_b,a_c,a_o) / det_A;

        if (!shadow_test && t_temp < t_min && t_temp>0 )
        {
            t_min = t_temp;
            return this;
        }

        if (shadow_test && t_temp < 1 && t_temp >= 0)
        {
            return this;
        }

    }

    return nullptr;
}

Vec3r Triangle::getNormal(Vertex &v)
{
    if (shadingType == ShadingType::SMOOTH)
    {
        Vec3r b_a = b.v - a.v;
        Vec3r c_a = c.v - a.v;
        Vec3r v_a = v - a.v;

        real d00 = dot_product(b_a, b_a);
        real d01 = dot_product(b_a, c_a);
        real d11 = dot_product(c_a, c_a);
        real d20 = dot_product(v_a, b_a);
        real d21 = dot_product(v_a, c_a);

        real denom = d00 * d11 - d01 * d01;
        real A_b = (d11 * d20 - d01 * d21) / denom;
        real A_c = (d00 * d21 - d01 * d20) / denom;
        real A_a = 1.0 - A_b - A_c;

        Vec3r interpolated_normal = a.n * A_a + b.n * A_b + c.n * A_c;
        interpolated_normal = interpolated_normal.normalize();
        //if (dot_product(interpolated_normal, n) < 0.0) interpolated_normal = -interpolated_normal;
        return interpolated_normal;
    }
    else return n;
}



ObjectType Sphere::getObjectType() { return ObjectType::SPHERE; }


Object *Sphere::checkIntersection(const Ray& r, real &t_min, bool shadow_test)
{
    real t_temp;
    Vec3r o_c = r.pos - center.v;

    real A = dot_product(r.dir, r.dir);
    real B = dot_product(r.dir, o_c);
    real C = dot_product(o_c, o_c) - radius2;

    real BB_AC = B*B - A*C;

    if (BB_AC > 0)
    {
        real sqrt_BB_AC_A = sqrt(BB_AC)/A;
        real res_1 = (-B + sqrt(BB_AC) )/ A; // TODO: burası floating point farklılığına yol açabilir
        real res_2 = (-B - sqrt(BB_AC) )/ A;

        if (res_1 <= res_2 && res_1 > 0 && res_2 > 0)  t_temp = res_1;
        else if (res_1 > 0 && res_2 > 0)  t_temp = res_2;
        else if (res_1 > 0)  t_temp = res_1;
        else if (res_2 > 0)  t_temp = res_2;
        else  t_temp = INFINITY;
    }
    else if (BB_AC == 0) t_temp =  -B/A;
    else /*BB_AC < 0*/   t_temp = INFINITY;


    if (!shadow_test && t_temp < t_min && t_temp>0 )
    {
        t_min = t_temp;
        return this;
    }
    else if (shadow_test && t_temp < 1 && t_temp >= 0)
    {
        return this;
    }

    return nullptr;
}

Vec3r Sphere::getNormal(Vertex &v)
{
    return (v-center.v).normalize();
}


ObjectType Mesh::getObjectType(){ return ObjectType::MESH; }

Object *Mesh::checkIntersection(const Ray& ray, real &t_min, bool shadow_test)
{
    int numFaces = Faces.size();
    Triangle *temp_obj = nullptr;
    Triangle *return_obj = nullptr;
    for (int i=0; i< numFaces; i++)
    {

        temp_obj = dynamic_cast<Triangle*>(Faces[i].checkIntersection(ray, t_min, shadow_test));
        if (temp_obj != nullptr)
        {
            if (shadow_test) return temp_obj;
            return_obj = temp_obj;
        }
    }

    return return_obj;
}



Object *Plane::checkIntersection(const Ray& r, real &t_min, bool shadow_test){
    real dot_r_n = dot_product(r.dir, n);
    if (dot_r_n == 0) return nullptr;

    real t_temp = dot_product((point - r.pos),n) / dot_r_n;

    if (!shadow_test && t_temp < t_min && t_temp>0 )
    {
        t_min = t_temp;
        return this;
    }

    if (shadow_test && t_temp < 1 && t_temp >= 0)
    {
        return this;
    }

    return nullptr;
}




