//
// Created by vicy on 11/09/25.
//

#include "BVH.h"

#include <algorithm>
#include <compare>

#include "../dataTypes/functions/overloads.h"

#include <stack>

#include "../fileManagement/Parser.h"

/*
BVHNode::BVHNode(BVHnodeType t, uint32_t unionID, uint16_t objCount) : type(t),objCount(objCount)
{
    if (t == BVHnodeType::LEAF) firstObjID = unionID;
    else rightOffset = unionID;
}*/

real BVH::getPivot(BBox bbox, Axes a, int start, int end, std::vector<Object *> &objects)
{
    switch (pivotType)
    {
    case PivotType::MEDIAN:
    {
        std::vector<real> centers;
        for (int i=start; i<end; i++) centers.push_back(objects[i]->main_center[a]);
        std::sort(centers.begin(), centers.end());
        return centers[centers.size() / 2];
    }
    case PivotType::MIDDLE:
    default:
        return (bbox.vMax[a] + bbox.vMin[a]) / 2.0;
    }

}

int BVH::partition(int start, int end, Axes a, std::vector<Object *> &objects)
{
    int curr_idx = nodes.size();
    nodes.push_back(BVHNode());
    nodes[curr_idx].bbox.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
    nodes[curr_idx].bbox.vMin = Vertex(INFINITY, INFINITY, INFINITY);

    for (int i=start; i< end; i++)
    {
        nodes[curr_idx].bbox.vMax = maxVert2(objects[i]->globalBbox.vMax, nodes[curr_idx].bbox.vMax);
        nodes[curr_idx].bbox.vMin = minVert2(objects[i]->globalBbox.vMin, nodes[curr_idx].bbox.vMin);
    }

    if (end - start <= MAX_OBJ_IN_NODE)
    {
        nodes[curr_idx].type = BVHNodeType::LEAF;
        nodes[curr_idx].firstObjID = start;
        nodes[curr_idx].objCount = end - start;
    }
    else
    {
        nodes[curr_idx].objCount = 0;



        real pivot = getPivot(nodes[curr_idx].bbox,a,start,end,objects);
        int swap_pos = start;

        for (int runn_idx = start; runn_idx< end; runn_idx++)
        {
            if (pivot > objects[runn_idx]->main_center[a])
            {
                std::swap(objects[runn_idx], objects[swap_pos]);
                swap_pos++;
            }
        }
        if      (swap_pos == end)    nodes[curr_idx].type = BVHNodeType::INT_W_LEFT;
        else if (swap_pos == start)  nodes[curr_idx].type = BVHNodeType::INT_W_RIGHT;
        else                         nodes[curr_idx].type = BVHNodeType::INT_W_BOTH;


        if (nodes[curr_idx].type == BVHNodeType::INT_W_LEFT ||
            nodes[curr_idx].type == BVHNodeType::INT_W_BOTH)
        {
            partition(start,swap_pos,next(a),objects);
        }

        if (nodes[curr_idx].type == BVHNodeType::INT_W_RIGHT ||
            nodes[curr_idx].type == BVHNodeType::INT_W_BOTH)
        {
            nodes[curr_idx].rightOffset = partition(swap_pos,end,next(a),objects);
        }
    }


    // std::cout << "--------------------------\n";
    // std::cout << curr_idx << " " <<nodes[curr_idx] << std::endl;
    return curr_idx;
}


void BVH::getScene(SceneInput &scene)
{
    partition(0, scene.numObjects, Axes::x, scene.objects);
    nodes.clear();

    for (int i=0; i< scene.numObjects; i++)
    {
        scene.objects[i]->_id = i;
    }

    if (PRINTINIT) std::cout << *this;
}

Object *BVH::traverse(Ray &ray, real &t_min, const std::vector<Object *> &objects, bool shadow_test ) const
{
    //std::cout << "BVH::traverse" << std::endl;
    std::stack<int> traverseIDs;
    traverseIDs.push(0);
    Object *return_obj = nullptr;

    while (traverseIDs.size() > 0)
    {
        int id = traverseIDs.top();
        BVHNode const &node = nodes[id];
        traverseIDs.pop();
        if (node.bbox.intersects(ray))
        {
            if (node.type == BVHNodeType::LEAF)
            {
                Object *temp_obj = nullptr;
                int finID = node.firstObjID + node.objCount;
                for (int i=node.firstObjID; i< finID; i++)
                {
                    temp_obj = objects[i]->checkIntersection(ray, t_min,  shadow_test);
                    if (shadow_test && temp_obj != nullptr) return temp_obj;
                    if (temp_obj != nullptr) return_obj = temp_obj;
                }
            }
            else
            {
                if (node.type == BVHNodeType::INT_W_BOTH ||
                    node.type == BVHNodeType::INT_W_LEFT)
                    traverseIDs.push(id + 1);
                if (node.type == BVHNodeType::INT_W_BOTH ||
                    node.type == BVHNodeType::INT_W_RIGHT)
                    traverseIDs.push(node.rightOffset);
            }
        }
    }


    return return_obj;
}