//
// Created by vicy on 10/21/25.
//

#include "RayTracer.h"
#include <filesystem>
#include <string>
#include <iostream>

#include "./fileManagement/PPM.h"
#include "./fileManagement/json.hpp"
using json = nlohmann::json;

long RaytracerThread::done_threads = 0;
bool BVH::print_acc_init = false;
RayTracer::RayTracer(json configs) :
    // prints
    print_init(configs["Prints"]["Initialization"]),
    print_progress(configs["Prints"]["Progress"]),
    bvh(getPivot(configs["Acceleration"]["PivotType"]),configs["Acceleration"]["MaxObjInNode"]),

    // output dir
    output_path(configs["Raytracer"]["OutputDir"]),

    // thread info
    thread_type(getThread(configs["Raytracer"]["Threads"]["Thread"].get<std::string>())),
    batch_w(configs["Raytracer"]["Threads"]["batchWidth"]),
    batch_h(configs["Raytracer"]["Threads"]["batchHeight"]),

    // logger info
    log_to_file(configs["Raytracer"]["Logger"]["LogToFile"]),
    log_to_console(configs["Raytracer"]["Logger"]["LogToConsole"]),
    logFileName(configs["Raytracer"]["Logger"]["LogDir"]),

    // acceleration info
    AccelerationStruct(getAcceleration(configs["Acceleration"]["AccelerationStruct"].get<std::string>())),



    // defaults
    maxDepth(configs["Defaults"]["DefaultMaxDepth"]),
    DefaultShadowEps(configs["Defaults"]["DefaultShadowEps"]),
    DefaultIntersEps(configs["Defaults"]["DefaultIntersEps"])
{
    scene.print_progress = print_progress;
    scene.thread_group_size = configs["Raytracer"]["Threads"]["ThreadGroupSize"];
    scene.thread_add_endl_after = configs["Raytracer"]["Threads"]["ThreadEndlAfter"];
    // std::cout << configs.dump(2) << std::endl;
    BVH::print_acc_init = configs["Prints"]["AccelerationCreation"];
    project_root = std::filesystem::current_path();
    scene.back_cull = configs["Acceleration"]["BackCulling"];
    scene.pt = getPivot(configs["Acceleration"]["PivotType"].get<std::string>());
    scene.MaxObjCount = configs["Acceleration"]["MaxObjInNode"];
    scene.filter_type = getFilter(configs["Sampling"]["FilterType"].get<std::string>());
    scene.sampling_type = getSampling(configs["Sampling"]["SamplingType"].get<std::string>());
    ACCELERATE = (AccelerationStruct != AccelerationType::NONE );
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_now);

    std::filesystem::create_directories(project_root / logFileName);
    filename << logFileName << "render_log_"
             << std::put_time(&local_tm, "%Y%m%d_%H%M%S") << ".txt";
    log("Log started.");
}

std::string RayTracer::timeString(long duration)
{
    std::string str = "";
    int h = 0;
    int m= 0;
    real s = 0.0;

    h = duration / 3600000;
    m = (duration % 3600000)/60000;
    s = (duration % 60000) / 1000.0;;


    if (h>0) str= str + std::to_string(h) + " h ";
    if (m > 0) str= str + std::to_string(m) + " m ";
    str= str + std::to_string(s) + " s ";
    return str;
}



RayTracer::~RayTracer()
{
    log("The end :)");
}


void RayTracer::log(std::string logText)
{
    if (!log_to_file && !log_to_console) return;
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_now);
    if (log_to_file)
    {
        logFile.open(filename.str(), std::ios::out | std::ios::app);
        if (logFile.is_open()) {
            logFile << "[" <<std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "] " << logText << "\n";
            logFile.close();
        }
    }
    if (log_to_console)
    {
        std::cout << "[" <<std::put_time(&local_tm, "%H:%M:%S") << "] " << logText << "\n";
    }
}

