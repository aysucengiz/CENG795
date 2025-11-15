//
// Created by vicy on 11/09/25.
//

#include "BVH.h"

#include <algorithm>
#include <compare>

#include "../functions/overloads.h"
#include <stack>

#include "../fileManagement/Parser.h"


real BVH::getPivot(BBox bbox, Axes a, int start, int end, std::deque<Object *> &objects)
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

int BVH::partition(int start, int end, Axes a, std::deque<Object *> &objects)
{
    int curr_idx = nodes.size();

    nodes.push_back(BVHNode());

    if (PRINTBVH)std::cout << "-------------------" << std::endl;
    if (PRINTBVH)std::cout << "start: " << start<< " end: "<< end << std::endl;

    nodes[curr_idx].bbox.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
    nodes[curr_idx].bbox.vMin = Vertex(INFINITY, INFINITY, INFINITY);

    for (int i=start; i< end; i++)
    {
        nodes[curr_idx].bbox.vMax = maxVert2(objects[i]->globalBbox.vMax, nodes[curr_idx].bbox.vMax);
        nodes[curr_idx].bbox.vMin = minVert2(objects[i]->globalBbox.vMin, nodes[curr_idx].bbox.vMin);
    }
    // std::cout << nodes[curr_idx].bbox.vMax<< std::endl;
    // std::cout << nodes[curr_idx].bbox.vMin<< std::endl;

    if (end - start <= MAX_OBJ_IN_NODE)
    {
        nodes[curr_idx].type = BVHNodeType::LEAF;
        nodes[curr_idx].firstObjID = start;
        nodes[curr_idx].objCount = end - start;
    }
    else
    {
        nodes[curr_idx].objCount = 0;

        int swap_pos = start;
        Axes curr_axis = a;
        for (int i=0; i<3; i++)
        {
            real pivot = getPivot(nodes[curr_idx].bbox,curr_axis,start,end,objects);
            swap_pos = start;
            for (int runn_idx = start; runn_idx< end; runn_idx++)
            {
                // if (PRINTBVH)std::cout << objects[runn_idx]->main_center[curr_axis] << " ";
                if (pivot >= objects[runn_idx]->main_center[curr_axis])
                {
                    std::swap(objects[runn_idx], objects[swap_pos]);
                    swap_pos++;
                }
            }
            // if (PRINTBVH)std::cout << std::endl;


            if (PRINTBVH)std::cout << "curr_axis: "<<curr_axis << " pivot: " << pivot << " start: " <<start << " swap_pos: " <<  swap_pos<< " end: " << end <<std::endl;
            if (swap_pos != end && swap_pos != start) break;
            curr_axis = next(curr_axis);
        }

        if      ((swap_pos == end || swap_pos == start) && (end - start != 2))
        {
            nodes[curr_idx].type = BVHNodeType::LEAF;
            nodes[curr_idx].firstObjID = start;
            nodes[curr_idx].objCount = end - start;
            //if (PRINTBVH)
            {
                for (int runn_idx = start; runn_idx< end; runn_idx++)
                {
                    std::cout << objects[runn_idx]->getObjectType() <<": "<< objects[runn_idx]->main_center << " | ";
                }
                std::cout << std::endl;
            }
        }
        else
        {
            if (end - start == 2) swap_pos = start + 1;

            nodes[curr_idx].type = BVHNodeType::INT_W_BOTH;

            if (nodes[curr_idx].type == BVHNodeType::INT_W_LEFT ||
                nodes[curr_idx].type == BVHNodeType::INT_W_BOTH)
            {
                partition(start,swap_pos,next(curr_axis),objects);
            }

            if (nodes[curr_idx].type == BVHNodeType::INT_W_RIGHT ||
                nodes[curr_idx].type == BVHNodeType::INT_W_BOTH)
            {
                nodes[curr_idx].rightOffset = partition(swap_pos,end,next(curr_axis),objects);
            }
        }

    }


    // std::cout << "--------------------------\n";
    // std::cout << curr_idx << " " <<nodes[curr_idx] << std::endl;
    return curr_idx;
}


void BVH::getScene(SceneInput &scene)
{
if (PRINTBVH)std::cout << "getScene" << std::endl;
// std::cout << scene.numObjects << " " << scene.objects.size()<< std::endl;
    nodes.clear();
    partition(0, scene.numObjects, Axes::x, scene.objects);


    for (int i=0; i< scene.numObjects; i++)
    {
        scene.objects[i]->_id = i;
    }

    //if (PRINTBVH)
        std::cout << *this;

    if (PRINTBVH) std::cout << "gotScene" << std::endl;
}

Object *BVH::traverse(Ray &ray, real &t_min, const std::deque<Object *> &objects, bool shadow_test ) const
{
    //std::cout << "BVH::traverse" << std::endl;
    std::stack<int> traverseIDs;
    traverseIDs.push(0);
    Object *return_obj = nullptr;

    while (traverseIDs.size() > 0)
    {
        //std::cout << traverseIDs.top() << std::endl;
        int id = traverseIDs.top();
        //std::cout << id << std::endl;
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