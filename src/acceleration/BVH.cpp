//
// Created by vicy on 11/09/25.
//

#include "BVH.h"

#include <algorithm>
#include <compare>

#include "../functions/overloads.h"
#include <stack>

#include "../fileManagement/Parser.h"



template<template<typename, typename...> class Container, typename T>
real BVH::getSAH(Axes& a, uint32_t start, uint32_t end, real areaC, Container<T>& objects, BBox &bbox)
{
    Axes best_a = a;
    real best_pivot = objects[0]->main_center[a];
    real min_split_cost = end - start;
    real split_cost;
    Axes curr_a;
    real pivot;
    std::vector<BBox> bboxtempSmall(end - start);
    std::vector<BBox> bboxtempBig(end - start);
    std::vector<T> objects_copied(end-start);
    // std::cout << "1" << std::endl;
    std::copy(objects.begin()+start, objects.begin()+end, objects_copied.begin());
    // std::cout << "2" << std::endl;



    curr_a = a;
    for (int axis = 0; axis < 3; axis++)
    {
        std::sort(objects_copied.begin(), objects_copied.end() ,
                            [&curr_a](T& lhs, T& rhs) { return lhs->main_center[curr_a] < rhs->main_center[curr_a]; });


        int size = end - start;
        bboxtempSmall[0] = objects_copied[0]->globalBbox;
        bboxtempBig[size - 1] = objects_copied[end-start-1]->globalBbox;
        for (int i = 1; i < size; ++i) {
            bboxtempSmall[i].vMin = minVert2(bboxtempSmall[i-1].vMin, objects_copied[i]->globalBbox.vMin);
            bboxtempSmall[i].vMax = maxVert2(bboxtempSmall[i-1].vMax, objects_copied[i]->globalBbox.vMax);
            bboxtempBig[size-i-1].vMin = minVert2(bboxtempBig[size-i].vMin, objects_copied[end-start -i-1]->globalBbox.vMin);
            bboxtempBig[size-i-1].vMax = maxVert2(bboxtempBig[size-i].vMax, objects_copied[end-start -i-1]->globalBbox.vMax);
        }


        for (int i = 0; i < end-start; i++)
        {
            pivot = objects_copied[i]->main_center[curr_a];

            split_cost = 0.125 +  bboxtempSmall[i].getArea()* areaC * (i ) + bboxtempBig[i].getArea() * areaC * (end -start - i);
            if (split_cost < min_split_cost)
            {
                min_split_cost = split_cost;
                best_a = curr_a;
                best_pivot = pivot;
            }
        }

       curr_a = next(curr_a);
    }
    a = best_a;
    return best_pivot;
}

template<typename T>
int BVH::getSwapPos(PivotType pt, BBox bbox, Axes a, int start, int end, T& objects)
{

    int swap_pos = start;
    if (print_acc_init) std::cout << "BVH::getPivot" << std::endl;
    real pivot;
    switch (pt)
    {
    case PivotType::SAH:
        {
            real areaC = 1 / (bbox.getArea());
            pivot = getSAH(a, start, end, areaC, objects,bbox);
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

    // if (PivotType::SAH != pt)
    {
        for (int runn_idx = start; runn_idx < end; runn_idx++)
        {
            // if (print_acc_init)std::cout << objects[runn_idx]->main_center[curr_axis] << " ";
            if (pivot > objects[runn_idx]->main_center[a])
            {
                std::swap(objects[runn_idx], objects[swap_pos]);
                swap_pos++;
            }
        }
    }

    // if (print_acc_init)std::cout << std::endl;

    if (print_acc_init)
        std::cout << "curr_axis: " << a << " pivot: " << pivot << " start: " << start <<
            " swap_pos: " << swap_pos << " end: " << end << std::endl;
    return swap_pos;
}

template<typename T>
int BVH::divideToTwo(PivotType pt, BBox bbox, Axes a, int start, int end, T &objects)
{
    int swap_pos = getSwapPos(pt, bbox, a, start, end, objects);

    if ((swap_pos == end || swap_pos == start) && (end - start > MaxObjInNode))
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

    if ((swap_pos == end || swap_pos == start) && (end - start > MaxObjInNode))
    { // arkadaş, hala olmadı
            swap_pos = start + MaxObjInNode;
    }

    return swap_pos;
}




template<typename T>
int BVH::partition(int start, int end, Axes a, T &objects)
{
    print_acc_init= false;
    // std::cout << (int) pivotType << std::endl;
    // std::cout << MaxObjInNode << std::endl;
    int curr_idx = nodes.size();

    nodes.push_back(BVHNode());

    if (print_acc_init)std::cout << "-------------------" << std::endl;
    if (print_acc_init)std::cout << "start: " << start << " end: " << end << " size: " << objects.size() << std::endl;

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
    if (print_acc_init)std::cout << "getScene" << std::endl;
    // std::cout << scene.numObjects << " " << scene.objects.size()<< std::endl;
    nodes.clear();


    partition(0, scene.numObjects, Axes::x, scene.objects);


    for (int i = 0; i < scene.numObjects; i++)
    {
        scene.objects[i]->_id = i;
    }

    if (print_acc_init)
        std::cout << *this;

    if (print_acc_init) std::cout << "gotScene" << std::endl;
}

void  BVH::getScene(std::vector<Triangle*> &triangles)
{
    if (print_acc_init)std::cout << "getScene" << std::endl;
    // std::cout <<" aaaa" << std::endl;
    nodes.clear();

    if (print_acc_init)std::cout << "Will partition" << std::endl;
    partition(0, triangles.size(), Axes::x, triangles);
    if (print_acc_init)std::cout <<" Partition done" << std::endl;


    // for (int i = 0; i < triangles.size(); i++)
    // {
    //     triangles[i]->_id = i;
    // }

    // std::cout << nodes[0] << std::endl;
    // std::cout << print_acc_init << std::endl;
    // std::cout << MaxObjInNode << std::endl;
    // std::cout << pivotType << std::endl;
    // std::cout << bboxA << std::endl;

    if (print_acc_init) std::cout << *this;
}

template<typename Container>
Object::intersectResult BVH::traverse(const Ray &ray,const  real &t_min, const Container &objects, bool shadow_test , bool back_cull, double time) const
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
                    temp = objects[i]->checkIntersection(ray, temp.t_min, shadow_test, back_cull, time);
                    if (shadow_test && temp.obj != nullptr) return temp;
                    if (temp.obj != nullptr)
                    {
                        result.obj = temp.obj;
                        if constexpr (std::is_same_v<Container, std::vector<Triangle*>>) result.currTri = i;
                        else result.currTri = temp.currTri;
                        result.t_min = temp.t_min;
                        // std::cout << result.currTri << std::endl;
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

template Object::intersectResult BVH::traverse<std::vector<Triangle*>>(
    const Ray&, const real&, const std::vector<Triangle*>&, bool, bool, double) const;

template Object::intersectResult BVH::traverse<std::deque<Object*>>(
    const Ray&, const real&, const std::deque<Object*>&, bool, bool, double) const;