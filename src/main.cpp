#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"




int main(int argc, char* argv[])
{
    json data = Parser::getJsonDataFromFile("configs.json");
    RayTracer raytracer(data["Configuration"]);
    raytracer.parseScene(argv[1]);
    if (argc > 2) raytracer.drawScene(std::stoi(argv[2]));
    else          raytracer.drawAllScenes();

    return 0;
}
