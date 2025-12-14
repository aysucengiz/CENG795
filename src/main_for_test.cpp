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
// TODO: mipmapping
// TODO: bump mapping

int main(void) {
    json data = Parser::getJsonDataFromFile("configs.json");
    std::cout << data << std::endl;
    RayTracer raytracer(data["Configuration"]);
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin_bump.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nobump_justbump.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nobump_bump.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/galactica_dynamic.json");
    // raytracer.drawAllFiles("jsonFiles/hw4/inputs/");
    // raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin.json");
    // // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box.json");
    // // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box_all.json");
    // // raytracer.drawFile("jsonFiles/hw4/inputs/wood_box_no_specular.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_normal.json");


    return 0;
    raytracer.drawFile("jsonFiles/hw4/inputs/ellipsoids_texture.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_wall.json");
}
