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
    int divideToTwo(PivotType pt, BBox bbox, Axes a, int start, int end, std::deque<Object *> &objects);
    int partition(int start, int end, Axes a, std::deque<Object *> &objects);
    void getScene(SceneInput &scene);
    real getSAH(Axes &a, uint32_t start, uint32_t end, real areaC, std::deque<Object *> &objects);
    int getSwapPos(PivotType pt, BBox bbox, Axes a, int start, int end, std::deque<Object*>& objects);

};


#endif //CENG795_BVH_H