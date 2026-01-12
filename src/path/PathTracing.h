//
// Created by vicy on 01/12/26.
//

#ifndef CENG795_PATHTRACING_H
#define CENG795_PATHTRACING_H
#include <cstdint>
#include "../typedefs.h"

struct BRDF
{
    // original blinn phong by default
    uint32_t _id;
    real exponent;
};


struct Phong : public BRDF
{
    bool blinn;
};

struct ModifiedPhong : public Phong
{
    bool normalized;
};

struct BRDF_TorranceSparrow : public BRDF
{
    bool kd_fresnel;
};

class PathTracing
{
};


#endif //CENG795_PATHTRACING_H