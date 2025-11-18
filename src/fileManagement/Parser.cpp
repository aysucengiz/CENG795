//
// Created by aysu on 04.10.2025.
//

#include "Parser.h"
#include "../functions/overloads.h"

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

    if (inp["Scene"].contains("Transformations"))
    {
        getTransformations(inp["Scene"]["Transformations"], sceneInput);
    }

    getCameras(inp["Scene"]["Cameras"], sceneInput);
    getLights(inp["Scene"]["Lights"], sceneInput);
    getMaterials(inp["Scene"]["Materials"]["Material"], sceneInput);



    getVertexData(inp["Scene"], sceneInput);
    if (PRINTINIT) std::cout << "vertices done" << std::endl;
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
    Transformation *t;
    if (pointLights.contains("Transformations"))
    {
        t = getTransFromStr(pointLights["Transformations"].get<std::string>(),sceneInput.transforms);
        t->getNormalTransform();
    }
    else
    {
        t = new Composite();
    }
    PointLight pl(
            std::stoi(pointLights["_id"].get<std::string>()) - 1,
            (*t * Vec4r(Vertex(pointLights["Position"]))).getVertex(),
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
        sceneInput.Vertices.push_back(CVertex(sceneInput.Vertices.size(),Vertex(mapped[x],mapped[y],mapped[z]),Vec3r()));

        //if(PRINTINIT) std::cout << sceneInput.Vertices[sceneInput.Vertices.size()-1] << std::endl;
    }
}







void Parser::getObjects(json inp, SceneInput &sceneInput, std::string root){
    // getTriangles
    uint32_t curr_id = sceneInput.objects.size();
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
        if (PRINTINIT) std::cout << "mesh inst exists" << std::endl;
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


    for(int i=0; i < sceneInput.objects.size(); i++)
    {
        if (sceneInput.objects[i]->getObjectType() == ObjectType::INSTANCE)
        {
            dynamic_cast<Instance *>(sceneInput.objects[i])->forwardTrans->getNormalTransform();
            dynamic_cast<Instance *>(sceneInput.objects[i])->backwardTrans->getNormalTransform();
        }
    }
}







void Parser::getNearFromFovY(int FovY, double nearDistance, double aspect, std::array<double,4> &nearPlane)
{
    real t = tan(FovY*0.5/180*M_PI) *  nearDistance;
    nearPlane[0] = -t*aspect;
    nearPlane[1] =  t*aspect;
    nearPlane[2] = -t;
    nearPlane[3] =  t;
}

void Parser::addCamera(json Cameras, SceneInput &sceneInput)
{
    std::array<double,4> nearPlane;
    Vec3r Gaze;
    double nearDistance = std::stod(Cameras["NearDistance"].get<std::string>());
    std::istringstream ss(Cameras["ImageResolution"].get<std::string>());
    real width, height;
    ss >> width >> height;
    real aspect = width/height;

    if (Cameras.contains("_type") && Cameras["_type"].get<std::string>() == "lookAt")
    {
        // compute near plane here
        int FovY = std::stoi(Cameras["FovY"].get<std::string>());
        getNearFromFovY(FovY, nearDistance, aspect, nearPlane);
        Gaze = Vertex(Cameras["GazePoint"]) - Vertex(Cameras["Position"]);
    }
    else
    {
        std::istringstream ss(Cameras["NearPlane"].get<std::string>());
        ss >> nearPlane[0] >> nearPlane[1] >> nearPlane[2] >> nearPlane[3];
        Gaze = Vec3r(Cameras["Gaze"]);
    }

    Vertex pos = Vertex(Cameras["Position"]);
    Vec3r up = Vec3r(Cameras["Up"]);

    if (Cameras.contains("Transformations"))
    {
        Transformation *t = getTransFromStr(Cameras["Transformations"].get<std::string>(),sceneInput.transforms);
        t->getNormalTransform();
        if (Cameras["Transformations"].get<std::string>().find('s') != std::string::npos)
        {
            Scale scale = getScaleFromStr(Cameras["Transformations"].get<std::string>(),sceneInput.transforms);
            if (scale.y != 1) {
                nearDistance *= scale.y;
            }
            else if (scale.x != 1)
            {
                nearDistance *= scale.x;
            }
            else if (scale.z != 1)
            {
                nearDistance *= scale.z;
            }
        }

        pos = (*t *Vec4r(pos)).getVertex();
        Gaze = (*t *Vec4r(Vec3r(Gaze))).getVec3r();
        up = Vec3r(Cameras["Up"]);

        delete t;

    }

    Camera c(
               std::stoi(Cameras["_id"].get<std::string>()) - 1,
               pos, Gaze,  up,
               nearPlane,
               nearDistance,
               width, height,
               Cameras["ImageName"]
           );
    sceneInput.Cameras.push_back(c);
    if(PRINTINIT) std::cout << c << std::endl;
}

