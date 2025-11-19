#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"


// Hw 1
// dialectrics -> isotropic, Beer's law -> saç baş yolucim

// Hw 2
// her şey halloldu :)


int main(void) {
    RayTracer raytracer;

    raytracer.drawAllFiles("jsonFiles/hw1/inputs/raven/");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/windmill_smooth.json");
    raytracer.drawFile("jsonFiles/hw1/inputs/akif_uslu/trex_smooth.json");
    raytracer.drawAllFiles("jsonFiles/hw1/inputs/deniz_sayin/");

    return 0;
    raytracer.log("\n\nHW 1 !!!!!!!!!!!!!!!!!!!");
    raytracer.drawAllFiles("jsonFiles/hw1/inputs/");
    raytracer.log("\n\nThe following are simple scenes:");
    raytracer.drawFile("jsonFiles/hw2/inputs/ellipsoids.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/spheres.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/simple_transform.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/mirror_room.json");
    raytracer.drawFile("jsonFiles/hw2/inputs/akif_uslu/berserker/two_berserkers.json");


    raytracer.log("\n\nThe following contain dielectric or conductors:");
    raytracer.drawFile("jsonFiles/hw2/inputs/metal_glass_plates.json");
    //raytracer.drawFile("jsonFiles/hw1/inputs/cornellbox_recursive.json");
    //raytracer.drawFile("jsonFiles/hw1/inputs/scienceTree_glass.json");


    raytracer.log("\n\nThe following are time consuming scenes:");
    raytracer.drawFile("jsonFiles/hw2/inputs/grass/grass_desert.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/raven/glaring_davids.json");
    // raytracer.drawFile("jsonFiles/hw2/inputs/marching_dragons.json");
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
