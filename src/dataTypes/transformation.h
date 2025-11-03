//
// Created by vicy on 11/01/25.
//

#ifndef CENG795_TRANSFORMATION_H
#define CENG795_TRANSFORMATION_H

#include "DataTypes.h"
#include "Matrix.h"

enum class TransformationType
{
    NONE,
    ROTATE,
    TRANSLATE,
    SCALE,
    COMPOSITE
};


class Transformation: public M4trix
{
public:
    virtual ~Transformation() = default;
    virtual TransformationType getTransformationType() const = 0;


};

class Rotate : public Transformation
{
private:
    Ray axis;
    real angle;
public:
    Rotate(Ray axis, float angle);
    Rotate(char axis, float angle);
    TransformationType getTransformationType() const override{ return TransformationType::ROTATE;}
};

class Translate : public Transformation
{
private:
    real x,y,z;
public:
    Translate(Vertex &v) : x(v.x), y(v.y), z(v.z){}
    Translate(real x, real y, real z) : x(x), y(y), z(z)
    {
        ptr[0][3] = x;
        ptr[1][3] = y;
        ptr[2][3] = z;
    }
    TransformationType getTransformationType() const override { return TransformationType::TRANSLATE; }
};


class Scale : public Transformation
{
private:
    Vertex center;
    real x, y, z;
public:
    Scale(real x, real y, real z) : center(Vertex()), x(x), y(y), z(z)
    {
        ptr[0][0] = x;
        ptr[1][1] = y;
        ptr[2][2] = z;
    }

    Scale(Vertex center, real x, real y, real z) : center(center), x(x), y(y), z(z), Transformation()
    {
        static_cast<M4trix&>(*this) = Translate(-center)*Scale(x,y,z)*Translate(center);
    }
    TransformationType getTransformationType() const override  { return TransformationType::SCALE; }
};

class Composite : public Transformation
{
public:
    Composite(const std::vector<Transformation> transformations);
    Composite(M4trix &m);
    TransformationType getTransformationType() const override { return TransformationType::COMPOSITE; }
    Composite();
};




#endif //CENG795_TRANSFORMATION_H