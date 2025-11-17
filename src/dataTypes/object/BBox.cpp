//
// Created by vicy on 11/08/25.
//

#include "BBox.h"
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"

BBox::BBox()
{
    Vec3r temp = vMax - vMin;
    area = 2* (temp.i * temp.j +
               temp.i * temp.k +
               temp.j * temp.k);
};


BBox::BBox(real xmax, real xmin, real ymax, real ymin, real zmax, real zmin)
    : vMax(xmax, ymax, zmax), vMin(xmin, ymin, zmin)
{
    Vec3r temp = vMax - vMin;
    area = 2* (temp.i * temp.j +
               temp.i * temp.k +
               temp.j * temp.k);
};

BBox::BBox(Vertex vMax, Vertex vMin)
    : vMax(vMax), vMin(vMin)
{
    Vec3r temp = vMax - vMin;
    area = 2* (temp.i * temp.j +
               temp.i * temp.k +
               temp.j * temp.k);
};

bool BBox::isWithin(const Vertex& v)
{
    if (v.x <= vMax.x && v.x >= vMin.x &&
        v.y <= vMax.y && v.y >= vMin.y &&
        v.z <= vMax.z && v.z >= vMin.z)
        return true;
    return false;
}

bool BBox::intersects(const Ray& r) const
{
    Vec3r t_min = (vMin - r.pos) / r.dir;
    Vec3r t_max = (vMax - r.pos) / r.dir;

    if (t_max.i < t_min.i) std::swap(t_max.i, t_min.i);
    if (t_max.j < t_min.j) std::swap(t_max.j, t_min.j);
    if (t_max.k < t_min.k) std::swap(t_max.k, t_min.k);

    real t1 = max3(t_min.i, t_min.j, t_min.k);
    real t2 = min3(t_max.i, t_max.j, t_max.k);

    if (t1 > t2) return false;
    return true;
}


real BBox::getArea() const
{
    return area;
}
