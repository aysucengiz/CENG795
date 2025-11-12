//
// Created by vicy on 10/21/25.
//

#include "Object.h"

#include <complex.h>

#include "../functions/helpers.h"
#include "../functions/overloads.h"

////////////////////////////////////////////////
/////////////////// OBJECT /////////////////////
////////////////////////////////////////////////

Object::~Object() = default;
Object::Object(Material &m, uint32_t id,
        Vertex vMax, Vertex vMin, bool v)
        : material(m), _id(id),
          globalBbox(vMax, vMin), visible(v){}


////////////////////////////////////////////////
///////////////// TRIANGLE /////////////////////
////////////////////////////////////////////////

ObjectType Triangle::getObjectType() { return ObjectType::TRIANGLE; }

Object *Triangle::checkIntersection(Ray& r, real& t_min, bool shadow_test)
{
    if (!visible) return nullptr;
    if (!shadow_test && dot_product(r.dir,n) >=0 ) return nullptr;
    if (globalBbox.intersects(r))
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

Triangle::Triangle(const uint32_t id, CVertex &v1, CVertex &v2, CVertex &v3, Material &material, const ShadingType st, bool v):
            Object(material, id,
                maxVert3(v1.v,v2.v,v3.v),
                minVert3(v1.v,v2.v,v3.v), v
                ),
                shadingType(st), a(v1), b(v2), c(v3), a_b(a.v-b.v), a_c(a.v-c.v)
{
    n = x_product((b.v-a.v), (c.v-a.v));
    a.n = n + a.n;
    b.n = n + b.n;
    c.n = n + c.n;
    n = n.normalize();
    main_center.x = (a.v + b.v + c.v).x / 3.0;
    main_center.y = (a.v + b.v + c.v).y / 3.0;
    main_center.z = (a.v + b.v + c.v).z / 3.0;
}

////////////////////////////////////////////////
////////////////// SPHERE /////////////////////
////////////////////////////////////////////////
///
Sphere::Sphere(uint32_t id, CVertex& c, real r, Material &m, bool v)
        : Object(m,id, Vertex(c.v.x+r, c.v.y+r,c.v.z+r),
            Vertex(c.v.x-r, c.v.y-r,c.v.z-r), v),  center(c), radius(r)
{
    radius2 = radius * radius;
    main_center = c.v;
}


ObjectType Sphere::getObjectType() { return ObjectType::SPHERE; }


Object *Sphere::checkIntersection(Ray& r, real& t_min, bool shadow_test)
{

    if (!visible) return nullptr;
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
    }

    return nullptr;
}

Vec3r Sphere::getNormal(Vertex &v)
{
    return (v-center.v).normalize();
}


////////////////////////////////////////////////
//////////////////// MESH //////////////////////
////////////////////////////////////////////////

Mesh::Mesh(uint32_t id, std::string st, Material &m, std::string s, bool read_from_file, std::deque<CVertex> &vertices, bool v, uint32_t start_index)
    : Object(m,id,Vertex(-INFINITY,-INFINITY,-INFINITY),Vertex(INFINITY,INFINITY,INFINITY),v ){
        if (st == "smooth")    shadingtype = ShadingType::SMOOTH;
        else if (st == "flat") shadingtype = ShadingType::FLAT;
        else                   shadingtype = ShadingType::NONE;

        if (read_from_file)
        {
            happly::PLYData plyIn(s);
            std::vector<std::vector<int>> f = plyIn.getElement("face").getListProperty<int>("vertex_indices");
            //std::cout << "First face indices: " << f[0][0] << " " << f[0][1] << " " << f[0][2] << std::endl;
            for (int i = 0; i < f.size(); i++)
            {
                if (vertices.size() <= start_index+f[i][0])std::cout << vertices.size() <<" : " << start_index-1+f[i][0]<< std::endl;
                if (vertices.size() <= start_index+f[i][1])std::cout << vertices.size() <<" : " << start_index-1+f[i][1]<< std::endl;
                if (vertices.size() <= start_index+f[i][2])std::cout << vertices.size() <<" : " << start_index-1+f[i][2]<< std::endl;

                if (f[i].size() == 3 && f[i][0] != f[i][1] && f[i][0] != f[i][2] && f[i][1] != f[i][2] )
                {
                    Vertex minv = minVert3(vertices[start_index+f[i][0]].v,vertices[start_index+f[i][1]].v,vertices[start_index+f[i][2]].v);
                    Vertex maxv = maxVert3(vertices[start_index+f[i][0]].v,vertices[start_index+f[i][1]].v,vertices[start_index+f[i][2]].v);
                    globalBbox.vMax = maxVert3(maxv,globalBbox.vMax,globalBbox.vMax);
                    globalBbox.vMin = minVert3(minv,globalBbox.vMin,globalBbox.vMin);
                    Faces.push_back(Triangle(Faces.size(),
                        vertices[start_index+f[i][0]], vertices[start_index+f[i][1]], vertices[start_index+f[i][2]],
                      m, shadingtype));
                }
            }

        }
        else
        {
            std::istringstream verticesStream(s);
            uint32_t vert[3];

            while (verticesStream >>vert[0]>>vert[1]>> vert[2]) {
                vert[0]--; vert[1]--; vert[2]--;
                if (vert[0] != vert[1] && vert[0] != vert[2] && vert[1] !=vert[2])
                {
                    Vertex minv = minVert3(vertices[vert[0]].v,vertices[vert[1]].v,vertices[vert[2]].v);
                    Vertex maxv = maxVert3(vertices[vert[0]].v,vertices[vert[1]].v,vertices[vert[2]].v);
                    globalBbox.vMax = maxVert3(maxv,globalBbox.vMax,globalBbox.vMax);
                    globalBbox.vMin = minVert3(minv,globalBbox.vMin,globalBbox.vMin);

                    Faces.push_back(Triangle(Faces.size(),
                        vertices[vert[0]], vertices[vert[1]], vertices[vert[2]],
                        m, shadingtype));
                }

            }
        }

    main_center.x = (globalBbox.vMax.x + globalBbox.vMin.x) / 2.0;
    main_center.y = (globalBbox.vMax.y + globalBbox.vMin.y) / 2.0;
    main_center.z = (globalBbox.vMax.z + globalBbox.vMin.z) / 2.0;


    }

