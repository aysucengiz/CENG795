//
// Created by aysu on 04.10.2025.
//

#include "Parser.h"
#include "../dataTypes/functions/overloads.h"

// TODO: material tipleri, bazılarında mesela _type : mirror olayı söz konusu

namespace Parser
{
    uint32_t type1_triStartID;
    uint32_t type2_sphereStartID;
    uint32_t type3_meshStartID;
    uint32_t type4_instStartID;
    uint32_t type5_planeStartID;

    uint32_t trans1_transStartID;
    uint32_t trans2_scaleStartID;
    uint32_t trans3_rotStartID;
}

json Parser::getJsonDataFromFile(std::string inpFile){

    std::ifstream f(inpFile);
    json j = json::parse(f);
    return j;
}


void Parser::parseScene(std::string inpFile, SceneInput &sceneInput){
    json inp = getJsonDataFromFile(inpFile);
    std::string root = inpFile.substr(0, inpFile.find_last_of('/')) + "/";

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

    if (inp["Scene"].contains("Transformations"))
    {
        std::cout <<inp["Scene"] <<std::endl;
        getTransformations(inp["Scene"]["Transformations"], sceneInput);
    }

    getVertexData(inp["Scene"], sceneInput);
    getObjects(inp["Scene"]["Objects"], sceneInput, root);
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
    sceneInput.AmbientLight =inp.contains("AmbientLight") ? Color(inp["AmbientLight"]) : Color();
    if(PRINTINIT) std::cout << "AmbientLight: " << sceneInput.AmbientLight << std::endl;

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

void Parser::getTransformations(json inp, SceneInput &sceneInput)
{
    // getTriangles
    trans1_transStartID = sceneInput.transforms.size();
    if(inp.contains("Translation")){
        json& Translations = inp["Translation"];
        uint32_t numTranslation = Translations.size();
        if (Translations.is_object())  addTranslation(Translations, sceneInput);
        else  for(int i=0; i < numTranslation; i++) addTranslation(Translations[i], sceneInput);
    }

    // getSpheres
    trans2_scaleStartID = sceneInput.transforms.size();
    if(inp.contains("Scaling")){
        json& Scalings = inp["Scaling"];
        uint32_t numScalings = Scalings.size();
        if (Scalings.is_object())              addScaling(Scalings, sceneInput);
        else for(int i=0; i < numScalings; i++) addScaling(Scalings[i], sceneInput);
    }

    // getMeshes
    trans3_rotStartID = sceneInput.transforms.size();
    if(inp.contains("Rotation")){
        json& Rotations = inp["Rotation"];
        int numRotations = Rotations.size();
        if (Rotations.is_object())              addRotation(Rotations, sceneInput);
        else for(int i=0; i < numRotations; i++) addRotation(Rotations[i],sceneInput);
    }

}





void Parser::getVertexData(json inp, SceneInput &sceneInput){

    std::istringstream verticesStream(inp["VertexData"]["_data"].get<std::string>());
    std::string ori = "xyz";
    if (inp["VertexData"].contains("_type")) ori = inp["VertexData"]["_type"].get<std::string>();
    int x = ori.find('x');
    int y = ori.find('y');
    int z = ori.find('z');
    real mapped[3];
    while (verticesStream >> mapped[0] >> mapped[1] >> mapped[2]) {
        sceneInput.Vertices.push_back(CVertex(sceneInput.Vertices.size(),mapped[x],mapped[y],mapped[z]));

        //if(PRINTINIT) std::cout << sceneInput.Vertices[sceneInput.Vertices.size()-1] << std::endl;
    }
}







void Parser::getObjects(json inp, SceneInput &sceneInput, std::string root){
    // getTriangles
    uint32_t curr_id = sceneInput.objects.size();
    std::cout << "getobjects: " << curr_id << std::endl;
    type1_triStartID = curr_id;
    if(inp.contains("Triangle")){
        json& Triangles = inp["Triangle"];
            uint32_t numTriangles = Triangles.size();
        if (Triangles.is_object())  addTriangle(Triangles, sceneInput, curr_id);
        else  for(int i=0; i < numTriangles; i++) addTriangle(Triangles[i], sceneInput, curr_id);
    }
    type2_sphereStartID = curr_id;

    // getSpheres
    if(inp.contains("Sphere")){
        json& Spheres = inp["Sphere"];
        uint32_t numSpheres = Spheres.size();
        if (Spheres.is_object())              addSphere(Spheres, sceneInput, curr_id);
        else for(int i=0; i < numSpheres; i++) addSphere(Spheres[i], sceneInput, curr_id);
    }
    type3_meshStartID = curr_id;

    // getMeshes
    if(inp.contains("Mesh")){
        json& Meshes = inp["Mesh"];
        int numMeshes = Meshes.size();
        if (Meshes.is_object())              addMesh(Meshes, sceneInput, curr_id,root);
        else for(int i=0; i < numMeshes; i++) addMesh(Meshes[i],sceneInput, curr_id,root);
    }
    type4_instStartID = curr_id;


    // getMeshInstances
    if(inp.contains("MeshInstance")){
        json& MeshInstances = inp["MeshInstance"];
        int numMeshInstances = MeshInstances.size();
        if (MeshInstances.is_object())              addInstance(MeshInstances, sceneInput, curr_id);
        else for(int i=0; i < numMeshInstances; i++) addInstance(MeshInstances[i],sceneInput, curr_id);
    }

    sceneInput.numObjects = sceneInput.objects.size();
    type5_planeStartID= curr_id;

    // getPlanes
    if(inp.contains("Plane")){
        json& planes = inp["Plane"];
        int numPlanes = planes.size();
        if (planes.is_object())              addPlane(planes, sceneInput, curr_id);
        else for(int i=0; i < numPlanes; i++) addPlane(planes[i],sceneInput, curr_id);
    }
    sceneInput.numPlanes = sceneInput.objects.size();

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

    if (ind[0] == ind[1] || ind[0] == ind[2] || ind[1] == ind[2] ) return;
    Triangle *tempt = new Triangle(curr_id,
                                sceneInput.Vertices[ind[0] - 1],
                                sceneInput.Vertices[ind[1] - 1],
                                sceneInput.Vertices[ind[2] - 1],
                                sceneInput.Materials[std::stoi(tri["Material"].get<std::string>()) - 1]);

    if (tri.contains("Transformations")) addInstance(tri["Transformations"].get<std::string>(), tempt, sceneInput);
    else                                   sceneInput.objects.push_back(tempt);

    if(PRINTINIT) std::cout << *dynamic_cast<Triangle*>(sceneInput.objects[curr_id]) << std::endl;
    curr_id++;
}

void Parser::addSphere(json s, SceneInput &sceneInput, uint32_t &curr_id)
{
    Sphere *temps = new Sphere(
                                curr_id,
                                sceneInput.Vertices[std::stoi(s["Center"].get<std::string>()) - 1],
                                std::stod(s["Radius"].get<std::string>()),
                                sceneInput.Materials[std::stoi(s["Material"].get<std::string>()) - 1]
                            );

    if (s.contains("Transformations")) addInstance(s["Transformations"].get<std::string>(), temps, sceneInput);
    else                                   sceneInput.objects.push_back(temps);

    if(PRINTINIT) std::cout << *dynamic_cast<Sphere*>(sceneInput.objects[curr_id])<< std::endl;

    curr_id++;
}

void Parser::addMesh(json mes, SceneInput &sceneInput, uint32_t &curr_id, std::string root)
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
            dataLine = root + mes["Faces"]["_plyFile"].get<std::string>();

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


        Mesh *tempm = new Mesh(curr_id,
                mes.contains("_shadingMode") ? mes["_shadingMode"].get<std::string>() : "flat",
                sceneInput.Materials[std::stoi(mes["Material"].get<std::string>()) - 1],
                dataLine,
                read_from_file,
                sceneInput.Vertices,
                numVerticesUntilNow);
        if (mes.contains("Transformations")) addInstance(mes["Transformations"].get<std::string>(), tempm, sceneInput);
        else                                   sceneInput.objects.push_back(tempm);

        curr_id++;
        if(PRINTINIT) std::cout <<  "Mesh " << tempm->_id << " has " << tempm->Faces.size() << " faces." << std::endl; //std::cout <<  temp_m << std::endl;
    }
}

