#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"


int main(void) {
    RayTracer raytracer;
    raytracer.parseScene("jsonFiles/deneme.json");
    raytracer.drawAllScenes();


    raytracer.parseScene("jsonFiles/inputs/simple.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres_mirror.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres_with_plane.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/two_spheres.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/bunny.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/bunny_with_plane.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/chinese_dragon.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/cornellbox.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/cornellbox_recursive.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/other_dragon.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/other_dragon.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/scienceTree.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/scienceTree_glass.json");
    raytracer.drawAllScenes();



    raytracer.parseScene("jsonFiles/inputs/deniz_sayin/lobster.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/low_poly_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/berserker_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/tower_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/car_smooth_fixed.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/windmill_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/trex_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/ton_Roosendaal_smooth.json");
    raytracer.drawAllScenes();
    return 0;
}
