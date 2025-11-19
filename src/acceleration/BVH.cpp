//
// Created by vicy on 11/09/25.
//

#include "BVH.h"

#include <algorithm>
#include <compare>

#include "../functions/overloads.h"
#include <stack>

#include "../fileManagement/Parser.h"



template<typename T>
real BVH::getSAH(Axes& a, uint32_t start, uint32_t end, real areaC, T& objects)
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

template<typename T>
int BVH::getSwapPos(PivotType pt, BBox bbox, Axes a, int start, int end, T& objects)
{
    if (PRINTBVH) std::cout << "BVH::getPivot" << std::endl;
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

template<typename T>
int BVH::divideToTwo(PivotType pt, BBox bbox, Axes a, int start, int end, T &objects)
{
    int swap_pos = getSwapPos(pt, bbox, a, start, end, objects);

    if ((swap_pos == end || swap_pos == start) && (end - start > MAX_OBJ_IN_NODE))
    { // bölemedik, ama maxtan da fazla çıktı
        if (pt == PivotType::SAH) swap_pos = getSwapPos(PivotType::MEDIAN, bbox, a, swap_pos, end, objects);
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




template<typename T>
int BVH::partition(int start, int end, Axes a, T &objects)
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

void BVH::getScene(std::vector<Triangle *> &triangles)
{
    if (PRINTBVH)std::cout << "getScene" << std::endl;
    // std::cout << scene.numObjects << " " << scene.objects.size()<< std::endl;
    nodes.clear();


    partition(0, triangles.size(), Axes::x, triangles);


    for (int i = 0; i < triangles.size(); i++)
    {
        triangles[i]->_id = i;
    }

    if (PRINTBVH)
        std::cout << *this;

    if (PRINTBVH) std::cout << "gotScene" << std::endl;
}


Object::intersectResult BVH::traverse(const Ray &ray,const  real &t_min,const std::deque<Object*> &objects, bool shadow_test , bool back_cull) const
{
    //std::cout << "BVH::traverse" << std::endl;
    Object::intersectResult result;
    result.t_min = t_min;
    result.obj = nullptr;
    std::vector<int> traverseIDs;
    traverseIDs.reserve(64);
    traverseIDs.push_back(0);

    while (traverseIDs.size() > 0)
    {
        //std::cout << traverseIDs.top() << std::endl;
        int id = traverseIDs.back();
        traverseIDs.pop_back();
        //std::cout << id << std::endl;
        BVHNode const &node = nodes[id];
        if (node.bbox.intersects(ray))
        {
            if (node.type == BVHNodeType::LEAF)
            {
                Object::intersectResult temp;
                temp.t_min = result.t_min;
                temp.obj = result.obj;
                int finID = node.firstObjID + node.objCount;
                for (int i=node.firstObjID; i< finID; i++)
                {
                    temp = objects[i]->checkIntersection(ray, temp.t_min, shadow_test, back_cull);
                    if (shadow_test && temp.obj != nullptr) return temp;
                    if (temp.obj != nullptr)
                    {
                        result.obj = temp.obj;
                        result.currTri = temp.currTri;
                        result.t_min = temp.t_min;
                    }
                }
            }
            else
            {
                if (node.type == BVHNodeType::INT_W_BOTH ||
                    node.type == BVHNodeType::INT_W_RIGHT)
                    traverseIDs.push_back(node.rightOffset);
                if (node.type == BVHNodeType::INT_W_BOTH ||
                    node.type == BVHNodeType::INT_W_LEFT)
                    traverseIDs.push_back(id + 1);
            }
        }
    }


    return result;
}


Object::intersectResult BVH::traverse(const Ray &ray,const  real &t_min,const std::vector<Triangle*> &objects, bool shadow_test , bool back_cull) const
{
    //std::cout << "BVH::traverse" << std::endl;
    Object::intersectResult result;
    result.t_min = t_min;
    result.obj = nullptr;
    std::vector<int> traverseIDs;
    traverseIDs.reserve(64);
    traverseIDs.push_back(0);

    while (traverseIDs.size() > 0)
    {
        //std::cout << traverseIDs.top() << std::endl;
        int id = traverseIDs.back();
        traverseIDs.pop_back();
        //std::cout << id << std::endl;
        BVHNode const &node = nodes[id];
        if (node.bbox.intersects(ray))
        {
            if (node.type == BVHNodeType::LEAF)
            {
                Object::intersectResult temp;
                temp.t_min = result.t_min;
                temp.obj = result.obj;
                int finID = node.firstObjID + node.objCount;
                for (int i=node.firstObjID; i< finID; i++)
                {
                    temp = objects[i]->checkIntersection(ray, temp.t_min, shadow_test, back_cull);
                    if (shadow_test && temp.obj != nullptr) return temp;
                    if (temp.obj != nullptr)
                    {
                        result.obj = temp.obj;
                        result.currTri = temp.currTri;
                        result.t_min = temp.t_min;
                    }
                }
            }
            else
            {
                if (node.type == BVHNodeType::INT_W_BOTH ||
                    node.type == BVHNodeType::INT_W_RIGHT)
                    traverseIDs.push_back(node.rightOffset);
                if (node.type == BVHNodeType::INT_W_BOTH ||
                    node.type == BVHNodeType::INT_W_LEFT)
                    traverseIDs.push_back(id + 1);
            }
        }
    }


    return result;
}