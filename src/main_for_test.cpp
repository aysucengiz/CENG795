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
// TODO: new dragon

int main(void) {
    json data = Parser::getJsonDataFromFile("configs.json");
    std::cout << data << std::endl;
    RayTracer raytracer(data["Configuration"]);;
    raytracer.drawAllFiles("jsonFiles/hw4/inputs/veach_ajar/");
    raytracer.drawAllFiles("jsonFiles/hw5/inputs/veach_ajar/");
    // raytracer.drawFile("jsonFiles/hw5/inputs/head_env_light.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/tt/scene_glacier.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/cube_point_hdr.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/tt/scene_pisa.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/akin_aydemir/teapot_roughness/teapot_roughness.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/cube_point.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/dragon/dragon_new_ply_with_spot.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/mirror_sphere_env.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/veach_ajar/scene.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/sphere_point_hdr_texture.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/dragon_spot_light_msaa.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/empty_environment_latlong.json");
    // raytracer.drawFile("jsonFiles/hw5/inputs/empty_environment_light_probe.json");
    return 0;
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nearest_bilinear.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_nearest_trilinear.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/plane_trilinear.json");
   // raytracer.drawFile("jsonFiles/hw4/inputs/killeroo_bump_walls.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/dragon/dragon_new_ply.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/brickwall_with_normalmap.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/sphere_normal.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/galactica_static.json");
    // raytracer.drawFile("jsonFiles/hw4/inputs/galactica_dynamic.json");
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