Scale Parser::getScaleFromStr(std::string transStr, std::vector<Transformation*>& transforms)
{
    std::istringstream ss(transStr);
    Scale s(1.0,1.0,1.0);
    char transChar;
    int transID;
    while (ss >> transChar >> transID) {
        if (transChar == 's')
        {
            int startID =trans2_scaleStartID;
            if (transforms[startID+transID - 1]->getTransformationType() == TransformationType::SCALE)
            {
                Scale *temp = dynamic_cast<Scale *>(transforms[startID+transID - 1]);
                s.x *= temp->x;
                s.y *= temp->y;
                s.z *= temp->z;
            }
        }
    }
    return s;
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
    Triangle *tempt = new Triangle(std::stoi(tri["_id"].get<std::string>()),
                                sceneInput.Vertices[ind[0] - 1],
                                sceneInput.Vertices[ind[1] - 1],
                                sceneInput.Vertices[ind[2] - 1],
                                sceneInput.Materials[std::stoi(tri["Material"].get<std::string>()) - 1]);

    if (tri.contains("Transformations")) addInstance(tri["Transformations"].get<std::string>(), tempt, sceneInput);
    else sceneInput.objects.push_back(tempt);
    if(PRINTINIT) std::cout << sceneInput.objects[curr_id] << std::endl;
    curr_id++;
}

void Parser::addSphere(json s, SceneInput &sceneInput, uint32_t &curr_id)
{
    Sphere *temps = new Sphere(
                                std::stoi(s["_id"].get<std::string>()),
                                sceneInput.Vertices[std::stoi(s["Center"].get<std::string>()) - 1],
                                std::stod(s["Radius"].get<std::string>()),
                                sceneInput.Materials[std::stoi(s["Material"].get<std::string>()) - 1]
                            );

    if (s.contains("Transformations"))addInstance(s["Transformations"].get<std::string>(), temps, sceneInput);
    else  sceneInput.objects.push_back(temps);
    if(PRINTINIT) std::cout << sceneInput.objects[curr_id]<< std::endl;


    curr_id++;
}

void Parser::addMesh(json mes, SceneInput &sceneInput, uint32_t &curr_id, std::string root)
{
    std::string sm = mes.contains("_shadingMode") ? mes["_shadingMode"].get<std::string>() : "flat";
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
            auto& vertexElem = plyIn.getElement("vertex");
            bool hasNormals = vertexElem.hasProperty("nx") && vertexElem.hasProperty("ny") && vertexElem.hasProperty("nz");

            std::vector<float> xs = vertexElem.getProperty<float>("x");
            std::vector<float> ys = vertexElem.getProperty<float>("y");
            std::vector<float> zs = vertexElem.getProperty<float>("z");
            if (hasNormals)
            {
                sm = "smooth";
                std::vector<float> nxs = vertexElem.getProperty<float>("nx");
                std::vector<float> nys = vertexElem.getProperty<float>("ny");
                std::vector<float> nzs = vertexElem.getProperty<float>("nz");

                for (int j=0; j<xs.size(); j++)
                {
                    sceneInput.Vertices.push_back(CVertex(numVerticesUntilNow+j,Vertex(xs[j],ys[j],zs[j]),Vec3r(nxs[j],nys[j],nzs[j])));
                }
            }
            else
            {
                for (int j=0; j<xs.size(); j++)
                {
                    sceneInput.Vertices.push_back(CVertex(numVerticesUntilNow+j,Vertex(xs[j],ys[j],zs[j]),Vec3r()));
                }
            }
        }
        else
        {

            read_from_file = false;
            dataLine = mes["Faces"]["_data"].get<std::string>();
        }


        Mesh *tempm = new Mesh(std::stoi(mes["_id"].get<std::string>()),
                sm,
                sceneInput.Materials[std::stoi(mes["Material"].get<std::string>()) - 1],
                dataLine,
                read_from_file,
                sceneInput.Vertices,
                true,
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
                                    original->material,
                                    true
                                    ));
    Instance *i = dynamic_cast<Instance*>(sceneInput.objects[sceneInput.objects.size()-1]);
    if (PRINTINIT) std::cout << "Instance " << i->_id << " of " << i->original->getObjectType() << std::endl;
}