void RayTracer::parseScene(std::string input_path){
    log("----- " + input_path + " -----");
    start_time = std::chrono::high_resolution_clock::now();
    scene.Cameras.clear();
    scene.PointLights.clear();
    scene.Materials.clear();
    scene.objects.clear();
    scene.Vertices.clear();
    scene.images.clear();
    scene.textures.clear();
    scene.BackgroundTexture = nullptr;
    scene.BackgroundLight = nullptr;
    // std::cout << "Loading " << input_path << std::endl;
    Parser::parseScene(input_path, scene, maxDepth, DefaultShadowEps, DefaultIntersEps, print_init);
    // std::cout << "Scene loaded." << std::endl;
    scene.numCameras = scene.Cameras.size();
    scene.numLights = scene.PointLights.size();
    if (AccelerationStruct == AccelerationType::BVH) bvh.getScene(scene);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    log( "parsing: " +  timeString(duration));

}


void RayTracer::drawFile(std::string input_path){
    parseScene(input_path);
    drawAllScenes();
}

void RayTracer::drawAllFiles(std::string path_to_dir){
    std::string orig_out = output_path;
    for (const auto& file: std::filesystem::directory_iterator(path_to_dir))
    {
        if (file.is_regular_file() && file.path().extension() == ".json") {
            // std::cout << output_path << "\n";
            orig_out = output_path;
            std::filesystem::path rel_path = std::filesystem::relative(file.path().parent_path(), project_root);
            output_path = output_path + rel_path.string() + "/";
            std::filesystem::path dir(output_path);
            std::filesystem::create_directories(project_root / dir);
            // std::cout << output_path << "\n";
            parseScene(file.path());
            drawAllScenes();
            output_path = orig_out;
        }
    }
}


void RayTracer::drawAllScenes(){
    for(int i = 0; i < scene.numCameras; i++)
    {
        this->drawScene(i);
    }
}


void RayTracer::drawScene(uint32_t c){
    this->camID = c;
    RaytracerThread::done_threads = 0;
    Camera cam = scene.Cameras[camID];
    // log("Drawing " + cam.ImageName);
    auto start = std::chrono::high_resolution_clock::now();
    scene.u = x_product(cam.Up, -cam.Gaze);
    scene.q = cam.Position + (cam.Gaze * cam.nearDistance) + scene.u*cam.l + cam.Up*cam.t;
    scene.s_u_0 = (cam.r - cam.l) / cam.imageData->width;
    scene.s_v_0 = (cam.t - cam.b) / cam.imageData->height;
    // scene.s_v = s_v_0 * 0.5;
    // scene.viewing_ray.pos = cam.Position;

    uint32_t width = cam.imageData->width;
    uint32_t height = cam.imageData->height;

    if (thread_type == ThreadType::ROW)
    {
        std::vector<RaytracerThread> raytracers;
        for (uint32_t y = 0; y < height; y++){
            raytracers.push_back(RaytracerThread(scene,cam, bvh));
        }
        std::cout << "Start Parallelization" << std::endl;
#pragma omp parallel for
        for (uint32_t y = 0; y < height; y++){
            raytracers[y].drawRow(y);
        }
    }
    else if (thread_type == ThreadType::BATCH)
    {
        uint32_t rowcount = (height + batch_h -1) /  batch_h;
        uint32_t colcount = (width + batch_w -1) /  batch_w;
        uint32_t batchcount = rowcount * colcount;
        if (print_progress) log("There are " + std::to_string(batchcount/scene.thread_group_size )  + " sets of " +std::to_string(scene.thread_group_size) + " batches.");
        RaytracerThread rtt(scene, scene.Cameras[camID], bvh);

#pragma omp parallel for schedule(dynamic,1) firstprivate(rtt)
        for (uint32_t i = 0; i < batchcount; i++){
            RaytracerThread rtt(scene, scene.Cameras[camID], bvh);
            rtt.drawBatch((i/colcount)*batch_h*width + (i%colcount)*batch_w, batch_w, batch_h);
        }
    }
    // std::cout << std::endl;
    cam.writeToImage(output_path);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_all = duration_cast<std::chrono::milliseconds>(stop - start_time).count();

    auto duration = duration_cast<std::chrono::milliseconds>(stop - start).count();
    log("drawing: " +  timeString(duration));
    log("all: " +  timeString(duration_all));

}