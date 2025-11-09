//
// Created by vicy on 10/21/25.
//

#include "RayTracer.h"


int RaytracerThread::done_threads = 0;

RayTracer::RayTracer()
{
    auto now = std::chrono::system_clock::now();
    std::time_t time_now = std::chrono::system_clock::to_time_t(now);
    std::tm local_tm = *std::localtime(&time_now);

    filename << "logs/render_log_"
             << std::put_time(&local_tm, "%Y%m%d_%H%M%S") << ".txt";


    log("Log started.");
}

RayTracer::~RayTracer()
{
    log("The end :)");
}


void RayTracer::log(std::string logText)
{
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
    scene.objects.clear();
    scene.Vertices.clear();
    Parser::parseScene(input_path, scene);
    scene.numCameras = scene.Cameras.size();
    scene.numObjects = scene.objects.size();
    scene.numLights = scene.PointLights.size();
    if (ACCELERATE) bvh.getScene(scene);
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    log( input_path + ": " +  std::to_string(duration / 1000.0) + " s");

}

void RayTracer::drawAllScenes(){
    for(int i = 0; i < scene.numCameras; i++)
    {
        this->drawScene(i);
    }
}


void RayTracer::drawScene(uint32_t camID){

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

    std::vector<RaytracerThread> raytracers;
    for (uint32_t y = 0; y < height; y++){
        raytracers.push_back(RaytracerThread(scene,y,cam, bvh));
    }

    #pragma omp parallel for
    for (uint32_t y = 0; y < height; y++){
        raytracers[y].drawRow();
    }

    PPM::write_stb(("outputs/" + cam.ImageName).c_str(), scene.image, width, height);
    delete[] scene.image;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_all = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    log(cam.ImageName + " all: " + std::to_string(duration_all / 1000.0) + " s");
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start).count();
    log(cam.ImageName + " drawing: " + std::to_string(duration / 1000.0) + " s");

}