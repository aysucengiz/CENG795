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

Color Object::getTextureColorAt(Vertex& pos, real time, int triID, Texel rate_of_change) const
{
    if (AllTexture != nullptr)
    {
        MipMap& mip0 = dynamic_cast<ImageTexture*>(AllTexture)->image->mipmaps[0];
        real a = rate_of_change.u * mip0.width;
        real b = rate_of_change.v * mip0.height;
        real level = 0.5 * log2(a * a + b * b);
        Texel tex = getTexel(pos, time, triID);
        // std::cout << level << std::endl;
        return AllTexture->TextureColor(pos, tex, level);
    }
    return Color(0, 0, 0);
}




Color Object::GetColourAt(Color I_R_2,const Vec3r& normal, const Ray& ray, Ray& shadow_ray, real time,
                          int triID, Texel& rate_of_change) const
{
    if (I_R_2.isBlack()) return Color(0, 0, 0);

    // get texel if needed
    Texel tex = {0.0,0.0};
    if(DiffuseTexture != nullptr || SpecularTexture != nullptr) tex = getTexel(shadow_ray.pos, time, triID);

    // get kd and ks
    Color kd = diffuseTerm(shadow_ray.pos, tex, time, rate_of_change);
    Color ks = specularTerm(shadow_ray.pos, tex, rate_of_change);
    real cos_theta = material.brdf->getCosTheta(normal,shadow_ray.dir);

    // get brdf
    Color f = material.brdf->Guards_BRDF_This_Man(kd, ks, material.PhongExponent, material.RefractionIndex, normal, ray.dir, shadow_ray.dir);

    return f * cos_theta * I_R_2;
}

real Object::getMipMapLevel(Texel rate_of_change) const
{
    MipMap mip0 = dynamic_cast<ImageTexture*>(DiffuseTexture)->image->mipmaps[0];
    real a = rate_of_change.u * mip0.width;
    real b = rate_of_change.v * mip0.height;
    return 0.5 * log2(a * a + b * b);
}

Color Object::diffuseTerm(Vertex& vert, Texel& t, real time, Texel rate_of_change) const
{
    Color kd = material.DiffuseReflectance;
    if (DiffuseTexture != nullptr)
    {
        real level = 0;
        if (DiffuseTexture->IsMipMapped()) level = getMipMapLevel(rate_of_change);
        Color tex_col = DiffuseTexture->TextureColor(vert, t, level);

        switch (DiffuseTexture->decalMode)
        {
        case DecalMode::REPLACE_KD:
            kd = tex_col;
            break;
        case DecalMode::BLEND_KD:
            kd = (kd + tex_col) / 2.0;
            break;
        }
    }

    if (material.degamma)
    {
        kd.r = pow(kd.r,2.2);
        kd.g = pow(kd.g,2.2);
        kd.b = pow(kd.b,2.2);
    }
    return kd;
}

Color Object::specularTerm( Vertex& vert, Texel& t, Texel rate_of_change) const
{
    Color ks = material.SpecularReflectance;
    if (SpecularTexture != nullptr)
    {
        real level = 0;
        if (SpecularTexture->IsMipMapped()) level = getMipMapLevel(rate_of_change);
        ks = SpecularTexture->TextureColor(vert, t, level) / 255.0;
    }

    if (material.degamma)
    {
        ks.r = pow(ks.r,2.2);
        ks.g = pow(ks.g,2.2);
        ks.b = pow(ks.b,2.2);
    }
    return ks;
}


real chenge_interval(real value)
{
    return value * 2 - 1.0;
}

real Object::GrayScale(Color c) const
{
    return (c.r + c.g + c.b) / (3.0);
}

real Object::h(Vertex v) const
{
    Texel t(0.0, 0.0);
    Color c = NormalTexture->TextureColor(v, t, 0);
    real result = (c.r + c.g + c.b) / 3.0;
    return result;
}


