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
    NONE,
    GRID,
    KD_TREE,
    BVH
};

enum class ThreadType
{
    NONE,
    ROW,
    BATCH
};

enum class SamplingType
{
    UNIFORM,
    RANDOM,
    STRATIFIED,
    N_ROOKS,
    MULTI_JITTERED,
    VAN_DER_CORPUT,
    HALTON,
    HAMMERSLEY
};

enum class FilterType
{
    BOX,
    GAUSSIAN,
    GAUSSIAN_ZERO
};

enum class LightType
{
    POINT,
    AREA
};

enum class TextureType
{
    IMAGE,
    PERLIN,
    CHECKERBOARD
};
#endif //CENG795_TYPEDEFS_H