void Parser::addInstance(std::string transformations, Object *original, SceneInput &sceneInput)
{
    sceneInput.objects.push_back(new Instance(
                                    original->_id,
                                    original,
                                    getTransFromStr(transformations, sceneInput.transforms),
                                    true
                                    ));
}

void Parser::addInstance(json s, SceneInput &sceneInput, uint32_t &curr_id)
{
    sceneInput.objects.push_back(new Instance(
                                    curr_id,
                                    getOriginalObjPtr(ObjectType::MESH,std::stoi(s["Transformations"].get<std::string>()), sceneInput.objects),
                                    getTransFromStr(s["Transformations"].get<std::string>(), sceneInput.transforms),
                                    false
                                    ));

    if(PRINTINIT) std::cout << *dynamic_cast<Instance*>(sceneInput.objects[curr_id])<< std::endl;

    curr_id++;
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


void Parser::addTranslation(json t, SceneInput &sceneInput)
{
    std::cout << "here" << std::endl;
    sceneInput.transforms.push_back(new Translate(Vertex(t["_data"])));
    if(PRINTINIT) std::cout <<  sceneInput.transforms[sceneInput.transforms.size()-1]<< std::endl;
}

void Parser::addScaling(json t, SceneInput &sceneInput)
{
    sceneInput.transforms.push_back(new Scale(Vertex(t["_data"])));
    if(PRINTINIT) std::cout <<  sceneInput.transforms[sceneInput.transforms.size()-1]<< std::endl;
}

void Parser::addRotation(json t, SceneInput &sceneInput)
{
    std::istringstream ss(t["_data"].get<std::string>());
    double angle, x, y, z;
    ss >> angle >>  x >> y >> z;
    angle = angle * M_PI / 180;
    if (ss.fail()) {
        throw std::invalid_argument("Invalid Vertex string: " + t["_data"].get<std::string>());
    }
    sceneInput.transforms.push_back(new Rotate(Ray(Vertex(0,0,0),Vec3r(x,y,z)), angle));
    if(PRINTINIT) std::cout <<  sceneInput.transforms[sceneInput.transforms.size()-1]<< std::endl;

}

Transformation *Parser::getTransFromStr(std::string transStr, std::vector<Transformation *>& transforms)
{
    std::istringstream ss(transStr);
    std::vector<Transformation *> temp;
    char transChar;
    int transID;
    while (ss >> transChar >> transID) {
        int startID;
        if (transChar == 't') startID =trans1_transStartID;
        else if (transChar == 's') startID =trans2_scaleStartID;
        else if (transChar == 'r') startID =trans3_rotStartID;
        else startID = 0;

        temp.push_back(transforms[startID+transID - 1]);
    }

    Transformation *transformation = new Composite(temp);
    return transformation;
}

Object *Parser::getOriginalObjPtr(ObjectType ot, int ot_id, std::vector<Object *>& objs)
{
    int startID;
    switch (ot)
    {
    case ObjectType::TRIANGLE:
        startID = type1_triStartID;
        break;
    case ObjectType::SPHERE:
        startID = type2_sphereStartID;
        break;
    case ObjectType::MESH:
        startID = type3_meshStartID;
        break;
    case ObjectType::INSTANCE:
        startID = type4_instStartID;
        break;
    case ObjectType::PLANE:
        startID = type5_planeStartID;
        break;
    default:
        startID = 0;

    }
    return objs[startID + ot_id];

}



