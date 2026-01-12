//
// Created by vicy on 11/08/25.
//

#ifndef CENG795_TYPEDEFS_H
#define CENG795_TYPEDEFS_H
#include <cstdint>
#include <numbers>

typedef float real;

enum class TraceType
{
    RAY,
    PATH
};

enum class MaterialType
{
    NONE,
    NORMAL,
    MIRROR,
    CONDUCTOR,
    DIELECTRIC
};

enum class ObjectType
{
    NONE,
    TRIANGLE,
    SPHERE,
    MESH,
    PLANE,
    INSTANCE
};

enum class ShadingType
{
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
    AREA,
    DIRECTIONAL,
    TEXTURE,
    SPOT
};

enum class TextureType
{
    IMAGE,
    PERLIN,
    CHECKERBOARD
};

enum class DecalMode
{
    REPLACE_KD,
    BLEND_KD,
    REPLACE_KS,
    REPLACE_BACKGROUND,
    REPLACE_NORMAL,
    BUMP_NORMAL,
    REPLACE_ALL,
    LIGHT
};

enum class Interpolation
{
    NEAREST,
    BILINEAR,
    TRILINEAR,
};

enum class OutputType
{
    LDR,
    HDR,
    EXR
};

enum class TMOType
{
    PHOTOGRAPHIC,
    ACES,
    FILMIC
};

enum class TextureLightType
{
    PROBE,
    LATLONG
};

enum class Sampler
{
    RANDOM,
    COSINE,
    UNIFORM
};

#endif //CENG795_TYPEDEFS_H