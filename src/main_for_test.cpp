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

int main(void) {
    json data = Parser::getJsonDataFromFile("configs.json");
    std::cout << data << std::endl;
    RayTracer raytracer(data["Configuration"]);


    raytracer.drawFile("jsonFiles/hw4/inputs/cube_cushion.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_wall_normal.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_waves.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_perlin.json");

    return 0;
    raytracer.drawFile("jsonFiles/hw4/inputs/ellipsoids_texture.json");
    raytracer.drawFile("jsonFiles/hw4/inputs/cube_wall.json");
}
