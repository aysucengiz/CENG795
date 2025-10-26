//
// Created by vicy on 10/21/25.
//

#ifndef CENG795_RAYTRACER_H
#define CENG795_RAYTRACER_H

#include "raytracerThread.h"
#include "../fileManagement/PPM.h"
#include "../dataTypes/Object.h"
#include "../fileManagement/Parser.h"
#include <chrono>

class RayTracer
{
public:
    SceneInput scene;
    void parseScene(std::string input_path);
    void drawScene(uint32_t camID);
    void drawAllScenes();

};


#endif //CENG795_RAYTRACER_H