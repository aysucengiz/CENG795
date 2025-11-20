//
// Created by vicy on 10/21/25.
//

#ifndef CENG795_RAYTRACER_H
#define CENG795_RAYTRACER_H

#include "raytracerThread.h"
#include "typedefs.h"
#include "../dataTypes/object/Object.h"
#include "../dataTypes/object/Mesh.h"
#include "../fileManagement/Parser.h"
#include "../functions/overloads.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <ctime>


class RayTracer
{
public:
    SceneInput scene;

    // prints
    const bool print_init;
    const bool print_progress;

    // output dir
    std::string output_path;

    // thread info
    const ThreadType thread_type;
    const uint16_t batch_w;
    const uint16_t batch_h;
    const uint16_t thread_group_size;
    const uint32_t thread_add_endl_after;

    // logger info
    const bool log_it;
    std::ofstream logFile;
    const std::string logFileName;

    // acceleration info
    const AccelerationType AccelerationStruct;
    BVH bvh;

    // defaults
    const uint32_t maxDepth;
    const real DefaultShadowEps;
    const real DefaultIntersEps;

    RayTracer(json configs);
    std::filesystem::path project_root;
    std::string input_path;
    std::ostringstream filename;
    std::chrono::high_resolution_clock::time_point start_time;
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