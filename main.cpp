#include <iostream>
#include <fstream>
#include <iostream>


#include "raytracer.h"


int main(void) {
    Raytracer raytracer;
    //raytracer.parseScene("jsonFiles/deneme.json");
    //raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/akif_uslu/low_poly_smooth.json");
    raytracer.drawAllScenes();
    //raytracer.parseScene("jsonFiles/akif_uslu/berserker_smooth.json");
    //raytracer.drawAllScenes();
    //raytracer.parseScene("jsonFiles/akif_uslu/tower_smooth.json");
    //raytracer.drawAllScenes();
    //raytracer.parseScene("jsonFiles/akif_uslu/car_smooth_fixed.json");
    //raytracer.drawAllScenes();
    return 0;
}
