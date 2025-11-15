//
// Created by vicy on 11/08/25.
//

#ifndef CENG795_TYPEDEFS_H
#define CENG795_TYPEDEFS_H
#include <cstdint>

typedef float real;

#define THREAD_PROGRESS 50
#define THREAD_ENDL 2000

#define ROW_THREAD false
#define BATCH_THREAD true

#define ACCELERATE true
#define MAX_OBJ_IN_NODE 1

#define PRINTINIT false
#define PRINTBVH false
#define DEFAULT_MAX_DEPTH 20
#define DEFAULT_SHADOW_EPS 1e-3
#define DEFAULT_INTERS_EPS 1e-6
static constexpr uint32_t batch_w = 16;
static constexpr uint32_t batch_h = batch_w;


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



#endif //CENG795_TYPEDEFS_H