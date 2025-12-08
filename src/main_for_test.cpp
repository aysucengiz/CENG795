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

    raytracer.parseScene("jsonFiles/hw4/");

    return 0;
}
