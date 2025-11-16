//
// Created by vicy on 11/08/25.
//

#include "BBox.h"
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"

BBox::BBox()
{
};


BBox::BBox(real xmax, real xmin, real ymax, real ymin, real zmax, real zmin)
    : vMax(xmax, ymax, zmax), vMin(xmin, ymin, zmin)
{
};

BBox::BBox(Vertex vMax, Vertex vMin)
    : vMax(vMax), vMin(vMin)
{
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
    real t_x[2] = {INFINITY, INFINITY};
    real t_y[2] = {INFINITY, INFINITY};
    real t_z[2] = {INFINITY, INFINITY};

    t_x[0] = (vMin.x - r.pos.x) / r.dir.i;
    t_x[1] = (vMax.x - r.pos.x) / r.dir.i;
    if (t_x[1] < t_x[0]) std::swap(t_x[0], t_x[1]);

    t_y[0] = (vMin.y - r.pos.y) / r.dir.j;
    t_y[1] = (vMax.y - r.pos.y) / r.dir.j;
    if (t_y[1] < t_y[0]) std::swap(t_y[0], t_y[1]);

    t_z[0] = (vMin.z - r.pos.z) / r.dir.k;
    t_z[1] = (vMax.z - r.pos.z) / r.dir.k;
    if (t_z[1] < t_z[0]) std::swap(t_z[0], t_z[1]);

    real t1 = max3(t_x[0], t_y[0], t_z[0]);
    real t2 = min3(t_x[1], t_y[1], t_z[1]);

    if (t1 > t2) return false;
    return true;
}


real BBox::getArea() const
{
    Vec3r temp = vMax - vMin;
    return 2* (temp.i * temp.j +
               temp.i * temp.k +
               temp.j * temp.k);
}
