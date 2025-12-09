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
// TODO: Get Texeller yazılacak
// TODO: Replace normal ve bump mapping yapılacak
// TODO: meshe fonksiyonlar eklenecek (trivial)
// TODO: mipmapping
// TODO: << overloads

int main(void) {
    json data = Parser::getJsonDataFromFile("configs.json");
    std::cout << data << std::endl;
    RayTracer raytracer(data["Configuration"]);

    raytracer.parseScene("jsonFiles/hw4/");

    return 0;
}
