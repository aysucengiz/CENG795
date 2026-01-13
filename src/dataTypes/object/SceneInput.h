//
// Created by vicy on 01/03/26.
//

#ifndef CENG795_SCENEINPUT_H
#define CENG795_SCENEINPUT_H


#include "../base/Camera.h"
#include "../base/Light.h"

struct SceneInput
{
    TraceType trace_type;
    bool NEE = false;
    bool importance_sampling = false;
    bool MIS_BALANCE = false;
    bool russian_roulette = false;

    bool print_progress;
    bool back_cull;
    PivotType pt;
    uint32_t MaxObjCount;
    // info about image
    Color BackgroundColor;
    double ShadowRayEpsilon;
    double IntersectionTestEpsilon;
    SamplingType sampling_type;
    FilterType filter_type;

    uint32_t thread_group_size;
    uint32_t thread_add_endl_after;

    // materials and cam  info
    std::vector<Material> Materials;
    std::vector<Camera> Cameras;

    // vertex info
    std::deque<CVertex> Vertices;
    std::deque<std::pair<int,int>> TexCoords;

    // light info
    Color AmbientLight;
    std::vector<Light*> PointLights;

    // object info
    std::deque<Object*> objects;

    // num info
    uint32_t numCameras;
    uint32_t numObjects;
    uint32_t numPlanes;
    uint32_t numLights;

    // transformations
    std::vector<std::shared_ptr<Transformation>> transforms;
    std::vector<Texture*> textures;
    Texture *BackgroundTexture;
    TextureLight *BackgroundLight = nullptr;
    std::vector<Image*> images;

    // precomputed near plane info
    Vec3r u;
    Vertex q;
    real s_u_0;
    real s_v_0;
};



#endif //CENG795_SCENEINPUT_H