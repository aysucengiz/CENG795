//
// Created by vicy on 11/08/25.
//

#ifndef CENG795_BBOX_H
#define CENG795_BBOX_H

#include "../base/SceneData.h"

struct BBox
{
    Vertex vMax, vMin;
    BBox();
    BBox(real xmax, real xmin, real ymax, real ymin, real zmax, real zmin);
    BBox(Vertex vmax, Vertex vMin);
    bool isWithin(const Vertex &v);
    bool intersects(const Ray &r) const;
    real getArea() const;
};


#endif //CENG795_BBOX_H