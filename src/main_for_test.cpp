#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/RayTracer.h"


// Hw 1
// back-face culling (yapıyoruz) Experiment with enabling and disabling it in your ray tracers and report your time measurements in your blog post.
// dialectrics -> isotropic, Beer's law -> saç baş yolucim

// Hw 2
// TODO: translation of EVERYTHING




int main(void) {
    RayTracer raytracer;

    // simple ones
    /*raytracer.log("\n\nThe following are the rather simple scenes:");
    raytracer.parseScene("jsonFiles/inputs/simple.json");
    raytracer.drawAllScenes();*/
    raytracer.parseScene("jsonFiles/inputs/spheres.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/spheres_mirror.json");
    raytracer.drawAllScenes();
    /*raytracer.parseScene("jsonFiles/inputs/spheres_with_plane.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/two_spheres.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/cornellbox.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/scienceTree.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/bunny.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/bunny_with_plane.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/chinese_dragon.json");
    raytracer.drawAllScenes();

    // revursive ones
    raytracer.log("\n\nDielectic and conductor scenes:");
    raytracer.parseScene("jsonFiles/inputs/cornellbox_recursive.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/scienceTree_glass.json");
    raytracer.drawAllScenes();

    // smooth ones
    raytracer.log("\n\nAkif Uslu ones (smooth):");
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/low_poly_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/berserker_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/tower_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/car_smooth_fixed.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/windmill_smooth.json");
    raytracer.drawAllScenes();

    raytracer.log("\n\nThe ones under the raven folder:");
    raytracer.parseScene("jsonFiles/inputs/raven/rt_david.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/raven/rt_raven.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/raven/rt_utahteapot_mug_ceng.json");
    raytracer.drawAllScenes();

    // lookat ones
    raytracer.log("\n\nScenes with lookAt:");
    raytracer.parseScene("jsonFiles/inputs/other_dragon.json");
    raytracer.drawAllScenes();*/

    // ply ones
    //raytracer.log("\n\nScenes with ply:");
    /*raytracer.parseScene("jsonFiles/inputs/akif_uslu/ton_Roosendaal_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/akif_uslu/trex_smooth.json");
    raytracer.drawAllScenes();
    raytracer.parseScene("jsonFiles/inputs/deniz_sayin/lobster.json");
    raytracer.drawAllScenes();*/



    return 0;
}
