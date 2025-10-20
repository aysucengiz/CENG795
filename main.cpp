#include <iostream>
#include <fstream>
#include <iostream>


#include "raytracer.h"


int main(int argc, char* argv[])
{
    Raytracer raytracer;
    std::cout << "Raytracer" << std::endl;
    raytracer.parseScene(argv[1]);
    if (argc > 2) raytracer.drawScene(std::stoi(argv[2]));
    else          raytracer.drawAllScenes();

    return 0;
}
