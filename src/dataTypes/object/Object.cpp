//
// Created by vicy on 10/21/25.
//

#include "Object.h"
#include "Mesh.h"

#include <complex.h>

#include "../../functions/helpers.h"
#include "../../functions/overloads.h"

////////////////////////////////////////////////
/////////////////// OBJECT /////////////////////
////////////////////////////////////////////////

Object::~Object() = default;
Object::Object(Material& m, uint32_t id, Vertex vMax, Vertex vMin,  bool v)
        : material(m), _id(id),
          globalBbox(vMax, vMin), visible(v) {}


////////////////////////////////////////////////
///////////////// TRIANGLE /////////////////////
////////////////////////////////////////////////

ObjectType Triangle::getObjectType()  const{ return ObjectType::TRIANGLE; }

Object::intersectResult Triangle::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time) const
{
    intersectResult result;
    result.currTri = 0;
    result.t_min = t_min;
    result.obj = nullptr;
    if (!visible) return result;
    if (back_cull&& !shadow_test && dot_product(r.dir,n) >=0 ) return result;
    if (globalBbox.intersects(r))
    {
        Vec3r a_o = a.v - r.pos;
        real det_A = determinant(a_b,a_c,r.dir);
        if (det_A == 0)
        {
            //std::cout << "a_b: " << a_b << " a_c: " << a_c << " a: " << a.v << " b: " << b.v << " c: " << c.v  <<  std::endl;
            //std::cout << "rdir: " << r.dir  <<  std::endl;
            return result;
        }

        real beta = determinant(a_o,a_c,r.dir) / det_A;
        if (beta > 1)  return result;

        real gamma = determinant(a_b,a_o,r.dir) / det_A;
        if (gamma > 1) return result;

        if(beta >= 0 && gamma >= 0 && beta+gamma <= 1)
        { // there is an intersection
            real t_temp = determinant(a_b,a_c,a_o) / det_A;

            if (!shadow_test && t_temp < t_min && t_temp>0 )
            {
                result.t_min = t_temp;
                result.obj = this;
                return result;
            }

            if (shadow_test && t_temp < 1 && t_temp >= 0)
            {
                result.t_min = t_temp;
                result.obj = this;
                return result;
            }
        }
    }

    return result;
}



Vec3r Triangle::getNormal( const Vertex &v, uint32_t triID, real time)  const
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

Triangle::Triangle(const uint32_t id, CVertex &v1, CVertex &v2, CVertex &v3, Material &material, const ShadingType st, bool v, bool computeVNormals):
            Object(material, id,
                maxVert3(v1.v,v2.v,v3.v),
                minVert3(v1.v,v2.v,v3.v), v
                ),
                shadingType(st), a(v1), b(v2), c(v3), a_b(a.v-b.v), a_c(a.v-c.v)
{
    n = x_product((b.v-a.v), (c.v-a.v));
    if (computeVNormals)
    {
        a.n = n + a.n;
        b.n = n + b.n;
        c.n = n + c.n;
    }
    n = n.normalize();
    main_center.x = (a.v + b.v + c.v).x / 3.0;
    main_center.y = (a.v + b.v + c.v).y / 3.0;
    main_center.z = (a.v + b.v + c.v).z / 3.0;
}

////////////////////////////////////////////////
////////////////// SPHERE /////////////////////
////////////////////////////////////////////////
///
Sphere::Sphere(uint32_t id, CVertex& c, real r, Material &m,  bool v)
        : Object(m,id, Vertex(c.v.x+r, c.v.y+r,c.v.z+r),
            Vertex(c.v.x-r, c.v.y-r,c.v.z-r), v),  center(c), radius(r)
{
    radius2 = radius * radius;
    main_center = c.v;
}


ObjectType Sphere::getObjectType()  const{ return ObjectType::SPHERE; }


Object::intersectResult Sphere::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time)  const
{
    intersectResult result;
    result.currTri = 0;
    result.t_min = t_min;
    result.obj = nullptr;
    if (!visible) return result;
    if (globalBbox.intersects(r)){
        real t_temp;
        Vec3r o_c = r.pos - center.v;

        real A = dot_product(r.dir, r.dir);
        real B = dot_product(r.dir, o_c);
        real C = dot_product(o_c, o_c) - radius2;

        real BB_AC = B*B - A*C;

        if (BB_AC > 0)
        {
            real sqrt_BB_AC_A = sqrt(BB_AC)/A;
            real res_1 = (-B + sqrt(BB_AC) )/ A;
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
            result.t_min = t_temp;
            result.obj = this;
            return result;
        }
        else if (shadow_test && t_temp < 1 && t_temp >= 0)
        {
            result.t_min = t_temp;
            result.obj = this;
            return result;
        }
    }

    return result;
}

