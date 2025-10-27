//
// Created by vicy on 10/21/25.
//

#include "RayTracer.h"


int RaytracerThread::done_threads = 0;

void RayTracer::parseScene(std::string input_path){
    std::cout << "----- Parsing scene from " << input_path << " -----" <<std::endl;
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
    std::cout << "Parsing done." << std::endl;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    std::cout << input_path << ": "
              << duration / 1000.0 << " s" << std::endl;

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
    std::cout << "----- Drawing " << cam.ImageName << " -----" << std::endl;
    auto start = std::chrono::high_resolution_clock::now();
    scene.u = x_product(cam.Up, -cam.Gaze);
    scene.q = cam.Position + (cam.Gaze * cam.nearDistance) + scene.u*cam.l + cam.Up*cam.t; // TODO: near plane verileri doğru mu?
    scene.s_u_0 = (cam.r - cam.l) / cam.width;
    scene.s_v_0 = (cam.t - cam.b) / cam.height;
    // scene.s_v = s_v_0 * 0.5;
    // scene.viewing_ray.pos = cam.Position;

    uint32_t width = cam.width;
    uint32_t height = cam.height;

    scene.image = new unsigned char[width * height * 3];

    std::vector<RaytracerThread> raytracers;
    for (uint32_t y = 0; y < height; y++){
        raytracers.push_back(RaytracerThread(scene,y,cam));
    }

    #pragma omp parallel for
    for (uint32_t y = 0; y < height; y++){
        raytracers[y].drawRow();
    }

    PPM::write_ppm(("outputs/" + cam.ImageName).c_str(), scene.image, width, height);
    delete[] scene.image;
    auto stop = std::chrono::high_resolution_clock::now();
    auto duration_all = duration_cast<std::chrono::milliseconds>(stop - start_time).count();
    std::cout << cam.ImageName << " all: "
    << duration_all / 1000.0 << " s" << std::endl;
    auto duration = duration_cast<std::chrono::milliseconds>(stop - start).count();
    std::cout << cam.ImageName << " drawing: "
              << duration / 1000.0 << " s" << std::endl;

}