//
// Created by vicy on 10/21/25.
//

#ifndef CENG795_RAYTRACER_H
#define CENG795_RAYTRACER_H

#include "raytracerThread.h"
#include "../fileManagement/PPM.h"
#include "../dataTypes/object/Object.h"
#include "../fileManagement/Parser.h"
#include "../functions/overloads.h"
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
    std::string output_path;
    std::ostringstream filename;
    std::chrono::high_resolution_clock::time_point start_time;
    SceneInput scene;
    BVH bvh;
    uint32_t camID;
    void drawFile(std::string input_path);
    void drawAllFiles(std::string path_to_dir);
    void parseScene(std::string input_path);
    void drawScene(uint32_t camID);
    void drawAllScenes();
    void log(std::string logText);
    ~RayTracer();
    std::string timeString(long duration);

};


#endif //CENG795_RAYTRACER_H