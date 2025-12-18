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

Object::Object(Material& m, uint32_t id, Vertex vMax, Vertex vMin, std::vector<Texture*> ts, bool v)
    : material(m), _id(id),
      globalBbox(vMax, vMin), visible(v)
{
    NormalTexture = nullptr;
    SpecularTexture = nullptr;
    DiffuseTexture = nullptr;
    AllTexture = nullptr;
    for (int i = 0; i < ts.size(); i++)
    {
        switch (ts[i]->decalMode)
        {
        case DecalMode::BUMP_NORMAL:
        case DecalMode::REPLACE_NORMAL:
            NormalTexture = ts[i];
            break;
        case DecalMode::BLEND_KD:
        case DecalMode::REPLACE_KD:
            DiffuseTexture = ts[i];
            break;
        case DecalMode::REPLACE_KS:
            SpecularTexture = ts[i];
            break;
        case DecalMode::REPLACE_ALL:
            AllTexture = ts[i];
            break;
        default:
            break;
        }
    }
}

Color Object::getTextureColorAt(Vertex &pos, real time, int triID) const
{
    if (AllTexture != nullptr)
    {
        Texel tex = getTexel(pos,time, triID);
        return AllTexture->TextureColor(pos, tex)*255.0;
    }
    return Color(0, 0, 0);
}


Color Object::GetColourAt( Color I_R_2, real cos_theta, const Vec3r& normal, const Ray& ray, Ray& shadow_ray, real time, int triID) const
{
    Texel tex = getTexel(shadow_ray.pos,time, triID);
    Color diffuse = diffuseTerm(I_R_2, cos_theta, shadow_ray.pos, tex, time);
    Color specular = specularTerm(normal, ray, I_R_2, shadow_ray, shadow_ray.pos, tex, time);
    return  diffuse + specular ;
}

Color Object::diffuseTerm(Color I_R_2, real cos_theta, Vertex &vert, Texel &t, real time) const
{
    Color kd = material.DiffuseReflectance;
    if (DiffuseTexture != nullptr)
    {
        // if (DiffuseTexture->getTextureType() == TextureType::CHECKERBOARD)
        //     std::cout << "Checkerboard" << std::endl;
        Color tex_col = DiffuseTexture->TextureColor(vert, t);
        switch (DiffuseTexture->decalMode)
        {
            case DecalMode::REPLACE_KD:
                kd = tex_col;
                break;
            case DecalMode::BLEND_KD:
                kd = (kd + tex_col)/2.0;
            break;
        }
    }
    return kd * cos_theta * I_R_2;
}

Color Object::specularTerm(const Vec3r& normal, const Ray& ray, Color I_R_2,
                           Ray& shadow_ray, Vertex &vert,  Texel &t, real time) const
{
    Color ks = material.SpecularReflectance;
    if (SpecularTexture != nullptr) ks = SpecularTexture->TextureColor(vert, t);
    if (ks.isBlack()) return ks;
    Vec3r h = (shadow_ray.dir.normalize() - ray.dir.normalize()).normalize();
    real cos_alpha = dot_product(normal, h);
    if (cos_alpha < 0) return Color(0.0,0.0,0.0);

    return ks * I_R_2 * pow(cos_alpha, material.PhongExponent);
}


real chenge_interval(real value)
{
    return value * 2 - 1.0;
}

real Object::h(Texel t) const
{
    Vertex v(0.0,0.0,0.0);
    Color c = NormalTexture->TextureColor(v, t);
    return (c.r + c.g + c.b)/3.0;
}

real Object::h(Vertex v) const
{
    Texel t(0.0,0.0);
    Color c = NormalTexture->TextureColor(v, t);
    real result = (c.r + c.g + c.b)/3.0;
    return result;
}