Vec3r Sphere::getNormal(const Vertex &v, uint32_t triID, real time) const
{
    return (v-center.v).normalize();
}



////////////////////////////////////////////////
/////////////////// PLANE /////////////////////
////////////////////////////////////////////////

Plane::Plane(uint32_t id, Vertex &v, std::string normal, Material &material,  bool vis) :
Object(material,id, Vertex(INFINITY,INFINITY,INFINITY), Vertex(-INFINITY,-INFINITY,-INFINITY),vis), point(v)
{
    std::istringstream ss(normal);
    ss >> n.i >> n.j >> n.k;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Normal string for plane: " + normal);
    }
    if (n.i ==0.0 && n.j ==0.0){ globalBbox.vMax.z = v.z + M4T_EPS; globalBbox.vMin.z = v.z - M4T_EPS; }
    else if (n.i ==0.0 && n.k ==0.0){ globalBbox.vMax.y = v.y + M4T_EPS; globalBbox.vMin.y = v.y - M4T_EPS; }
    else if (n.k ==0.0 && n.j ==0.0){ globalBbox.vMax.x = v.x + M4T_EPS; globalBbox.vMin.x = v.x - M4T_EPS; }
}

ObjectType Plane::getObjectType() const {return ObjectType::PLANE;}

Object::intersectResult Plane::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull, real time) const {
    real dot_r_n = dot_product(r.dir, n);
    intersectResult result;
    result.currTri = 0;
    result.t_min = t_min;
    result.obj = nullptr;
    if (dot_r_n == 0) return result;

    real t_temp = dot_product((point - r.pos),n) / dot_r_n;

    if (!shadow_test && t_temp < t_min && t_temp>0 )
    {
        result.t_min = t_temp;
        result.obj = this;
        return result;
    }

    if (shadow_test && t_temp < 1 && t_temp >= 0)
    {
        result.obj = this;
        return result;
    }

    return result;
}

Vec3r Plane::getNormal(const Vertex &v, uint32_t currTri, real time) const { return n;}


////////////////////////////////////////////////
///////////////// INSTANCE /////////////////////
////////////////////////////////////////////////


Instance::Instance(uint32_t id, Object* o, std::shared_ptr<Transformation> trans, Material &mat,Vec3r m,  bool orig, bool v) :
Object(mat,id,
    Vertex(),Vertex(),v), forwardTrans(std::move(trans))
{
    motion = m;
    if (m.i == 0.0 && m.j == 0.0 && m.k == 0.0) has_motion = false;
    else has_motion = true;
    if (orig) { // unique
        original = o;
    } else { // share
        original = o;
    }
    backwardTrans = forwardTrans->inv()->clone();
    computeGlobal();
    main_center =((*forwardTrans) * Vec4r(original->main_center)).getVertex();
    forwardTrans->getNormalTransform();
    backwardTrans->getNormalTransform();
}

Instance::~Instance()
{
}

ObjectType Instance::getObjectType() const { return ObjectType::INSTANCE; }

 Instance::intersectResult Instance::checkIntersection(const Ray& ray, const  real& t_min, bool shadow_test, bool back_cull, real time) const {

    // std::cout << "Check Intersection of Instance" << std::endl;
     intersectResult result;
     result.t_min = t_min;
     result.currTri = 0;
     result.obj = nullptr;
     Composite temp_f ,temp_b;
     Transformation* bwt = backwardTrans.get();
     Transformation* fwt = forwardTrans.get();
     if (has_motion && time>0)
     {
         Vec3r motion_At_time = motion*time;
         temp_f = Translate(motion_At_time) * (*fwt);
         temp_b = Translate(-motion_At_time) * (*bwt);
         fwt = &temp_f;
         bwt = &temp_b;
     }
     if (!shadow_test)
     {
         Ray localRay = (*bwt) * ray;
         result = original->checkIntersection(localRay, t_min, shadow_test,back_cull,time);
         result.obj = result.obj ? this :nullptr;
         return result;
     }
     else
     {
         Ray localRay = (*bwt) * ray; // get the local light point
         result = original->checkIntersection(localRay, t_min, shadow_test,back_cull,time);
         if (result.obj)
         {
             Vertex intersect = localRay.pos + localRay.dir * t_min;
             Vertex globalIntersect = ((*fwt) * Vec4r(intersect)).getVertex(); // o + t*d
             result.t_min = (globalIntersect.x - ray.pos.x) / ray.dir.i;
             result.obj = this;
             return result;
         }
         result.obj = nullptr;
         return result;
     }

}

