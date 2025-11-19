//
// Created by vicy on 10/21/25.
//

#include "RayTracer.h"
#include <filesystem>
#include <string>
#include <iostream>

long RaytracerThread::done_threads = 0;



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

RayTracer::RayTracer()
{
    project_root = std::filesystem::current_path();
    output_path = "outputs/";
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_now);

    filename << "logs/render_log_"
             << std::put_time(&local_tm, "%Y%m%d_%H%M%S") << ".txt";
    bvh.pivotType = PivotType::PIVOT_TYPE;

    log("Log started.");
}

RayTracer::~RayTracer()
{
    log("The end :)");
}


void RayTracer::log(std::string logText)
{
    if (!LOG_ON) return;
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_now);

    logFile.open(filename.str(), std::ios::out | std::ios::app);
    if (logFile.is_open()) {
        logFile << "[" <<std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "] " << logText << "\n";
        logFile.close();
    }
    std::cout << "[" <<std::put_time(&local_tm, "%Y-%m-%d %H:%M:%S") << "] " << logText << "\n";
}

void RayTracer::parseScene(std::string input_path){
    //log("----- Parsing scene from " + input_path + " -----");
    start_time = std::chrono::high_resolution_clock::now();
    scene.Cameras.clear();
    scene.PointLights.clear();
    scene.Materials.clear();
    //for (int i=0; i< scene.objects.size(); i++){if (scene.objects[i]) delete scene.objects[i];}
    scene.objects.clear();
    scene.Vertices.clear();
    Parser::parseScene(input_path, scene);
    scene.numCameras = scene.Cameras.size();
    scene.numLights = scene.PointLights.size();
    if (ACCELERATE) bvh.getScene(scene);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    log( input_path + ": " +  timeString(duration));

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
            std::cout << output_path << "\n";
            orig_out = output_path;
            std::filesystem::path rel_path = std::filesystem::relative(file.path().parent_path(), project_root);
            output_path = output_path + rel_path.string() + "/";
            std::filesystem::path dir(output_path);
            std::filesystem::create_directories(project_root / dir);
            std::cout << output_path << "\n";
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
    //log("----- Drawing " + cam.ImageName + " -----");
    auto start = std::chrono::high_resolution_clock::now();
    scene.u = x_product(cam.Up, -cam.Gaze);
    scene.q = cam.Position + (cam.Gaze * cam.nearDistance) + scene.u*cam.l + cam.Up*cam.t;
    scene.s_u_0 = (cam.r - cam.l) / cam.width;
    scene.s_v_0 = (cam.t - cam.b) / cam.height;
    // scene.s_v = s_v_0 * 0.5;
    // scene.viewing_ray.pos = cam.Position;

    uint32_t width = cam.width;
    uint32_t height = cam.height;

    scene.image = new unsigned char[width * height * 3];

    if (ROW_THREAD)
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
    else if (BATCH_THREAD)
    {
        uint32_t rowcount = (height + batch_h -1) /  batch_h;
        uint32_t colcount = (width + batch_w -1) /  batch_w;
        uint32_t batchcount = rowcount * colcount;
        // log("There are " + std::to_string(batchcount/ THREAD_PROGRESS)  + " sets of " +std::to_string(THREAD_PROGRESS) + " batches.");
        RaytracerThread rtt(scene, scene.Cameras[camID], bvh);

#pragma omp parallel for schedule(dynamic,1) firstprivate(rtt)
        for (uint32_t i = 0; i < batchcount; i++){
            RaytracerThread rtt(scene, scene.Cameras[camID], bvh);
            rtt.drawBatch((i/colcount)*batch_h*width + (i%colcount)*batch_w, batch_w, batch_h);
        }
    }
    // std::cout << std::endl;


    PPM::write_stb((output_path  + cam.ImageName).c_str(), scene.image, width, height);
    delete[] scene.image;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_all = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    log(cam.ImageName + " all: " +  timeString(duration_all));

    auto duration = duration_cast<std::chrono::milliseconds>(stop - start).count();
    log(cam.ImageName + " drawing: " +  timeString(duration));

}