Vec3r Object::getTexturedNormal(const Vertex& v, const Vec3r& n, real time, int triID) const
{
    Vec3r NewN = n;
    if (NormalTexture != nullptr)
    {
        Texel t = getTexel(v, time, triID);
        Color textureColor = NormalTexture->TextureColor(v, t, 0);
        Vec3r locNormal(textureColor.r * 2 - 1.0, textureColor.g * 2 - 1.0, textureColor.b * 2 - 1.0);
        locNormal = locNormal.normalize();
        Vec3r onb[3];
        onb[2] = n;
        // std::cout << onb[0] << " " << onb[1] << " " << onb[2] << std::endl;
        if (NormalTexture->decalMode == DecalMode::REPLACE_NORMAL)
        {
            getBitan(v, onb[0], onb[1], triID, false, time);
            if (onb[0].mag() < 1e-8 || onb[1].mag() < 1e-8)
            {
                return NewN;
            }
            NewN = Vec3r(0.0, 0.0, 0.0);
            for (int i = 0; i < 3; i++)
                for (int j = 0; j < 3; j++)
                    NewN[(Axes)i] += locNormal[(Axes)j] * onb[j][(Axes)i];
        }
        else if (NormalTexture->decalMode == DecalMode::BUMP_NORMAL)
        {
            if (NormalTexture->getTextureType() == TextureType::IMAGE)
            {
                getBitan(v, onb[0], onb[1], triID, false, time);
                if (onb[0].mag() < 1e-8 || onb[1].mag() < 1e-8)
                {
                    return NewN;
                }
                onb[2] = x_product(onb[1], onb[0]); //n;

                Vec3r p_u = onb[0];
                Vec3r p_v = onb[1];
                ImageTexture* imtex = dynamic_cast<ImageTexture*>(NormalTexture);
                real w = imtex->image->mipmaps[0].width;
                real h = imtex->image->mipmaps[0].height;

                Texel xy = {(real)fmod(t.u, 1.0) * w, (real)fmod(t.v, 1.0) * h};
                Texel pq = {std::floor(xy.u), std::floor(xy.v)};

                real n_curr = GrayScale(imtex->ImageColor(pq.u, pq.v, 0));
                real n_up = GrayScale(imtex->ImageColor(pq.u, pq.v + 1, 0));
                real n_right = GrayScale(imtex->ImageColor(pq.u + 1, pq.v, 0));
                real dv = (n_up - n_curr) * NormalTexture->bumpFactor;
                real du = (n_right - n_curr) * NormalTexture->bumpFactor;

                Vec3r q_u = p_u + n * du;
                Vec3r q_v = p_v + n * dv;
                NewN = x_product(q_v, q_u);
                NewN = (dot_product(NewN, n) < 0) ? -NewN : NewN;
            }
            else
            {
                getBitan(v, onb[0], onb[1], triID, true, time);
                if (onb[0].mag() < 1e-8 || onb[1].mag() < 1e-8)
                {
                    return NewN;
                }
                real epsilon = 0.0001;

                real dhT = (h(v + onb[0] * epsilon) - h(v - onb[0] * epsilon)) / (2 * epsilon);
                real dhB = (h(v + onb[1] * epsilon) - h(v - onb[1] * epsilon)) / (2 * epsilon);

                Vec3r g_perp = onb[0] * dhT + onb[1] * dhB;
                Vec3r g_perp_factored = g_perp * NormalTexture->bumpFactor;
                NewN = n - g_perp_factored;
            }
        }
        NewN = NewN.normalize();
    }

    return NewN;
}

void Object::ComputeBitan(CVertex& b, CVertex& a, CVertex& c, Vec3r& pT, Vec3r& pB, Vec3r& n)
{
    real cv_av = c.t.v - a.t.v;
    real cu_au = c.t.u - a.t.u;
    real bv_av = b.t.v - a.t.v;
    real bu_au = b.t.u - a.t.u;
    real det = bu_au * cv_av - cu_au * bv_av;
    std::array<std::array<real,2>,2> M1{};
    if (std::abs(det) < 0.000001)
    {
        pT = Vec3r(0,0,0);
        pB = Vec3r(0,0,0);
        return;
    }


    M1 = {
        cv_av / det, -bv_av / det,
        -cu_au /det, bu_au / det
    };

    Vec3r A[2] = {b.v - a.v, c.v - a.v};
    Vec3r Bits[2] = {};

    for (int k = 0; k < 2; k++)
        for (int i = 0; i < 2; i++)
            for (int j = 0; j < 3; j++)
                Bits[k][(Axes)j] += M1[k][i] * A[i][(Axes)j];


    pT = Bits[0];
    pB = Bits[1];
    if (std::isnan(pT.i) || std::isnan(pT.j) || std::isnan(pT.k))
        std::cout << "T: " << pT << std::endl;
    if (std::isnan(pB.i) || std::isnan(pB.j) || std::isnan(pB.k))
        std::cout << "B: " << pB << std::endl;
}


////////////////////////////////////////////////
///////////////// TRIANGLE /////////////////////
////////////////////////////////////////////////

ObjectType Triangle::getObjectType() const { return ObjectType::TRIANGLE; }


void Triangle::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const
{
    if (normalize)
    {
        pT = T_norm;
        pB = B_norm;
    }
    else
    {
        pT = T;
        pB = B;
    }
}


Object::intersectResult Triangle::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull,
                                                    real time, real dist) const
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

            if (shadow_test && t_temp < dist && t_temp >= 0)
            {
                result.t_min = t_temp;
                result.obj = this;
                return result;
            }
        }
    }

    return result;
}


