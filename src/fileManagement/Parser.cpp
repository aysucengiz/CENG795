//
// Created by aysu on 04.10.2025.
//

#include "Parser.h"


// TODO: material tipleri, bazılarında mesela _type : mirror olayı söz konusu



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
    getVertexData(inp["Scene"], sceneInput);
    getObjects(inp["Scene"]["Objects"], sceneInput);
}


void Parser::getCameras(json inp, SceneInput &sceneInput){
    json& Cameras = inp["Camera"];
    int numCameras = Cameras.size();
    if (Cameras.is_object()) addCamera(Cameras, sceneInput);
    else  for(int i = 0; i < numCameras; i++) addCamera(Cameras[i], sceneInput);
}


void Parser::addLight(json pointLights, SceneInput &sceneInput)
{
    PointLight pl(
            std::stoi(pointLights["_id"].get<std::string>()) - 1,
            Vertex(pointLights["Position"]),
            Color(pointLights["Intensity"])
    );
    sceneInput.PointLights.push_back(pl);

    if(PRINTINIT) std::cout << pl << std::endl;
}


void Parser::getLights(json inp, SceneInput &sceneInput){
    if (inp.contains("AmbientLight"))
    {
        sceneInput.AmbientLight = Color(inp["AmbientLight"]);
    if(PRINTINIT) std::cout << "AmbientLight: " << sceneInput.AmbientLight << std::endl;
    }
    if (inp.contains("PointLight"))
    {
        json& pointLights = inp["PointLight"];
        int numPointLights = pointLights.size();
        if (pointLights.is_object()) addLight(pointLights, sceneInput);
        else for(int i=0; i < numPointLights; i++) addLight(pointLights[i], sceneInput);
    }


}



void Parser::getMaterials(json inp, SceneInput &sceneInput)
{
    int numMaterials = inp.size();
    if (inp.is_object()) addMaterial(inp, sceneInput);
    else for(int i=0; i < numMaterials; i++) addMaterial(inp[i], sceneInput);
}

void Parser::getVertexData(json inp, SceneInput &sceneInput){

    std::istringstream verticesStream(inp["VertexData"]["_data"].get<std::string>());
    real x,y,z;
    while (verticesStream >> x >> y >> z) {
        sceneInput.Vertices.push_back(CVertex(sceneInput.Vertices.size(),x, y, z));

        //if(PRINTINIT) std::cout << sceneInput.Vertices[sceneInput.Vertices.size()-1] << std::endl;
    }


    std::istringstream (inp["VertexData"]["_data"].get<std::string>());

}







void Parser::getObjects(json inp, SceneInput &sceneInput){
    // getTriangles
    uint32_t curr_id = sceneInput.objects.size();

    if(inp.contains("Triangle")){
        json& Triangles = inp["Triangle"];
            uint32_t numTriangles = Triangles.size();
        if (Triangles.is_object())  addTriangle(Triangles, sceneInput, curr_id);
        else  for(int i=0; i < numTriangles; i++) addTriangle(Triangles[i], sceneInput, curr_id);
    }

    // getSpheres
    if(inp.contains("Sphere")){
        json& Spheres = inp["Sphere"];
        uint32_t numSpheres = Spheres.size();
        if (Spheres.is_object())              addSphere(Spheres, sceneInput, curr_id);
        else for(int i=0; i < numSpheres; i++) addSphere(Spheres[i], sceneInput, curr_id);
    }

    // getMeshes
    if(inp.contains("Mesh")){
        json& Meshes = inp["Mesh"];
        int numMeshes = Meshes.size();
        if (Meshes.is_object())              addMesh(Meshes, sceneInput, curr_id);
        else for(int i=0; i < numMeshes; i++) addMesh(Meshes[i],sceneInput, curr_id);
    }

    // getPlanes
    if(inp.contains("Plane")){
        json& planes = inp["Plane"];
        int numPlanes = planes.size();
        if (planes.is_object())              addPlane(planes, sceneInput, curr_id);
        else for(int i=0; i < numPlanes; i++) addPlane(planes[i],sceneInput, curr_id);
    }

    for(int i=0; i < sceneInput.Vertices.size(); i++)
    {
        sceneInput.Vertices[i].n = sceneInput.Vertices[i].n.normalize();
    }
}






void Parser::addCamera(json Cameras, SceneInput &sceneInput)
{
    std::string nearPlane = "";
    Vec3r Gaze;
    if (Cameras.contains("_type") && Cameras["_type"].get<std::string>() == "lookAt")
    {
        // compute near plane here
        std::istringstream ss(Cameras["ImageResolution"].get<std::string>());
        real width, height;
        ss >> width >> height;
        real aspect = width/height;
        int FovY = std::stoi(Cameras["FovY"].get<std::string>());
        real t = tan(FovY*0.5/180*M_PI) *  std::stoi(Cameras["NearDistance"].get<std::string>());
        nearPlane = std::to_string(-t*aspect) + " "
                   +std::to_string(t*aspect) + " "
                   +std::to_string(-t) + " "
                   +std::to_string(t);
        Gaze = Vertex(Cameras["GazePoint"]) - Vertex(Cameras["Position"]);
    }
    else
    {
        nearPlane = Cameras["NearPlane"];
        Gaze = Vec3r(Cameras["Gaze"]);
    }

    Camera c(
               std::stoi(Cameras["_id"].get<std::string>()) - 1,
               Vertex(Cameras["Position"]),
               Gaze,
               Vec3r(Cameras["Up"]),
               nearPlane,
               std::stoi(Cameras["NearDistance"].get<std::string>()),
               Cameras["ImageResolution"],
               Cameras["ImageName"]
           );
    sceneInput.Cameras.push_back(c);
    if(PRINTINIT) std::cout << c << std::endl;
}


