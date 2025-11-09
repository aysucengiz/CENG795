//
// Created by vicy on 10/21/25.
//

#ifndef CENG795_RAYTRACER_H
#define CENG795_RAYTRACER_H

#include "raytracerThread.h"
#include "../fileManagement/PPM.h"
#include "../dataTypes/object/Object.h"
#include "../fileManagement/Parser.h"
#include "../dataTypes/functions/overloads.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ctime>


class RayTracer
{
public:
    RayTracer();
    std::ofstream logFile;
    std::string input_path;
    std::ostringstream filename;
    std::chrono::high_resolution_clock::time_point start_time;
    SceneInput scene;
    BVH bvh;
    void parseScene(std::string input_path);
    void drawScene(uint32_t camID);
    void drawAllScenes();
    void log(std::string logText);
    ~RayTracer();

};


#endif //CENG795_RAYTRACER_H