Vec3r Object::getTexturedNormal(const Vertex & v, const Vec3r& n, real time, int triID) const
{

    Vec3r NewN = n;
    if ( NormalTexture != nullptr)
    {
        Texel t = getTexel(v,time, triID);
        Color textureColor =  NormalTexture->TextureColor(v, t);
        Vec3r locNormal(textureColor.r * 2- 1.0 , textureColor.g * 2 - 1.0 , textureColor.b *2 - 1.0 );
        locNormal = locNormal.normalize();
        Vec3r onb[3];
        getBitan(v, onb[0], onb[1], triID);
        onb[2] = x_product(onb[1], onb[0]);

        Vec3r p_u = onb[0];
        Vec3r p_v = onb[1];
        Vec3r norm = x_product(p_v,p_u);
        // std::cout << onb[0] << " " << onb[1] << " " << onb[2] << std::endl;
        if (NormalTexture->decalMode == DecalMode::REPLACE_NORMAL)
        {
            NewN = Vec3r(0.0, 0.0, 0.0);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    NewN[(Axes) i] += locNormal[(Axes) j] * onb[j][(Axes) i];
        }
        else if (NormalTexture->decalMode == DecalMode::BUMP_NORMAL)
        {
            Texel d(0.0,0.0);
            if (NormalTexture->getTextureType() == TextureType::IMAGE)
            {
                d.u = 1.0 / dynamic_cast<ImageTexture*>(NormalTexture)->image->width;
                d.v = 1.0 / dynamic_cast<ImageTexture*>(NormalTexture)->image->height;

                Texel t_u(t.u + d.u, t.v);
                Texel t_u_(t.u - d.u, t.v);
                Texel t_v(t.u, t.v + d.v);
                Texel t_v_(t.u, t.v - d.v);
                real h_t = h(t);
                Texel h_t_p(h(t_u),h(t_v));
                Texel h_t_n(h(t_u_),h(t_v_));

                Texel dh = {
                    (h_t_p.u - h_t_n.u) / (2*d.u),
                    (h_t_p.v - h_t_n.v) / (2*d.v)
                };
                Vec3r q_u = p_u +n * dh.u* NormalTexture->bumpFactor;
                Vec3r q_v = p_v+ n * dh.v* NormalTexture->bumpFactor;
                NewN =  x_product(q_v,q_u);
            }
            else
            {
                real epsilon = 0.0001;
                Vertex vp[3], vn[3];
                Vec3r dh;
                for (int i = 0; i < 3; i++) vp[i] = v + onb[i]*epsilon;
                for (int i = 0; i < 3; i++) vn[i] = v - onb[i]*epsilon;
                for (int i = 0; i < 3; i++) dh[(Axes) i] = (h(vp[i]) - h(vn[i]))/(2*epsilon);

                Vec3r g_parall  = n *dot_product(dh,n);
                Vec3r g_perp    = dh - g_parall;
                NewN = n - g_perp* NormalTexture->bumpFactor;
            }




        }
        NewN = NewN.normalize();
    }

    return NewN;
}

void Object::ComputeBitan(CVertex &b, CVertex &a, CVertex &c, Vec3r& pT, Vec3r& pB, Vec3r& n)
{
    real cv_av = c.t.v - a.t.v;
    real cu_au = c.t.u - a.t.u;
    real bv_av = b.t.v - a.t.v;
    real bu_au = b.t.u - a.t.u;
    real det =bu_au * cv_av - cu_au * bv_av;

    real M1[2][2] = {
        {cv_av / det, -bv_av / det},
        {-cu_au /det, bu_au / det}
    };

    Vec3r A[2] = {b.v - a.v, c.v - a.v};
    Vec3r Bits[2] = {};

    for ( int k = 0; k < 2; k++)
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                Bits[k][(Axes) j] += M1[k][i] * A[i][(Axes) j];


    pT = Bits[0].normalize();
    pB = Bits[1].normalize();
    // std::cout << "a: v: " << a.v << " | t: " << a.t.u << " " <<  a.t.v  << std::endl;
    // std::cout << "b: v: " << b.v << " | t: " << b.t.u << " " <<  b.t.v  << std::endl;
    // std::cout << "c: v: " << c.v << " | t: " << c.t.u << " " <<  c.t.v  << std::endl;
    // std::cout << "B: " << pB << std::endl;
    // std::cout << "T: " << pT << std::endl;
    // pT = Bits[0].normalize();
    // pB = Bits[1].normalize();
}