void Parser::addMaterial(json inp, SceneInput &sceneInput)
{

    Material m(
            std::stoi(inp["_id"].get<std::string>()) - 1,
            Color(inp["AmbientReflectance"]),
            Color(inp["DiffuseReflectance"]),
            Color(inp["SpecularReflectance"]),
            inp.contains("PhongExponent") ? std::stoi(inp["PhongExponent"].get<std::string>()) : 1.0,
            inp.contains("_type") ? inp["_type"].get<std::string>() :  "",
            inp.contains("MirrorReflectance") ? Color(inp["MirrorReflectance"]) : Color(),
            inp.contains("AbsorptionCoefficient") ? Color(inp["AbsorptionCoefficient"]) : Color(),
            inp.contains("RefractionIndex") ? std::stod(inp["RefractionIndex"].get<std::string>()) : 0.0,
            inp.contains("AbsorptionIndex") ? std::stod(inp["AbsorptionIndex"].get<std::string>()) : 0.0
            );
    sceneInput.Materials.push_back(m);

    if(PRINTINIT) std::cout << m << std::endl;
}

void Parser::addTriangle(json tri, SceneInput &sceneInput, uint32_t &curr_id)
{

    std::istringstream ss(tri["Indices"].get<std::string>());
    uint32_t ind[3];
    ss >> ind[0] >> ind[1] >> ind[2];
    if (ss.fail()) {
        throw std::invalid_argument("Invalid triangle indices string: " + tri["Indices"].get<std::string>());
    }


    sceneInput.objects.push_back(new Triangle(
                                            curr_id,
                                            sceneInput.Vertices[ind[0] - 1],
                                            sceneInput.Vertices[ind[1] - 1],
                                            sceneInput.Vertices[ind[2] - 1],
                                            sceneInput.Materials[std::stoi(tri["Material"].get<std::string>()) - 1]));

    if(PRINTINIT) std::cout << *dynamic_cast<Triangle*>(sceneInput.objects[curr_id]) << std::endl;
    curr_id++;
}

void Parser::addSphere(json s, SceneInput &sceneInput, uint32_t &curr_id)
{
    sceneInput.objects.push_back(new Sphere(
                                    curr_id,
                                    sceneInput.Vertices[std::stoi(s["Center"].get<std::string>()) - 1],
                                    std::stod(s["Radius"].get<std::string>()),
                                    sceneInput.Materials[std::stoi(s["Material"].get<std::string>()) - 1]
                            ));

    if(PRINTINIT) std::cout << *dynamic_cast<Sphere*>(sceneInput.objects[curr_id])<< std::endl;

    curr_id++;
}

void Parser::addMesh(json mes, SceneInput &sceneInput, uint32_t &curr_id)
{
    std::string typeString = "";
    if (sceneInput.Materials[std::stoi(mes["Material"].get<std::string>()) - 1].materialType != MaterialType::NONE)
    {
        std::string dataLine;
        bool read_from_file;
        uint32_t numVerticesUntilNow = sceneInput.Vertices.size();
        if (mes["Faces"].contains("_plyFile"))
        {

            read_from_file = true;
            dataLine = JSON_FILES + mes["Faces"]["_plyFile"].get<std::string>();

            happly::PLYData plyIn(dataLine);
            std::vector<float> xs = plyIn.getElement("vertex").getProperty<float>("x");
            std::vector<float> ys = plyIn.getElement("vertex").getProperty<float>("y");
            std::vector<float> zs = plyIn.getElement("vertex").getProperty<float>("z");

            for (int j=0; j<xs.size(); j++)
            {
                sceneInput.Vertices.push_back(CVertex(numVerticesUntilNow+j,xs[j],ys[j],zs[j]));
            }
        }
        else
        {

            read_from_file = false;
            dataLine = mes["Faces"]["_data"].get<std::string>();
        }


        sceneInput.objects.push_back(new Mesh(curr_id,
                mes.contains("_shadingMode") ? mes["_shadingMode"].get<std::string>() : "flat",
                sceneInput.Materials[std::stoi(mes["Material"].get<std::string>()) - 1],
                dataLine,
                read_from_file,
                sceneInput.Vertices,
                numVerticesUntilNow));
        Mesh *temp_m = dynamic_cast<Mesh*>(sceneInput.objects[curr_id]);
        curr_id++;
        if(PRINTINIT) std::cout <<  "Mesh " << temp_m->_id << " has " << temp_m->Faces.size() << " faces." << std::endl; //std::cout <<  temp_m << std::endl;
    }
}


void Parser::addPlane(json p, SceneInput &sceneInput, uint32_t &curr_id)
{
    sceneInput.objects.push_back(new Plane(
                                curr_id,
                                sceneInput.Vertices[std::stoi(p["Point"].get<std::string>()) - 1].v,
                                p["Normal"].get<std::string>(),
                                sceneInput.Materials[std::stoi(p["Material"].get<std::string>()) - 1]
                        ));

    if(PRINTINIT) std::cout <<  sceneInput.objects[curr_id]<< std::endl;

    curr_id++;
}


