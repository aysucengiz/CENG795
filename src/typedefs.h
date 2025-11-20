//
// Created by vicy on 11/08/25.
//

#ifndef CENG795_TYPEDEFS_H
#define CENG795_TYPEDEFS_H
#include <cstdint>

typedef float real;

enum class MaterialType{
    NONE,
    NORMAL,
    MIRROR,
    CONDUCTOR,
    DIELECTRIC
};

enum class ObjectType{
    NONE,
    TRIANGLE,
    SPHERE,
    MESH,
    PLANE,
    INSTANCE
};

enum class ShadingType{
    NONE,
    SMOOTH,
    FLAT
};

enum class TransformationType
{
    NONE,
    ROTATE,
    TRANSLATE,
    SCALE,
    COMPOSITE
};

enum class Axes
{
    x,
    y,
    z
};

enum class BVHNodeType : uint8_t
{
    LEAF,
    INT_W_LEFT,
    INT_W_RIGHT,
    INT_W_BOTH
};



enum class PivotType
{
    MIDDLE,
    MEDIAN,
    SAH
};

enum class AccelerationType
{
    BVH
};

enum class ThreadType
{
    ROW,
    BATCH
};

#endif //CENG795_TYPEDEFS_H