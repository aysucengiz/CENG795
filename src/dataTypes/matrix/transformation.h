//
// Created by vicy on 11/01/25.
//

#ifndef CENG795_TRANSFORMATION_H
#define CENG795_TRANSFORMATION_H

#include <memory>

#include "../../typedefs.h"
#include "../base/SceneData.h"
#include "Matrix.h"


class Transformation: public M4trix
{
public:
    M4trix normalTransform;
    virtual ~Transformation() = default;
    virtual TransformationType getTransformationType() const;
    virtual Transformation *inv() const = 0;
    void getNormalTransform();
    virtual std::unique_ptr<Transformation> clone() const = 0;
};

class Rotate : public Transformation
{
public:
    Ray axis;
    real angle;
    Rotate(Ray ax, double ang);
    Rotate(Axes ax, double ang);
    Rotate(const Rotate& rotate);
    Rotate operator=(const Rotate& rotate);
    TransformationType getTransformationType() const override;
    Transformation *inv() const override;

    std::unique_ptr<Transformation> clone() const override;

};

class Translate : public Transformation
{
private:
public:
    real x,y,z;
    Translate(Vertex v);
    Translate(real x, real y, real z);
    Translate(const Translate &c);
    Translate operator=(const Translate &c);
    TransformationType getTransformationType() const override;
    Transformation *inv() const override;

    std::unique_ptr<Transformation> clone() const override;
};


class Scale : public Transformation
{
private:
public:
    Vertex center;
    real x, y, z;
    Scale(Vertex v);
    Scale(real x, real y, real z);
    Scale(Vertex center, real x, real y, real z);
    Scale(const Scale &c);
    Scale operator=(const Scale &c);
    TransformationType getTransformationType() const override;
    Transformation *inv() const override;
    std::unique_ptr<Transformation> clone() const override;
};

class Composite : public Transformation
{
public:
    Composite(const std::vector<Transformation *>& transformations);
    Composite(M4trix m);
    Composite(const Composite &c);
    Composite operator=(const Composite &other);
    TransformationType getTransformationType() const override;
    Composite();
    Transformation *inv() const override;
    std::unique_ptr<Transformation> clone() const override;
};






#endif //CENG795_TRANSFORMATION_H