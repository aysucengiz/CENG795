#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"


// Hw 1
// dialectrics -> isotropic, Beer's law -> saç baş yolucim

// Hw 2
// ply dosyalarındaki vektör normallerini kullan
// TODO: mirrorda epsilona bak
// TODO: dialectricleri düzelt
// TODO: Surface area heuristics ile pivot belirleme


int main(void) {
    RayTracer raytracer;
    raytracer.output_path = "";

    // raytracer.log("\n\nThe following are simple scenes:");
    // // raytracer.drawFile("jsonFiles/hw2/inputs/ellipsoids.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/spheres.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/simple_transform.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/mirror_room.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/akif_uslu/berserker/two_berserkers.json");


    // raytracer.log("\n\nThe following contain dielectric or conductors:");
    // raytracer.drawFile("jsonFiles/hw2/inputs/metal_glass_plates.json");
    // raytracer.drawFile("jsonFiles/hw1/inputs/cornellbox_recursive.json");
    // raytracer.drawFile("jsonFiles/hw1/inputs/scienceTree_glass.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/dragon_metal.json");

    // raytracer.log("\n\nThe following are moving/dynamic scenes:");
    //raytracer.drawAllFiles("jsonFiles/hw2/inputs/akif_uslu/windmill/input/");
    //raytracer.drawAllFiles("jsonFiles/hw2/inputs/raven/camera_around_david/");
    // raytracer.drawAllFiles("jsonFiles/hw2/inputs/raven/light_around_david/");
    // raytracer.drawAllFiles("jsonFiles/hw2/inputs/raven/camera_zoom_david/");


    raytracer.log("\n\nThe following are time consuming scenes:");
    raytracer.drawFile("jsonFiles/hw2/inputs/grass/grass_desert.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/raven/glaring_davids.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/marching_dragons.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/raven/dragon/dragon_new_ply.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/raven/dragon/dragon_new_right_ply.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/raven/dragon/dragon_new_top_ply.json");



    return 0;

    // simple ones
    raytracer.log("\n\nThe following are the rather simple scenes:");
    raytracer.drawFile("jsonFiles/hw1/inputs/simple.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/spheres.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/spheres_mirror.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/spheres_with_plane.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/two_spheres.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/cornellbox.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/scienceTree.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/bunny.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/bunny_with_plane.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/chinese_dragon.json");

    // revursive ones
    raytracer.log("\n\nDielectic and conductor scenes:");
    raytracer.drawFile("jsonFiles/hw1/inputs/cornellbox_recursive.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/scienceTree_glass.json");

    // smooth ones
    raytracer.log("\n\nAkif Uslu ones (smooth):");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/low_poly_smooth.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/berserker_smooth.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/tower_smooth.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/car_smooth_fixed.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/windmill_smooth.json");

    raytracer.log("\n\nThe ones under the raven folder:");
    raytracer.drawFile("jsonFiles/hw1/inputs/raven/rt_david.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/raven/rt_raven.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/raven/rt_utahteapot_mug_ceng.json");

    // lookat ones
    /*raytracer.log("\n\nScenes with lookAt:");
    raytracer.drawFile("jsonFiles/hw1/inputs/other_dragon.json");*/

    // ply ones
    //raytracer.log("\n\nScenes with ply:");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/ton_Roosendaal_smooth.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/trex_smooth.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/deniz_sayin/lobster.json");



    return 0;
}
