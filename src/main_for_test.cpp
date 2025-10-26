#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"

// add time record
//round to nearest integer during clamping

// TODO: planelere bak
// TODO: ppm'e yazmayı hocanın dediği gibi yap
// TODO: smooth or flat shading
// TODO: .ply file management
// TODO: conductors and dialectrics (Fresnel reflection) (simple mirrors do not follow fresnel)
// TODO: dialectrics -> isotropic, Beer's law
// TODO: parser should check for degenerate triangles
// TODO: bu yapılıyor mu -> shadowrayepsilon for reglections/refractions
// TODO: cameras, lookAt These cameras assume a symmetric image plane centered along the gaze direction. These cameras take a \GazePoint"
//       to specify the point that the camera is looking at. You can nd the gaze direction by sub-
//        tracting the camera position from this gaze point. The \FovY" parameter species the eld
//         of view in degrees that the image plane covers in its vertical direction. The aspect ratio is...
// TODO: vertices, orientation
// TODO: back-face culling (bu zaten yapılıyor mu?) Experiment with enabling and disabling it in your ray tracers and report your time measurements in your blog post.


int main(void) {
    RayTracer raytracer;
    /*raytracer.parseScene("jsonFiles/deneme.json");
    raytracer.drawAllScenes();


    raytracer.parseScene("jsonFiles/inputs/simple.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres_mirror.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres_with_plane.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/two_spheres.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/bunny.json");
    raytracer.drawAllScenes();*/
    /*raytracer.parseScene("jsonFiles/inputs/bunny_with_plane.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/chinese_dragon.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/cornellbox.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/cornellbox_recursive.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/other_dragon.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/other_dragon.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/scienceTree.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/scienceTree_glass.json");
    raytracer.drawAllScenes();



    raytracer.parseScene("jsonFiles/inputs/deniz_sayin/lobster.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/low_poly_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/berserker_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/tower_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/car_smooth_fixed.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/windmill_smooth.json");
    raytracer.drawAllScenes();*/
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/trex_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/ton_Roosendaal_smooth.json");
    raytracer.drawAllScenes();
    return 0;
}
