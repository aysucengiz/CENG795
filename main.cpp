#include <iostream>
#include <fstream>
#include <iostream>


#include "raytracer.h"


int main(void) {
    Raytracer raytracer;
    raytracer.parseScene("jsonFiles/akif_uslu/car_smooth_fixed.json");
    raytracer.drawScene(0);
    return 0;
}