////////////////////////////////////////////////
///////////////// TRIANGLE /////////////////////
////////////////////////////////////////////////

ObjectType Triangle::getObjectType() const { return ObjectType::TRIANGLE; }



void Triangle::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const
{
    pT = T;
    pB = B;
}


Object::intersectResult Triangle::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull,
                                                    real time) const
{
    intersectResult result;
    result.currTri = 0;
    result.t_min = t_min;
    result.obj = nullptr;
    if (!visible) return result;
    if (back_cull && !shadow_test && dot_product(r.dir, n) >= 0) return result;
    if (globalBbox.intersects(r))
    {
        Vec3r a_o = a.v - r.pos;
        real det_A = determinant(a_b, a_c, r.dir);
        if (det_A == 0)
        {
            //std::cout << "a_b: " << a_b << " a_c: " << a_c << " a: " << a.v << " b: " << b.v << " c: " << c.v  <<  std::endl;
            //std::cout << "rdir: " << r.dir  <<  std::endl;
            return result;
        }

        real beta = determinant(a_o, a_c, r.dir) / det_A;
        if (beta > 1) return result;

        real gamma = determinant(a_b, a_o, r.dir) / det_A;
        if (gamma > 1) return result;

        if (beta >= 0 && gamma >= 0 && beta + gamma <= 1)
        {
            // there is an intersection
            real t_temp = determinant(a_b, a_c, a_o) / det_A;

            if (!shadow_test && t_temp < t_min && t_temp > 0)
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


void Triangle::BaryCentric(real &alpha, real& beta, real& gamma, const Vertex& v) const
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
    beta = (d11 * d20 - d01 * d21) / denom;
    gamma = (d00 * d21 - d01 * d20) / denom;
    alpha = 1.0 - beta - gamma;
}

Vec3r Triangle::getNormal(const Vertex& v, uint32_t triID, real time) const
{
    Vec3r normal;

    if (shadingType == ShadingType::SMOOTH)
    {
        real alpha, beta, gamma;
        BaryCentric(alpha,beta,gamma,v);
        Vec3r interpolated_normal = a.n * alpha + b.n * beta + c.n * gamma;
        interpolated_normal = interpolated_normal.normalize();
        //if (dot_product(interpolated_normal, n) < 0.0) interpolated_normal = -interpolated_normal;
        normal = interpolated_normal;
    }
    else normal = n;

    normal = getTexturedNormal(v, normal, time, triID);
    return normal;
}

Texel Triangle::getTexel(const Vertex &v, real time, int triID) const
{
    real alpha, beta, gamma;
    BaryCentric(alpha,beta,gamma,v);
    // if (AllTexture)
    // std::cout << a.t.u << " " << a.t.v << " " << b.t.u << std::endl;
    return alpha * a.t + beta * b.t + gamma * c.t;
}

Triangle::Triangle(const uint32_t id, CVertex& v1, CVertex& v2, CVertex& v3, Material& material,
                   std::vector<Texture*> ts, const ShadingType st, bool v, bool computeVNormals) :
    Object(material, id,
           maxVert3(v1.v, v2.v, v3.v),
           minVert3(v1.v, v2.v, v3.v), ts, v
    ),
    shadingType(st), a(v1), b(v2), c(v3), a_b(a.v - b.v), a_c(a.v - c.v)
{
    n = x_product((b.v - a.v), (c.v - a.v));
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
    ComputeBitan(b,a,c,T,B, n);
}

////////////////////////////////////////////////
////////////////// SPHERE /////////////////////
////////////////////////////////////////////////
///
Sphere::Sphere(uint32_t id, CVertex& c, real r, Material& m, std::vector<Texture*> ts, bool v)
    : Object(m, id, Vertex(c.v.x + r, c.v.y + r, c.v.z + r),
             Vertex(c.v.x - r, c.v.y - r, c.v.z - r), ts, v), center(c), radius(r)
{
    radius2 = radius * radius;
    main_center = c.v;
}


Texel Sphere::getTexel(const Vertex& v, real time, int triID) const
{
    Vec3r v_c = v - center.v;
    real theta = acos((v_c.j) / radius);
    real phi = atan2(v_c.k, v_c.i);
    return Texel((-phi + M_PI)/(2*M_PI), theta / M_PI);
}


void Sphere::getBitan(const Vertex &v, Vec3r &pT, Vec3r &pB, int triID) const
{
    real x = v.x - center.v.x;
    real y = v.y - center.v.y;
    real z = v.z - center.v.z;
    real theta = acos(y / radius);
    real phi = atan2( z, x);
    pT = {static_cast<real>(2*M_PI*z), 0, static_cast<real>(-2*M_PI*x)};
    pB = {static_cast<real>(M_PI * y * cos(phi)),
             static_cast<real>(-M_PI * radius * sin(theta)),
             static_cast<real>(M_PI * y * sin(phi))};
    pT = pT.normalize();
    pB = pB.normalize();
}


ObjectType Sphere::getObjectType() const { return ObjectType::SPHERE; }


Object::intersectResult Sphere::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull,
                                                  real time) const
{
    intersectResult result;
    result.currTri = 0;
    result.t_min = t_min;
    result.obj = nullptr;
    if (!visible) return result;
    if (globalBbox.intersects(r))
    {
        real t_temp;
        Vec3r o_c = r.pos - center.v;

        real A = dot_product(r.dir, r.dir);
        real B = dot_product(r.dir, o_c);
        real C = dot_product(o_c, o_c) - radius2;

        real BB_AC = B * B - A * C;

        if (BB_AC > 0)
        {
            real sqrt_BB_AC_A = sqrt(BB_AC) / A;
            real res_1 = (-B + sqrt(BB_AC)) / A;
            real res_2 = (-B - sqrt(BB_AC)) / A;

            if (res_1 <= res_2 && res_1 > 0 && res_2 > 0) t_temp = res_1;
            else if (res_1 > 0 && res_2 > 0) t_temp = res_2;
            else if (res_1 > 0) t_temp = res_1;
            else if (res_2 > 0) t_temp = res_2;
            else t_temp = INFINITY;
        }
        else if (BB_AC == 0) t_temp = -B / A;
        else /*BB_AC < 0*/ t_temp = INFINITY;


        if (!shadow_test && t_temp < t_min && t_temp > 0)
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

Vec3r Sphere::getNormal(const Vertex& v, uint32_t triID, real time) const
{
    Vec3r normal = (v - center.v).normalize();
    normal = getTexturedNormal(v, normal, time, triID);
    return normal;
}


////////////////////////////////////////////////
/////////////////// PLANE /////////////////////
////////////////////////////////////////////////

Plane::Plane(uint32_t id, Vertex& v, std::string normal, Material& material, std::vector<Texture*> ts, bool vis) :
    Object(material, id, Vertex(INFINITY,INFINITY,INFINITY), Vertex(-INFINITY, -INFINITY, -INFINITY), ts, vis), point(v)
{
    std::istringstream ss(normal);
    ss >> n.i >> n.j >> n.k;
    if (ss.fail())
    {
        throw std::invalid_argument("Invalid Normal string for plane: " + normal);
    }
    if (n.i == 0.0 && n.j == 0.0)
    {
        globalBbox.vMax.z = v.z + M4T_EPS;
        globalBbox.vMin.z = v.z - M4T_EPS;
    }
    else if (n.i == 0.0 && n.k == 0.0)
    {
        globalBbox.vMax.y = v.y + M4T_EPS;
        globalBbox.vMin.y = v.y - M4T_EPS;
    }
    else if (n.k == 0.0 && n.j == 0.0)
    {
        globalBbox.vMax.x = v.x + M4T_EPS;
        globalBbox.vMin.x = v.x - M4T_EPS;
    }

    std::pair<Vec3r,Vec3r> onb = getONB(n);
    T = onb.first;
    B = onb.second;
}

void Plane::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const
{
    pT = T;
    pB = B;
}

Texel Plane::getTexel(const Vertex& vert, real time, int triID) const
{
    real u = dot_product(vert - Vertex(0.0,0.0,0.0), T);
    real v = dot_product(vert - Vertex(0.0,0.0,0.0), B);

    u = fmod(u, 1.0);
    if (u < 0) u += 1.0;

    v = fmod(v, 1.0);
    if (v < 0) v += 1.0;

    return Texel(u, v);
}

ObjectType Plane::getObjectType() const { return ObjectType::PLANE; }

Object::intersectResult Plane::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull,
                                                 real time) const
{
    real dot_r_n = dot_product(r.dir, n);
    intersectResult result;
    result.currTri = 0;
    result.t_min = t_min;
    result.obj = nullptr;
    if (dot_r_n == 0) return result;

    real t_temp = dot_product((point - r.pos), n) / dot_r_n;

    if (!shadow_test && t_temp < t_min && t_temp > 0)
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

Vec3r Plane::getNormal(const Vertex& v, uint32_t currTri, real time) const
{

    return getTexturedNormal(v, n, time, currTri);
}


////////////////////////////////////////////////
///////////////// INSTANCE /////////////////////
////////////////////////////////////////////////


Instance::Instance(uint32_t id, Object* o, std::shared_ptr<Transformation> trans, Material& mat, Vec3r m,
                   std::vector<Texture*> ts, bool orig, bool v) :
    Object(mat, id,
           Vertex(), Vertex(), ts, v), forwardTrans(std::move(trans))
{
    motion = m;
    if (m.i == 0.0 && m.j == 0.0 && m.k == 0.0) has_motion = false;
    else has_motion = true;
    if (orig)
    {
        // unique
        original = o;
    }
    else
    {
        // share
        original = o;
    }
    backwardTrans = forwardTrans->inv()->clone();
    computeGlobal();
    main_center = ((*forwardTrans) * Vec4r(original->main_center)).getVertex();
    forwardTrans->getNormalTransform();
    backwardTrans->getNormalTransform();
}

void Instance::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID) const
{
    original->getBitan(v, pT, pB, triID);
}


Texel Instance::getTexel(const Vertex& v, real time, int triID) const
{
    Vertex localV = getLocal(v, time);
    if (forwardTrans->Determinant() < 0)
    {
        Texel t(0.0,0.0);
        real alpha, beta, gamma;
        switch (original->getObjectType())
        {
            case ObjectType::PLANE:
            break;
        case ObjectType::TRIANGLE:
            {
                Triangle* tri = dynamic_cast<Triangle*>(original);
                tri->BaryCentric(alpha,beta,gamma,localV);
                std::swap(beta, gamma);

                t = alpha * tri->a.t + beta * tri->b.t + gamma * tri->c.t;
                return t;
                break;
            }
            case ObjectType::MESH:
            {
                Triangle* tri = dynamic_cast<Mesh*>(original)->Faces[triID];
                tri->BaryCentric(alpha,beta,gamma,localV);
                std::swap(beta, gamma);

                t = alpha * tri->a.t + beta * tri->b.t + gamma * tri->c.t;
                return t;
            }
            break;
        }
    }

    return original->getTexel(localV, 0, triID);
}

Instance::~Instance()
{
}

ObjectType Instance::getObjectType() const { return ObjectType::INSTANCE; }

Instance::intersectResult Instance::checkIntersection(const Ray& ray, const real& t_min, bool shadow_test,
                                                      bool back_cull, real time) const
{
    // std::cout << "Check Intersection of Instance" << std::endl;
    intersectResult result;
    result.t_min = t_min;
    result.currTri = 0;
    result.obj = nullptr;

    if (!shadow_test)
    {
        Ray localRay = getLocal(ray, time);
        result = original->checkIntersection(localRay, t_min, shadow_test, back_cull, 0);
        result.obj = result.obj ? this : nullptr;
        return result;
    }
    else
    {
        Ray localRay = getLocal(ray, time); // get the local light point
        result = original->checkIntersection(localRay, t_min, shadow_test, back_cull, 0);
        if (result.obj)
        {
            Vertex intersect = localRay.pos + localRay.dir * t_min;
            Vertex globalIntersect = getGlobal(intersect, time); // o + t*d
            result.t_min = (globalIntersect.x - ray.pos.x) / ray.dir.i;
            result.obj = this;
            return result;
        }
        result.obj = nullptr;
        return result;
    }
}

Vec3r Instance::getNormal(const Vertex& v, uint32_t triID, real time) const
{
    Vertex localV = getLocal(v, time);
    Vec3r res = original->getNormal(localV, triID, 0);
    res = getGlobalNormal(res, time);
    return res;
}


void Instance::computeGlobal()
{
    if (forwardTrans != nullptr && backwardTrans != nullptr)
    {
        globalBbox.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
        globalBbox.vMin = Vertex(INFINITY, INFINITY, INFINITY);
        Vertex v[2] = {original->globalBbox.vMax, original->globalBbox.vMin};
        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 2; j++)
            {
                for (int k = 0; k < 2; k++)
                {
                    if (has_motion)
                    {
                        globalBbox.vMax = maxVert3(globalBbox.vMax, getGlobal(Vertex(v[i].x, v[j].y, v[k].z), 1.0),
                                                   getGlobal(Vertex(v[i].x, v[j].y, v[k].z), 0.0));
                        globalBbox.vMin = minVert3(globalBbox.vMin, getGlobal(Vertex(v[i].x, v[j].y, v[k].z), 1.0),
                                                   getGlobal(Vertex(v[i].x, v[j].y, v[k].z), 0.0));
                    }
                    else
                    {
                        globalBbox.vMax = maxVert2(globalBbox.vMax, getGlobal(Vertex(v[i].x, v[j].y, v[k].z), 0.0));
                        globalBbox.vMin = minVert2(globalBbox.vMin, getGlobal(Vertex(v[i].x, v[j].y, v[k].z), 0.0));
                    }
                }
            }
        }
    }
}

