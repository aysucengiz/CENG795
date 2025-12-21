#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"


// Hw 1

// Hw 2
// her şey halloldu :)

// hw 3
// TODO: mesh bvh parallelization

// hw4
// TODO: bump mapping -> biraz sorunlu
// TODO: mipmapping
// TODO: new dragon

int main(void) {
    json data = Parser::getJsonDataFromFile("configs.json");
    std::cout << data << std::endl;
    RayTracer raytracer(data["Configuration"]);;

    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nearest_trilinear.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/plane_trilinear.json");
   // raytracer.drawFile("jsonFiles/hw4/inputs/killeroo_bump_walls.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/dragon/dragon_new_ply.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/galactica_static.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box_all.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/bump_mapping_transformed.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box_no_specular.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin_bump.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_perlin_bump.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nobump_justbump.json");

    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nobump_bump.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/mytap/mytap_final.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin.json");
    // // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box.json");
    // // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box_all.json");
    // // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box_no_specular.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_normal.json");


    return 0;
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_wall.json");
}
