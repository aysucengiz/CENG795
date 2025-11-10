//
// Created by vicy on 11/01/25.
//

#include "transformation.h"

////////////////////////////////////////////////
///////// Transformation /////////////////////
////////////////////////////////////////////////

TransformationType Transformation::getTransformationType() const
{return TransformationType::NONE;};



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
    normalTransform = this->Transpose().Inverse();
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
}


Rotate::Rotate(const Rotate& rotate)
{
    axis.pos = rotate.axis.pos;
    axis.dir = rotate.axis.dir;
    angle = rotate.angle;
    normalTransform = rotate.normalTransform;
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
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
    normalTransform.arr = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -x, -y, -z, 1
    };
}

Translate::Translate(real x, real y, real z) : x(x), y(y), z(z)
{
    arr = {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        0, 0, 0, 1
    };
    normalTransform.arr = {
        1, 0, 0, x,
        0, 1, 0, y,
        0, 0, 1, z,
        -x, -y, -z, 1
    };
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
    normalTransform.arr = {
        1.0/v.x, 0, 0, 0,
        0, 1.0/v.y, 0, 0,
        0, 0, 1.0/v.z, 0,
        0, 0, 0, 1
    };
}


Scale::Scale(real x, real y, real z) : center(Vertex()), x(x), y(y), z(z)
{
    arr = {
        x, 0, 0, 0,
        0, y, 0, 0,
        0, 0, z, 0,
        0, 0, 0, 1
    };
    normalTransform.arr = {
        1.0/x, 0, 0, 0,
        0, 1.0/y, 0, 0,
        0, 0, 1.0/z, 0,
        0, 0, 0, 1
    };
}

Scale::Scale(Vertex center, real x, real y, real z) : center(center), x(x), y(y), z(z), Transformation()
{
    arr = (Translate(-center)*Scale(x,y,z)*Translate(center)).arr;
    normalTransform = (this->Transpose()).Inverse();
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
        arr = (*transformations[i] * M4trix(arr)).arr;
    }
    normalTransform = (this->Transpose()).Inverse();
}


Composite::Composite(M4trix m)
{
    arr = m.arr;
}

TransformationType Composite::getTransformationType() const  { return TransformationType::COMPOSITE; }

Composite::Composite()
{
    arr = Identity().arr;
}

Transformation *Composite::inv() const
{
    return new Composite(Inverse());
}