void Triangle::BaryCentric(real& alpha, real& beta, real& gamma, const Vertex& v) const
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
        BaryCentric(alpha, beta, gamma, v);
        Vec3r interpolated_normal = a.n * alpha + b.n * beta + c.n * gamma;
        interpolated_normal = interpolated_normal.normalize();
        //if (dot_product(interpolated_normal, n) < 0.0) interpolated_normal = -interpolated_normal;
        normal = interpolated_normal;
    }
    else normal = n;

    normal = getTexturedNormal(v, normal, time, triID);
    return normal;
}

Texel Triangle::getTexel(const Vertex& v, real time, int triID) const
{
    real alpha, beta, gamma;
    BaryCentric(alpha, beta, gamma, v);
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
    ComputeBitan(b, a, c, T, B, n);
    T_norm = T.normalize();
    B_norm = B.normalize();
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
    return Texel((-phi + M_PI) / (2 * M_PI), theta / M_PI);
}


void Sphere::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const
{
    real x = v.x - center.v.x;
    real y = v.y - center.v.y;
    real z = v.z - center.v.z;
    real theta = acos(y / radius);
    real phi = atan2(z, x);
    pT = {static_cast<real>(2 * M_PI * z), 0, static_cast<real>(-2 * M_PI * x)};
    pB = {
        static_cast<real>(M_PI * y * cos(phi)),
        static_cast<real>(-M_PI * radius * sin(theta)),
        static_cast<real>(M_PI * y * sin(phi))
    };
    if (normalize)
    {
        pT = pT.normalize();
        pB = pB.normalize();
    }
}


ObjectType Sphere::getObjectType() const { return ObjectType::SPHERE; }


Object::intersectResult Sphere::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull,
                                                  real time, real dist) const
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
        else if (shadow_test && t_temp < dist && t_temp >= 0)
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

    std::pair<Vec3r, Vec3r> onb = getONB(n);
    T = onb.first;
    B = onb.second;
}

void Plane::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const
{
    pT = T;
    pB = B;
}

Texel Plane::getTexel(const Vertex& vert, real time, int triID) const
{
    real u = dot_product(vert - Vertex(0.0, 0.0, 0.0), T);
    real v = dot_product(vert - Vertex(0.0, 0.0, 0.0), B);

    u = fmod(u, 1.0);
    if (u < 0) u += 1.0;

    v = fmod(v, 1.0);
    if (v < 0) v += 1.0;

    return Texel(u, v);
}

ObjectType Plane::getObjectType() const { return ObjectType::PLANE; }

Object::intersectResult Plane::checkIntersection(const Ray& r, const real& t_min, bool shadow_test, bool back_cull,
                                                 real time, real dist) const
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

    if (shadow_test && t_temp < dist && t_temp >= 0)
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


void Instance::getBitan(const Vertex& v, Vec3r& pT, Vec3r& pB, int triID, bool normalize, real time) const
{
    original->getBitan(getLocal(v, time), pT, pB, triID, normalize, time);
    pT = getGlobalNormal(pT, time);
    pB = getGlobalNormal(pB, time);
}


Texel Instance::getTexel(const Vertex& v, real time, int triID) const
{
    Vertex localV = getLocal(v, time);
    if (forwardTrans->Determinant() < 0)
    {
        Texel t(0.0, 0.0);
        real alpha, beta, gamma;
        switch (original->getObjectType())
        {
        case ObjectType::PLANE:
            break;
        case ObjectType::TRIANGLE:
            {
                Triangle* tri = dynamic_cast<Triangle*>(original);
                tri->BaryCentric(alpha, beta, gamma, localV);
                std::swap(beta, gamma);

                t = alpha * tri->a.t + beta * tri->b.t + gamma * tri->c.t;
                return t;
                break;
            }
        case ObjectType::MESH:
            {
                Triangle* tri = dynamic_cast<Mesh*>(original)->Faces[triID];
                tri->BaryCentric(alpha, beta, gamma, localV);
                // std::swap(beta, gamma);

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
                                                      bool back_cull, real time, real dist) const
{
    // std::cout << "Check Intersection of Instance" << std::endl;
    intersectResult result;
    result.t_min = t_min;
    result.currTri = 0;
    result.obj = nullptr;

    if (!shadow_test)
    {
        Ray localRay = getLocal(ray, time);
        result = original->checkIntersection(localRay, t_min, shadow_test, back_cull, 0, dist);
        result.obj = result.obj ? this : nullptr;
        return result;
    }
    else
    {
        Ray localRay = getLocal(ray, time); // get the local light point
        result = original->checkIntersection(localRay, t_min, shadow_test, back_cull, 0, dist);
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