Vec3r Instance::getNormal(const Vertex &v, uint32_t triID, real time) const {
    Vertex localV = getLocal(v,time);
    Vec3r res = original->getNormal(localV,triID, 0);
    return  getGlobalNormal(res,time);

}

Vec3r Instance::getGlobalNormal(const Vec3r& res, double time) const
{
    Composite temp_f;
    Transformation* fwt = forwardTrans.get();
    if (has_motion)
    {
        Vec3r motion_At_time = motion*time;
        temp_f = Translate(motion_At_time) * (*forwardTrans);
        fwt = &temp_f;
        fwt->getNormalTransform();
    }
    return ((fwt->normalTransform) * Vec4r(res)).getVec3r().normalize();
}


void Instance::computeGlobal()
{
    if (forwardTrans != nullptr && backwardTrans != nullptr)
    {
        globalBbox.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
        globalBbox.vMin = Vertex(INFINITY, INFINITY, INFINITY);
        Vertex v[2] = {original->globalBbox.vMax,original->globalBbox.vMin};
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    if (has_motion)
                    {
                        globalBbox.vMax = maxVert3(globalBbox.vMax, getGlobal(Vertex(v[i].x,v[j].y,v[k].z),1.0),getGlobal(Vertex(v[i].x,v[j].y,v[k].z),0.0));
                        globalBbox.vMin = minVert3(globalBbox.vMin, getGlobal(Vertex(v[i].x,v[j].y,v[k].z),1.0),getGlobal(Vertex(v[i].x,v[j].y,v[k].z),0.0));

                    }
                    else
                    {
                        globalBbox.vMax = maxVert2(globalBbox.vMax, getGlobal(Vertex(v[i].x,v[j].y,v[k].z),0.0));
                        globalBbox.vMin = minVert2(globalBbox.vMin, getGlobal(Vertex(v[i].x,v[j].y,v[k].z),0.0));
                    }
                }
            }
        }
    }
}

Ray Instance::getLocal(Ray &r)
{
    Ray result;
    result.pos = ((*backwardTrans) * Vec4r(r.pos)).getVertex();
    result.dir = ((*backwardTrans) * Vec4r(r.dir)).getVec3r();
    return result;
}

Vertex Instance::getLocal(const Vertex &v, real time) const
{
    Composite temp_b;
    Transformation* bwt = backwardTrans.get();
    if (has_motion && time > 0)
    {
        Vec3r motion_At_time = motion*time;
        temp_b = Translate(-motion_At_time) * (*backwardTrans);
        bwt = &temp_b;
    }
    Vertex result;
    result = ((*bwt) * Vec4r(v)).getVertex();
    return result;
}

Vec3r Instance::getLocal(Vec3r &v, real time)
{
    Composite temp_b;
    Transformation* bwt = backwardTrans.get();
    if (has_motion && time > 0)
    {
        Vec3r motion_At_time = motion*time;
        temp_b = Translate(-motion_At_time) * (*backwardTrans);
        bwt = &temp_b;
    }
    Vec3r result;
    result = ((*bwt) * Vec4r(v)).getVec3r();
    return result;
}


Vec3r Instance::getGlobal(Vec3r &v, double time)
{
    Composite temp_f;
    Transformation* fwt = forwardTrans.get();
    if (has_motion && time > 0)
    {
        Vec3r motion_At_time = motion*time;
        temp_f = Translate(motion_At_time) * (*forwardTrans);
        fwt = &temp_f;
    }
    Vec3r result;
    result = ((*fwt) * Vec4r(v)).getVec3r();
    return result;
}

Vertex Instance::getGlobal(Vertex v, real time) const
{
    Composite temp_f;
    Transformation* fwt = forwardTrans.get();
    if (has_motion && time > 0)
    {
        Vec3r motion_At_time = motion*time;
        temp_f = Translate(motion_At_time) * (*forwardTrans);
        fwt = &temp_f;
    }
    Vertex result;
    result = ((*fwt) * Vec4r(v)).getVertex();
    return result;
}