ObjectType Mesh::getObjectType(){ return ObjectType::MESH; }


Vec3r Mesh::getNormal(Vertex &v)
{
    return Faces[currTri].getNormal(v);
}

Object *Mesh::checkIntersection(Ray& ray, real& t_min, bool shadow_test)
{

    if (!visible) return nullptr;
    if (globalBbox.intersects(ray))
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
                currTri = i;
            }
        }

        return return_obj;
    }
    return nullptr;

}


////////////////////////////////////////////////
/////////////////// PLANE /////////////////////
////////////////////////////////////////////////

Plane::Plane(uint32_t id, Vertex &v, std::string normal, Material &material, bool vis) :
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

ObjectType Plane::getObjectType() {return ObjectType::PLANE;}

Object *Plane::checkIntersection(Ray& r, real& t_min, bool shadow_test){
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

Vec3r Plane::getNormal(Vertex &v) { return n;}



////////////////////////////////////////////////
///////////////// INSTANCE /////////////////////
////////////////////////////////////////////////


Instance::Instance(uint32_t id, Object *original, Transformation *trans, Material &mat, bool orig, bool v) : original(original),
Object(mat,id,
    Vertex(),Vertex(),v)
{
    switch (trans->getTransformationType())
    {
    case TransformationType::ROTATE:
        forwardTrans = new Rotate(*dynamic_cast<Rotate*>(trans));
        break;
    case TransformationType::SCALE:
        forwardTrans = new Scale(*dynamic_cast<Scale*>(trans));
        break;
    case TransformationType::TRANSLATE:
        forwardTrans = new Translate(*dynamic_cast<Translate*>(trans));
        break;
    case TransformationType::COMPOSITE:
    default:
        forwardTrans = new Composite(*dynamic_cast<Composite*>(trans));
        break;
    }
    backwardTrans = forwardTrans->inv();
    computeGlobal();
    main_center =((*forwardTrans) * Vec4r(original->main_center)).getVertex();
}

Instance::~Instance()
{
    delete forwardTrans;
    delete backwardTrans;
    if (orig && original != nullptr) delete original;
}

ObjectType Instance::getObjectType() { return ObjectType::INSTANCE; }

Object* Instance::checkIntersection(Ray& ray, real& t_min, bool shadow_test){

    //std::cout << "Check Intersection of Instance" << std::endl;
    if (!shadow_test)
    {
        Ray localRay = (*backwardTrans) * ray;
        Object *temp = original->checkIntersection(localRay, t_min, shadow_test);
        return temp ? this : nullptr;
    }
    else
    {
        Ray localRay = (*backwardTrans) * ray; // get the local light point
        Object *temp = original->checkIntersection(localRay, t_min, shadow_test);  // TODO: t_min manipüle olunca intersection point kayıyor olabilir mi
        if (temp)
        {
            Vertex intersect = localRay.pos + localRay.dir * t_min;
            Vertex globalIntersect = getGlobal(intersect); // o + t*d
            t_min = (globalIntersect.x - ray.pos.x) / ray.dir.i;
            return this;
        }
        return nullptr;
    }
}

Vec3r Instance::getNormal(Vertex &v){
    Vertex localV = ((*backwardTrans) * Vec4r(v)).getVertex();
    Vec3r res = original->getNormal(localV);
    return  ((forwardTrans->normalTransform) * Vec4r(res)).getVec3r().normalize();
}


void Instance::addTransformation(Transformation *trans)
{
    if (trans->getTransformationType() == TransformationType::ROTATE)
    {
        Rotate tempR(Ray(main_center,dynamic_cast<Rotate*>(trans)->axis.dir),dynamic_cast<Rotate*>(trans)->angle);
        forwardTrans->arr = (tempR * (*forwardTrans)).arr;
    }
    else if  (trans->getTransformationType() == TransformationType::SCALE)
    {
        Scale tempS(main_center,dynamic_cast<Scale*>(trans)->x,dynamic_cast<Scale*>(trans)->y,dynamic_cast<Scale*>(trans)->z );
        forwardTrans->arr = (tempS * (*forwardTrans)).arr;
    }
    else forwardTrans->arr = ((*trans) * (*forwardTrans)).arr;
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
                    globalBbox.vMax = maxVert2(globalBbox.vMax, getGlobal(Vertex(v[i].x,v[j].y,v[k].z)));
                    globalBbox.vMin = minVert2(globalBbox.vMin, getGlobal(Vertex(v[i].x,v[j].y,v[k].z)));
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

Vertex Instance::getLocal(Vertex &v)
{
    Vertex result;
    result = ((*backwardTrans) * Vec4r(v)).getVertex();
    return result;
}

Vec3r Instance::getLocal(Vec3r &v)
{
    Vec3r result;
    result = ((*backwardTrans) * Vec4r(v)).getVec3r();
    return result;
}


Vec3r Instance::getGlobal(Vec3r &v)
{
    Vec3r result;
    result = ((*forwardTrans) * Vec4r(v)).getVec3r();
    return result;
}

Vertex Instance::getGlobal(Vertex v)
{
    Vertex result;
    result = ((*forwardTrans) * Vec4r(v)).getVertex();
    return result;
}