Vertex Instance::getLocal(const Vertex& v, real time) const
{
    if (has_motion && time > 0) return ((*backwardTrans) * Translate(-motion * time) * Vec4r(v)).getVertex();
    else return ((*backwardTrans) * Vec4r(v)).getVertex();
}

Ray Instance::getLocal(const Ray& r, real time) const
{
    if (has_motion && time > 0) return (*backwardTrans) * Translate(-motion * time) * r;
    else return (*backwardTrans) * r;
}

Vec3r Instance::getLocal(Vec3r& v, real time)
{
    if (has_motion && time > 0) return ((*backwardTrans) * Translate(-motion * time) * Vec4r(v)).getVec3r();
    else return ((*backwardTrans) * Vec4r(v)).getVec3r();
}

///// GET GLOBAL //////

Vec3r Instance::getGlobal(Vec3r& v, double time)
{
    if (has_motion && time > 0) return (Translate(motion * time) * (*forwardTrans) * Vec4r(v)).getVec3r();
    else return ((*forwardTrans) * Vec4r(v)).getVec3r();
}

Vertex Instance::getGlobal(Vertex v, real time) const
{
    if (has_motion && time > 0) return (Translate(motion * time) * (*forwardTrans) * Vec4r(v)).getVertex();
    else return ((*forwardTrans) * Vec4r(v)).getVertex();
}

Vec3r Instance::getGlobalNormal(const Vec3r& res, double time) const
{
    Composite temp_f;
    Transformation* fwt = forwardTrans.get();
    if (has_motion && time > 0)
    {
        temp_f = Translate(motion * time) * (*forwardTrans);
        fwt = &temp_f;
        fwt->getNormalTransform();
    }
    return ((fwt->normalTransform) * Vec4r(res)).getVec3r().normalize();
}
