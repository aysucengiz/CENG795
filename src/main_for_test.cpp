#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"


// Hw 1

// Hw 2
// her şey halloldu :)

// hw 3
// TODO: mesh bvh parallelization

int main(void) {
    json data = Parser::getJsonDataFromFile("configs.json");
    std::cout << data << std::endl;
    RayTracer raytracer(data["Configuration"]);


    // raytracer.log("\n\nHW 1 !!!!!!!!!!!!!!!!!!!");
    // // raytracer.drawAllFiles("jsonFiles/hw1/inputs/");
    // raytracer.log("\n\nThe following are simple scenes:");
    // // raytracer.drawFile("jsonFiles/hw2/inputs/ellipsoids.json");
    // // raytracer.drawFile("jsonFiles/hw2/inputs/spheres.json");
    // // raytracer.drawFile("jsonFiles/hw2/inputs/simple_transform.json");
    // // raytracer.drawFile("jsonFiles/hw2/inputs/mirror_room.json");
    // // raytracer.drawFile("jsonFiles/hw2/inputs/akif_uslu/berserker/two_berserkers.json");
    // raytracer.drawFile("jsonFiles/hw1/inputs/raven/rt_david.json");
    //
    // raytracer.drawFile("jsonFiles/hw3/inputs/cornellbox_boxes_dynamic.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/dragon_dynamic.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/cornellbox_area.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/spheres_dof.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/ramazan_tokay/chessboard_arealight.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/ramazan_tokay/chessboard_arealight_dof.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/ramazan_tokay/chessboard_arealight_dof_glass_queen.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/focusing_dragons.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/cornellbox_brushed_metal.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/metal_glass_plates.json");
    // raytracer.drawFile("jsonFiles/hw3/inputs/ramazan_tokay/deadmau5_scene.json");
    raytracer.drawFile("jsonFiles/hw3/inputs/ramazan_tokay/wine_glass_scene.json");
    // raytracer.drawAllFiles("jsonFiles/hw3/inputs/tap_water/json");
    return 0;

    // std::cout << raytracer << std::endl;
    // raytracer.drawAllFiles("jsonFiles/hw1/inputs/akif_uslu/");
    // raytracer.drawAllFiles("jsonFiles/hw1/inputs/deniz_sayin/");



    raytracer.log("\n\nThe following contain dielectric or conductors:");
    raytracer.drawFile("jsonFiles/hw2/inputs/metal_glass_plates.json");
    //raytracer.drawFile("jsonFiles/hw1/inputs/cornellbox_recursive.json");
    //raytracer.drawFile("jsonFiles/hw1/inputs/scienceTree_glass.json");


    raytracer.log("\n\nThe following are time consuming scenes:");
    raytracer.drawFile("jsonFiles/hw2/inputs/grass/grass_desert.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/raven/dragon/dragon_new_ply.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/raven/dragon/dragon_new_right_ply.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/raven/dragon/dragon_new_top_ply.json");

    raytracer.log("\n\nThe following are moving/dynamic scenes:");
    // raytracer.drawAllFiles("jsonFiles/hw2/inputs/akif_uslu/windmill/input/");
    // raytracer.drawAllFiles("jsonFiles/hw2/inputs/raven/light_around_david/");
     raytracer.drawAllFiles("jsonFiles/hw2/inputs/raven/camera_zoom_david/");
    raytracer.drawAllFiles("jsonFiles/hw2/inputs/raven/camera_around_david/");




    return 0;
}
