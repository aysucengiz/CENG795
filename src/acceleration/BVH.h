//
// Created by vicy on 11/09/25.
//

#ifndef CENG795_BVH_H
#define CENG795_BVH_H

#include <cstdint>
#include <vector>

#include "../dataTypes/object/BBox.h"
#include "../dataTypes/base/typedefs.h"
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
    std::vector<BVHNode> nodes;
    PivotType pivotType;
    real getPivot(BBox bbox, Axes a, int start, int end, std::vector<Object *> &objects);
    int partition(int start, int end, Axes a, std::vector<Object *> &objects);
    void getScene(SceneInput &scene);
    Object *traverse(Ray &ray, real &t_min, const std::vector<Object *> &objects, bool shadow_test = false ) const;

};


#endif //CENG795_BVH_H