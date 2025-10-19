//
// Created by aysu on 04.10.2025.
//

#include "Parser.h"




json Parser::getJsonDataFromFile(std::string inpFile){

    std::ifstream f(inpFile);
    json j = json::parse(f);
    return j;
}


void Parser::parseScene(std::string inpFile, SceneInput &sceneInput){
    json inp = getJsonDataFromFile(inpFile);
    if(PRINTINIT) std::cout << "Scene Input: " << std::endl;

    sceneInput.MaxRecursionDepth =  inp["Scene"].contains("MaxRecursionDepth") ? std::stoi(inp["Scene"]["MaxRecursionDepth"].get<std::string>()) : DEFAULT_MAX_DEPTH ;
    if(PRINTINIT) std::cout << "MaxRecursionDepth: " << sceneInput.MaxRecursionDepth << std::endl;

    sceneInput.BackgroundColor = inp["Scene"].contains("BackgroundColor") ? Color(inp["Scene"]["BackgroundColor"].get<std::string>()) : Color(0,0,0);
    if(PRINTINIT) std::cout << "BackgroundColor: " << sceneInput.BackgroundColor << std::endl;

    sceneInput.ShadowRayEpsilon = inp["Scene"].contains("ShadowRayEpsilon") ? std::stod(inp["Scene"]["ShadowRayEpsilon"].get<std::string>()) : DEFAULT_SHADOW_EPS;
    if(PRINTINIT) std::cout << "ShadowRayEpsilon: " << sceneInput.ShadowRayEpsilon << std::endl;

    sceneInput.IntersectionTestEpsilon = inp["Scene"].contains("IntersectionTestEpsilon") ?  std::stod(inp["Scene"]["IntersectionTestEpsilon"].get<std::string>()) : DEFAULT_INTERS_EPS;
    if(PRINTINIT) std::cout << "IntersectionTestEpsilon: " << sceneInput.IntersectionTestEpsilon << std::endl;

    getCameras(inp["Scene"]["Cameras"], sceneInput);
    getLights(inp["Scene"]["Lights"], sceneInput);
    getMaterials(inp["Scene"]["Materials"]["Material"], sceneInput);
    getVertexData(inp["Scene"]["VertexData"], sceneInput);
    getObjects(inp["Scene"]["Objects"], sceneInput);
}


void Parser::getCameras(json inp, SceneInput &sceneInput){
    json& Cameras = inp["Camera"];
    int numCameras = Cameras.size();
    if (Cameras.is_object())
    {
        Camera c(
                std::stoi(Cameras["_id"].get<std::string>()),
                Vertex(Cameras["Position"]),
                Vec3r(Cameras["Gaze"]),
                Vec3r(Cameras["Up"]),
                Cameras["NearPlane"],
                std::stoi(Cameras["NearDistance"].get<std::string>()),
                Cameras["ImageResolution"],
                Cameras["ImageName"]
            );
        sceneInput.Cameras.push_back(c);
        if(PRINTINIT) std::cout << c << std::endl;
    }else
    {
        for(int i = 0; i < numCameras; i++){
            Camera c(
                std::stoi(Cameras[i]["_id"].get<std::string>()),
                Vertex(Cameras[i]["Position"]),
                Vec3r(Cameras[i]["Gaze"]),
                Vec3r(Cameras[i]["Up"]),
                Cameras[i]["NearPlane"],
                std::stoi(Cameras[i]["NearDistance"].get<std::string>()),
                Cameras[i]["ImageResolution"],
                Cameras[i]["ImageName"]
            );
            sceneInput.Cameras.push_back(c);
            if(PRINTINIT) std::cout << c << std::endl;
        }
    }



}
void Parser::getLights(json inp, SceneInput &sceneInput){
    sceneInput.AmbientLight = Color(inp["AmbientLight"]);
    if(PRINTINIT) std::cout << "AmbientLight: " << sceneInput.AmbientLight << std::endl;

    json& pointLights = inp["PointLight"];
    int numPointLights = pointLights.size();
    for(int i=0; i < numPointLights; i++){
        PointLight pl(
                std::stoi(pointLights[i]["_id"].get<std::string>()),
                Vertex(pointLights[i]["Position"]),
                Color(pointLights[i]["Intensity"])
        );
        sceneInput.PointLights.push_back(pl);

        if(PRINTINIT) std::cout << pl << std::endl;
    }


}

