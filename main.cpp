#include <iostream>
#include <fstream>
#include <iostream>


#include "raytracer.h"


int main(void) {
    Raytracer raytracer;
    raytracer.parseScene("jsonFiles/akif_uslu/low_poly_smooth.json");
    raytracer.drawAllScenes();
    return 0;
}