void Parser::addInstance(json s, SceneInput &sceneInput, uint32_t &curr_id)
{
    Object *orig_obj = getOriginalObjPtr(ObjectType::MESH,std::stoi(s["_baseMeshId"].get<std::string>()), sceneInput.objects);
    // std::cout << "original: " << orig_obj->_id << std::endl;
    std::string resetTransform = s.contains("_resetTransform") ? s["_resetTransform"].get<std::string>() : "false";
    if (resetTransform == "true") while (orig_obj->getObjectType() == ObjectType::INSTANCE) orig_obj = dynamic_cast<Instance*>(orig_obj)->original;
    sceneInput.objects.push_back(new Instance(
                                    std::stoi(s["_id"].get<std::string>()),
                                    orig_obj,
                                    getTransFromStr(s["Transformations"].get<std::string>(), sceneInput.transforms),
                                    s.contains("Material") ? sceneInput.Materials[std::stoi(s["Material"].get<std::string>()) - 1] : orig_obj->material,
                                    false
                                    ));

    if(PRINTINIT) std::cout << "Instance " <<sceneInput.objects[curr_id]->_id << " of " << dynamic_cast<Instance*>(sceneInput.objects[curr_id])->original->_id<< std::endl;

    curr_id++;
}



void Parser::addPlane(json p, SceneInput &sceneInput, uint32_t &curr_id)
{
    Plane *tempp= new Plane(
                                std::stoi(p["_id"].get<std::string>()),
                                sceneInput.Vertices[std::stoi(p["Point"].get<std::string>()) - 1].v,
                                p["Normal"].get<std::string>(),
                                sceneInput.Materials[std::stoi(p["Material"].get<std::string>()) - 1]
                        );

    if (p.contains("Transformations"))addInstance(p["Transformations"].get<std::string>(), tempp, sceneInput);
    else sceneInput.objects.push_back(tempp);
    if(PRINTINIT) std::cout <<  sceneInput.objects[curr_id]<< std::endl;

    curr_id++;
}


void Parser::addTranslation(json t, SceneInput &sceneInput)
{
    Translate *temp = new Translate(Vertex(t["_data"]));
    sceneInput.transforms.push_back(temp);
    if(PRINTINIT) std::cout << sceneInput.transforms[sceneInput.transforms.size()-1]<< std::endl;
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
        // std::cout << transChar << " " << startID << std::endl;

        temp.push_back(transforms[startID+transID - 1]);
    }

    Transformation *transformation = new Composite(temp);
    return transformation;
}

Object *Parser::getOriginalObjPtr(ObjectType ot, int ot_id, std::deque<Object *>& objs)
{
    for(int startID = 0; startID < objs.size(); startID++)
    {
        if ((objs[startID]->getObjectType() == ot || (objs[startID]->getObjectType() == ObjectType::INSTANCE))
            && objs[startID]->_id == ot_id)
        {
            if (PRINTINIT) std::cout << objs.size() << " " << startID << std::endl;
            return objs[startID];
        }
    }
    return nullptr;


}