void Parser::getMaterials(json inp, SceneInput &sceneInput){

    int numMaterials = inp.size();
    for(int i=0; i < numMaterials; i++){

        Material m(
                std::stoi(inp[i]["_id"].get<std::string>()),
                Color(inp[i]["AmbientReflectance"]),
                Color(inp[i]["DiffuseReflectance"]),
                Color(inp[i]["SpecularReflectance"]),
                Color(inp[i]["MirrorReflectance"]),
                std::stoi(inp[i]["PhongExponent"].get<std::string>())
        );
        sceneInput.Materials.push_back(m);

        if(PRINTINIT) std::cout << m << std::endl;
    }


}

void Parser::getVertexData(json inp, SceneInput &sceneInput){
    std::istringstream verticesStream(inp["_data"].get<std::string>());
    real x,y,z;

    while (verticesStream >> x >> y >> z) {
        sceneInput.Vertices.push_back(Vertex(x, y, z));
        // if(PRINTINIT) std::cout << x << " " << y <<" " <<  z << std::endl;
    }

}

void Parser::getObjects(json inp, SceneInput &sceneInput){
    // getTriangles

    if(inp.contains("Triangle")){
        json& Triangles = inp["Triangle"];
        uint32_t numTriangles = Triangles.size();
        for(int i=0; i < numTriangles; i++){
            Triangle t(
                    std::stoi(Triangles[i]["_id"].get<std::string>()),
                    Triangles[i]["Indices"],
                    std::stoi(Triangles[i]["Material"].get<std::string>()) - 1
            );
            computeTriangleValues(t,sceneInput); // TODO: would this work
            sceneInput.Triangles.push_back(t);

            if(PRINTINIT) std::cout << t << std::endl;
        }
    }


    // getSpheres
    if(inp.contains("Sphere")){
        json& Spheres = inp["Sphere"];
        uint32_t numSpheres = Spheres.size();
        for(int i=0; i < numSpheres; i++){
            Sphere s(
                    std::stoi(Spheres[i]["_id"].get<std::string>()),
                    std::stoi(Spheres[i]["Center"].get<std::string>()) - 1,
                    std::stod(Spheres[i]["Radius"].get<std::string>()),
                    std::stoi(Spheres[i]["Material"].get<std::string>()) - 1
            );
            sceneInput.Spheres.push_back(s);

            if(PRINTINIT) std::cout << s << std::endl;
        }
    }

    // getMeshes
    if(inp.contains("Mesh")){
        json& Meshes = inp["Mesh"];
        int numMeshes = Meshes.size();
        for(int i=0; i < numMeshes; i++){
            std::string typeString = "";
            if (sceneInput.Materials[std::stoi(Meshes[i]["Material"].get<std::string>()) - 1].materialType != MaterialType::NONE)
            {
                if (Meshes[i]["Faces"].contains("_plyFile"))
                {
                    Mesh m(
                            std::stoi(Meshes[i]["_id"].get<std::string>()),
                            Meshes[i]["_shadingMode"].get<std::string>(),
                            std::stoi(Meshes[i]["Material"].get<std::string>()) - 1,
                            Meshes[i]["Faces"]["_plyFile"].get<std::string>(),
                            true
                        );

                    sceneInput.Meshes.push_back(m);
                    if(PRINTINIT) std::cout << m << std::endl;
                }
                else
                {
                    Mesh m(
                            std::stoi(Meshes[i]["_id"].get<std::string>()),
                            Meshes[i]["_shadingMode"].get<std::string>(),
                            std::stoi(Meshes[i]["Material"].get<std::string>()) - 1,
                            Meshes[i]["Faces"]["_data"].get<std::string>(),
                            false
                        );

                    sceneInput.Meshes.push_back(m);
                    // if(PRINTINIT) std::cout << m << std::endl;
                }
            }


        }
    }
}
void Parser::computeTriangleValues(Triangle &t, SceneInput &scene)
{
    Vertex &a = scene.Vertices[t.indices[0]];
    Vertex &b = scene.Vertices[t.indices[1]];
    Vertex &c = scene.Vertices[t.indices[2]];
    t.a_b = a - b;
    t.a_c = a - c;
    t.n =  x_product((b-a), (c-a)).normalize();
}