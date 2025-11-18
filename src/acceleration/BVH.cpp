//
// Created by vicy on 11/09/25.
//

#include "BVH.h"

#include <algorithm>
#include <compare>

#include "../functions/overloads.h"
#include <stack>

#include "../fileManagement/Parser.h"




real BVH::getSAH(Axes& a, uint32_t start, uint32_t end, real areaC, std::deque<Object*>& objects)
{
    BBox bboxtempSmall;
    BBox bboxtempBig;
    Axes best_a = a;
    real best_pivot = objects[0]->main_center[a];
    real min_split_cost = end - start;
    real split_cost;
    Axes curr_a = a;
    real pivot = objects[0]->main_center[a];

    for (int i = start; i < end; i++)
    {
        curr_a = a;
        for (int axis = 0; axis < 3; axis++)
        {
            bboxtempSmall.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
            bboxtempSmall.vMin = Vertex(INFINITY, INFINITY, INFINITY);
            bboxtempBig.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
            bboxtempBig.vMin = Vertex(INFINITY, INFINITY, INFINITY);
            pivot = objects[i]->main_center[curr_a];

            for (int k = start; k < end; k++)
            {
                if (objects[k]->main_center[curr_a] < pivot)
                {
                    bboxtempSmall.vMax = maxVert2(objects[k]->globalBbox.vMax, bboxtempSmall.vMax);
                    bboxtempSmall.vMin = minVert2(objects[k]->globalBbox.vMin, bboxtempSmall.vMin);
                }
                else
                {
                    bboxtempBig.vMax = maxVert2(objects[k]->globalBbox.vMax, bboxtempBig.vMax);
                    bboxtempBig.vMin = minVert2(objects[k]->globalBbox.vMin, bboxtempBig.vMin);
                }
            }

            split_cost = 0.125 +  bboxtempSmall.getArea()* areaC * (i - start) + bboxtempBig.getArea() * areaC * (end - i);
            if (split_cost < min_split_cost)
            {
                min_split_cost = split_cost;
                best_a = curr_a;
                best_pivot = pivot;
            }
            curr_a = next(curr_a);
        }
    }
    a = best_a;
    return best_pivot;
}

int BVH::getSwapPos(PivotType pt, BBox bbox, Axes a, int start, int end, std::deque<Object*>& objects)
{
    if (PRINTBVH) std::cout << "BVH::getSwapPos" << std::endl;
    real pivot;
    switch (pt)
    {
    case PivotType::SAH:
        {
            real areaC = 1 / (bbox.getArea());
            pivot = getSAH(a, start, end, areaC, objects);
            break;
        }
    case PivotType::MEDIAN:
        {
            std::vector<real> centers;
            for (int i = start; i < end; i++) centers.push_back(objects[i]->main_center[a]);
            std::sort(centers.begin(), centers.end());
            pivot = centers[centers.size() / 2];
            break;
        }
    case PivotType::MIDDLE:
    default:
        pivot = (bbox.vMax[a] + bbox.vMin[a]) / 2.0;
        break;
    }

    int swap_pos = start;
    for (int runn_idx = start; runn_idx < end; runn_idx++)
    {
        // if (PRINTBVH)std::cout << objects[runn_idx]->main_center[curr_axis] << " ";
        if (pivot > objects[runn_idx]->main_center[a])
        {
            std::swap(objects[runn_idx], objects[swap_pos]);
            swap_pos++;
        }
    }
    // if (PRINTBVH)std::cout << std::endl;

    if (PRINTBVH)
        std::cout << "curr_axis: " << a << " pivot: " << pivot << " start: " << start <<
            " swap_pos: " << swap_pos << " end: " << end << std::endl;
    return swap_pos;
}

int BVH::divideToTwo(PivotType pt, BBox bbox, Axes a, int start, int end, std::deque<Object*>& objects)
{
    int swap_pos = getSwapPos(pt, bbox, a, start, end, objects);

    if ((swap_pos == end || swap_pos == start) && (end - start > MAX_OBJ_IN_NODE))
    { // bölemedik, ama maxtan da fazla çıktı
        // std::cout << "swap pos is not suitable";
        if (pt == PivotType::SAH) swap_pos = getSwapPos(PivotType::MEDIAN, bbox, a, start, end, objects);
        else
        {
            Axes curr_axis = a;
            for (int i = 0; i < 2; i++)
            {
                swap_pos = getSwapPos(pt, bbox, curr_axis, start, end, objects);

                if (swap_pos != end && swap_pos != start) break;
                curr_axis = next(curr_axis);
            }
        }
    }

    if ((swap_pos == end || swap_pos == start) && (end - start > MAX_OBJ_IN_NODE))
    { // arkadaş, hala olmadı
        swap_pos = start + MAX_OBJ_IN_NODE;
    }

    return swap_pos;
}




int BVH::partition(int start, int end, Axes a, std::deque<Object*>& objects)
{
    int curr_idx = nodes.size();

    nodes.push_back(BVHNode());

    if (PRINTBVH)std::cout << "-------------------" << std::endl;
    if (PRINTBVH)std::cout << "start: " << start << " end: " << end << std::endl;

    nodes[curr_idx].bbox.vMax = Vertex(-INFINITY, -INFINITY, -INFINITY);
    nodes[curr_idx].bbox.vMin = Vertex(INFINITY, INFINITY, INFINITY);

    for (int i = start; i < end; i++)
    {
        nodes[curr_idx].bbox.vMax = maxVert2(objects[i]->globalBbox.vMax, nodes[curr_idx].bbox.vMax);
        nodes[curr_idx].bbox.vMin = minVert2(objects[i]->globalBbox.vMin, nodes[curr_idx].bbox.vMin);
    }

    int swap_pos = start;
    Axes curr_axis = a;
    if (end - start > 1) swap_pos = divideToTwo(pivotType, nodes[curr_idx].bbox, curr_axis, start, end, objects);
    // std::cout << "divided" << std::endl;
    if (swap_pos == end || swap_pos == start)
    {
        nodes[curr_idx].type = BVHNodeType::LEAF;
        nodes[curr_idx].firstObjID = start;
        nodes[curr_idx].objCount = end - start;
    }
    else
    {
        nodes[curr_idx].objCount = 0;
        nodes[curr_idx].type = BVHNodeType::INT_W_BOTH;
        partition(start, swap_pos, next(curr_axis), objects);
        nodes[curr_idx].rightOffset = partition(swap_pos, end, next(curr_axis), objects);
    }

    // std::cout << "--------------------------\n";
    // std::cout << curr_idx << " " <<nodes[curr_idx] << std::endl;
    return curr_idx;
}


void BVH::getScene(SceneInput& scene)
{
    if (PRINTBVH)std::cout << "getScene" << std::endl;
    // std::cout << scene.numObjects << " " << scene.objects.size()<< std::endl;
    nodes.clear();


    partition(0, scene.numObjects, Axes::x, scene.objects);


    for (int i = 0; i < scene.numObjects; i++)
    {
        scene.objects[i]->_id = i;
    }

    if (PRINTBVH)
        std::cout << *this;

    if (PRINTBVH) std::cout << "gotScene" << std::endl;
}
