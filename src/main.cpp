#include <iostream>
#include <fstream>
#include <iostream>


#include "rayTracer/raytracerThread.h"

// add time record
// TODO: conductors and dialectrics (Fresnel reflection) (simple mirrors do not follow fresnel)
// TODO: dialectrics -> isotropic, Beer's law
//round to nearest integer during clamping
// TODO: back-face culling (bu zaten yapılıyor mu?) Experiment with enabling and disabling it in your ray tracers and report your time measurements in your blog post.
// TODO: parser should check for degenerate triangles
// TODO: bu yapılıyor mu -> shadowrayepsilon for reglections/refractions
// TODO: cameras, lookAt These cameras assume a symmetric image plane centered along the gaze direction. These cameras take a \GazePoint"
//       to specify the point that the camera is looking at. You can nd the gaze direction by sub-
//        tracting the camera position from this gaze point. The \FovY" parameter species the eld
//         of view in degrees that the image plane covers in its vertical direction. The aspect ratio is...
// TODO: vertices, orientation
// TODO: .ply file management
// TODO: smooth or flat shading
// TODO: ppm'e yazmayı hocanın dediği gibi yap


int main(int argc, char* argv[])
{
    RaytracerThread raytracer;
    std::cout << "Raytracer" << std::endl;
    raytracer.parseScene(argv[1]);
    if (argc > 2) raytracer.drawScene(std::stoi(argv[2]));
    else          raytracer.drawAllScenes();

    return 0;
}
