//
// Created by vicy on 11/01/25.
//

#include "transformation.h"
#include "../functions/overloads.h"

////////////////////////////////////////////////
///////// Transformation /////////////////////
////////////////////////////////////////////////

TransformationType Transformation::getTransformationType() const
{return TransformationType::NONE;};


void Transformation::getNormalTransform()
{
    normalTransform = this->Inverse().Transpose();
}

////////////////////////////////////////////////
///////////////// Rotate /////////////////////
////////////////////////////////////////////////

Rotate::Rotate(Ray ax, double ang) : axis(ax), angle(ang)
{
    real a = ax.dir.i;
    real b = ax.dir.j;
    real c = ax.dir.k;
    real K = 1.0 - cos(ang);
    arr = {cos(ang) + pow(a,2)*K,       a*b*K - c* sin(ang),        a*c*K + b*sin(ang), 0,
              a*b*K + c* sin(ang), cos(ang) + pow(b,2)*K,        b*c*K - a*sin(ang), 0,
              a*c*K - b* sin(ang),       b*c*K + a* sin(ang), cos(ang) + pow(c,2)*K, 0,
                                0,                         0,                         0, 1
    };
    getNormalTransform();
}

Rotate::Rotate(Axes ax, double ang)
{
    if (ax == Axes::x)
    {
        axis = Ray(Vertex(), Vec3r(1.0,0.0,0.0));
        arr = {
            1.0,      0.0,      0.0,   0.0,
            0.0, cos(ang),-sin(ang),   0.0,
            0.0, sin(ang), cos(ang),   0.0,
            0.0,      0.0,      0.0,   1.0
        };
    }
    else if (ax == Axes::y)
    {
        axis = Ray(Vertex(), Vec3r(0.0,1.0,0.0));
        arr = {
            cos(ang),      0.0, sin(ang),   0.0,
                 0.0,      1.0,      0.0,   0.0,
           -sin(ang),      0.0, cos(ang),   0.0,
                 0.0,      0.0,      0.0,   1.0
        };
    }
    else if (ax == Axes::z)
    {
        axis = Ray(Vertex(), Vec3r(0.0,0.0,1.0));
        arr = {
            cos(ang),-sin(ang),      0.0,   0.0,
            sin(ang), cos(ang),      0.0,   0.0,
                 0.0,      0.0,      1.0,   0.0,
                 0.0,      0.0,      0.0,   1.0
        };
    }

    angle = ang;
    getNormalTransform();
}


Rotate::Rotate(const Rotate& rotate)
{
    axis.pos = rotate.axis.pos;
    axis.dir = rotate.axis.dir;
    angle = rotate.angle;
    normalTransform = rotate.normalTransform;
}


Rotate Rotate::operator=(const Rotate& rotate)
{
    if (this == &rotate) return *this;
    axis.pos = rotate.axis.pos;
    axis.dir = rotate.axis.dir;
    angle = rotate.angle;
    normalTransform = rotate.normalTransform;
    return *this;
}


Transformation *Rotate::inv() const
{
    return new Rotate(axis,-angle);
}

TransformationType Rotate::getTransformationType() const { return TransformationType::ROTATE;}


////////////////////////////////////////////////
//////////////// Translate /////////////////////
////////////////////////////////////////////////


Translate::Translate(Vertex v) : x(v.x), y(v.y), z(v.z)
{
    arr = {
        {
            {{1, 0, 0, x}},
            {{0, 1, 0, y}},
            {{0, 0, 1, z}},
            {{0, 0, 0, 1}}
        }
    };
    normalTransform = Identity();
}

Translate::Translate(real x, real y, real z) : x(x), y(y), z(z)
{
    arr = {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
    normalTransform = Identity();
}



TransformationType Translate::getTransformationType() const  { return TransformationType::TRANSLATE; }

Transformation *Translate::inv() const
{
    return new Translate(-x,-y,-z);
}


Scale::Scale(Vertex v) : center(Vertex()), x(v.x), y(v.y), z(v.z)
{
    arr = {
        v.x, 0, 0, 0,
        0, v.y, 0, 0,
        0, 0, v.z, 0,
        0, 0, 0, 1
    };
    getNormalTransform();
}


Scale::Scale(real x, real y, real z) : center(Vertex()), x(x), y(y), z(z)
{
    arr = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
    getNormalTransform();
}

Scale::Scale(Vertex center, real x, real y, real z) : center(center), x(x), y(y), z(z), Transformation()
{
    arr = (Translate(-center)*Scale(x,y,z)*Translate(center)).arr;
    getNormalTransform();
}
TransformationType Scale::getTransformationType() const   { return TransformationType::SCALE; }

Transformation *Scale::inv() const
{
    return new Scale(center,1.0/x,1.0/y,1.0/z);
}


Composite::Composite(const std::vector<Transformation *>& transformations)
{
    arr = Identity().arr;
    for (int i=0; i < transformations.size(); i++)
    {
        //std::cout << *transformations[i];
        arr = (*transformations[i] * *this).arr;
    }

    getNormalTransform();
}


Composite::Composite(M4trix m)
{
    arr = m.arr;
    getNormalTransform();
}

TransformationType Composite::getTransformationType() const  { return TransformationType::COMPOSITE; }

Composite::Composite()
{
    arr = Identity().arr;
    getNormalTransform();
}

Transformation *Composite::inv() const
{
    return new Composite(this->Inverse());
}


Composite::Composite(const Composite &c)
{
    arr = c.arr;
    normalTransform = c.normalTransform;
}

Scale Scale::operator=(const Scale &c)
{
    if (this == &c) return *this;
    arr = c.arr;
    center = c.center;
    x = c.x;
    y = c.y;
    z = c.z;
    normalTransform = c.normalTransform;
    return *this;
}

Scale::Scale(const Scale &c)
{
    arr = c.arr;
    center = c.center;
    x = c.x;
    y = c.y;
    z = c.z;
    normalTransform = c.normalTransform;
}


Translate::Translate(const Translate &c)
{
    arr = c.arr;
    x = c.x;
    y = c.y;
    z = c.z;
    normalTransform = Identity();
}


Translate Translate::operator=(const Translate &c)
{
    if (this == &c) return *this;
    arr = c.arr;
    x = c.x;
    y = c.y;
    z = c.z;
    normalTransform = c.normalTransform;
    return *this;
}


Composite Composite::operator=(const Composite &other)
{
    if (this == &other) return *this;
    arr = other.arr;
    normalTransform = other.normalTransform;
    return *this;

}

