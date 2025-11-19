//
// Created by vicy on 11/09/25.
//

#ifndef CENG795_BVH_H
#define CENG795_BVH_H

#include <cstdint>
#include <vector>

#include "../dataTypes/object/BBox.h"
#include "../typedefs.h"
#include "../dataTypes/object/Object.h"

struct alignas(32) BVHNode
{
    BBox bbox; // 24 byte
    BVHNodeType type; // 1 byte
    union// 4 byte
    {
        uint32_t firstObjID;
        uint32_t rightOffset;
    };
    uint16_t objCount; // 2 byte


    //BVHNode(BVHnodeType t, uint32_t unionID, uint16_t objCount=0);
};


class BVH
{
public:
    BBox bboxA;
    BBox bboxB;
    std::vector<BVHNode> nodes;
    PivotType pivotType;

    template<typename T>
    int divideToTwo(PivotType pt, BBox bbox, Axes a, int start, int end, T &objects);
    template<typename T>
    int partition(int start, int end, Axes a, T &objects);

    void getScene(SceneInput &scene);
    void getScene(std::vector<Triangle *> &triangles);

    template<typename T>
    real getSAH(Axes &a, uint32_t start, uint32_t end, real areaC,T &objects);
    template<typename T>
    int getSwapPos(PivotType pt, BBox bbox, Axes a, int start, int end, T& objects);

    Object::intersectResult traverse(const Ray &ray,const  real &t_min, const std::deque<Object *> &objects, bool shadow_test , bool back_cull) const;
    Object::intersectResult traverse(const Ray &ray,const  real &t_min, const std::vector<Triangle *> &objects, bool shadow_test , bool back_cull) const;

};


#endif //CENG795_BVH_H