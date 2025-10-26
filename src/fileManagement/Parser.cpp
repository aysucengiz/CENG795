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
    getVertexData(inp["Scene"], sceneInput);
    getObjects(inp["Scene"]["Objects"], sceneInput);
}


void Parser::getCameras(json inp, SceneInput &sceneInput){
    json& Cameras = inp["Camera"];
    int numCameras = Cameras.size();
    if (Cameras.is_object())
    {
        Camera c(
                std::stoi(Cameras["_id"].get<std::string>()) - 1,
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
    if (inp.contains("AmbientLight"))
    {
        sceneInput.AmbientLight = Color(inp["AmbientLight"]);
    if(PRINTINIT) std::cout << "AmbientLight: " << sceneInput.AmbientLight << std::endl;
    }
    if (inp.contains("PointLight"))
    {
        json& pointLights = inp["PointLight"];
        if (pointLights.is_object())
        {
            std::cout <<"here" << std::endl;
            PointLight pl(
                        std::stoi(pointLights["_id"].get<std::string>()) - 1,
                        Vertex(pointLights["Position"]),
                        Color(pointLights["Intensity"])
                );
            sceneInput.PointLights.push_back(pl);
        }
        else
        {
            std::cout <<"why here" << std::endl;
            int numPointLights = pointLights.size();
            for(int i=0; i < numPointLights; i++){
                PointLight pl(
                        std::stoi(pointLights[i]["_id"].get<std::string>()) - 1,
                        Vertex(pointLights[i]["Position"]),
                        Color(pointLights[i]["Intensity"])
                );
                sceneInput.PointLights.push_back(pl);

                if(PRINTINIT) std::cout << pl << std::endl;
            }
        }
    }


}

void Parser::getMaterials(json inp, SceneInput &sceneInput){

    int numMaterials = inp.size();
    for(int i=0; i < numMaterials; i++){
        Material m(
                std::stoi(inp[i]["_id"].get<std::string>()) - 1,
                Color(inp[i]["AmbientReflectance"]),
                Color(inp[i]["DiffuseReflectance"]),
                Color(inp[i]["SpecularReflectance"]),
                inp[i].contains("MirrorReflectance") ? Color(inp[i]["MirrorReflectance"]) : Color(),
                std::stoi(inp[i]["PhongExponent"].get<std::string>())
        );
        sceneInput.Materials.push_back(m);

        if(PRINTINIT) std::cout << m << std::endl;
    }


}

void Parser::getVertexData(json inp, SceneInput &sceneInput){

    std::istringstream verticesStream(inp["VertexData"]["_data"].get<std::string>());
    real x,y,z;

    while (verticesStream >> x >> y >> z) {
        sceneInput.Vertices.push_back(CVertex(sceneInput.Vertices.size(),x, y, z));

        // if(PRINTINIT) std::cout << sceneInput.Vertices[sceneInput.Vertices.size()-1] << std::endl;
    }


}

void Parser::getObjects(json inp, SceneInput &sceneInput){
    // getTriangles
    std::vector<uint32_t> normal_counts;
    uint32_t curr_id = sceneInput.objects.size();
    normal_counts.resize(sceneInput.Vertices.size());

    if(inp.contains("Triangle")){
        json& Triangles = inp["Triangle"];
        uint32_t numTriangles = Triangles.size();
        for(int i=0; i < numTriangles; i++){

            std::istringstream ss(Triangles[i]["Indices"].get<std::string>());
            uint32_t ind[3];
            ss >> ind[0] >> ind[1] >> ind[2];
            if (ss.fail()) {
                throw std::invalid_argument("Invalid triangle indices string: " + Triangles[i]["Indices"].get<std::string>());
            }


            sceneInput.objects.push_back(new Triangle(
                                                    curr_id,
                                                    sceneInput.Vertices[ind[0] - 1],
                                                    sceneInput.Vertices[ind[1] - 1],
                                                    sceneInput.Vertices[ind[2] - 1],
                                                    sceneInput.Materials[std::stoi(Triangles[i]["Material"].get<std::string>()) - 1]));
            computeTriangleValues(*dynamic_cast<Triangle*>(sceneInput.objects[curr_id]), normal_counts);

            if(PRINTINIT) std::cout << *dynamic_cast<Triangle*>(sceneInput.objects[curr_id]) << std::endl;
            curr_id++;
        }
    }


    // getSpheres
    if(inp.contains("Sphere")){
        json& Spheres = inp["Sphere"];
        uint32_t numSpheres = Spheres.size();
        for(int i=0; i < numSpheres; i++){
            sceneInput.objects.push_back(new Sphere(
                                                curr_id,
                                                sceneInput.Vertices[std::stoi(Spheres[i]["Center"].get<std::string>()) - 1],
                                                std::stod(Spheres[i]["Radius"].get<std::string>()),
                                                sceneInput.Materials[std::stoi(Spheres[i]["Material"].get<std::string>()) - 1]
                                        ));

            if(PRINTINIT) std::cout << curr_id << *dynamic_cast<Sphere*>(sceneInput.objects[curr_id])<< std::endl;

            curr_id++;
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
                std::string dataLine;
                bool read_from_file;
                uint32_t numVerticesUntilNow = sceneInput.Vertices.size();
                if (Meshes[i]["Faces"].contains("_plyFile"))
                {
                    read_from_file = true;
                    dataLine = JSON_FILES + Meshes[i]["Faces"]["_plyFile"].get<std::string>();

                    happly::PLYData plyIn(dataLine);
                    std::vector<float> xs = plyIn.getElement("vertex").getProperty<float>("x");
                    std::vector<float> ys = plyIn.getElement("vertex").getProperty<float>("y");
                    std::vector<float> zs = plyIn.getElement("vertex").getProperty<float>("z");

                    for (int j=0; j<xs.size(); j++)
                    {
                        sceneInput.Vertices.push_back(CVertex(numVerticesUntilNow+j,xs[j],ys[j],zs[j]));
                        normal_counts.push_back(0);
                    }
                }
                else
                {
                    read_from_file = false;
                    dataLine = Meshes[i]["Faces"]["_data"].get<std::string>();
                }


                sceneInput.objects.push_back(new Mesh(curr_id,
                        Meshes[i]["_shadingMode"].get<std::string>(),
                        sceneInput.Materials[std::stoi(Meshes[i]["Material"].get<std::string>()) - 1],
                        dataLine,
                        read_from_file,
                        sceneInput.Vertices,
                        numVerticesUntilNow));
                int siz = dynamic_cast<Mesh*>(sceneInput.objects[curr_id])->Faces.size();
                Mesh *temp_m = dynamic_cast<Mesh*>(sceneInput.objects[curr_id]);
                for (int k = 0; k < siz; k++)
                {
                    computeTriangleValues(temp_m->Faces[k], normal_counts);
                }
                //if(PRINTINIT) std::cout <<  temp_m << std::endl;
            }


        }
    }


    computeVertexNormals(sceneInput,normal_counts);
}
void Parser::computeTriangleValues(Triangle &t, std::vector<uint32_t> &normal_counts)
{
    normal_counts[t.a.id]++;
    normal_counts[t.b.id]++;
    normal_counts[t.c.id]++;

}

void Parser::computeVertexNormals(SceneInput &scene, const std::vector<uint32_t> &normal_counts)
{
    uint32_t verNum = scene.VertexNormals.size();
    for(int i=0; i < verNum; i++)
    {
        if (normal_counts[i] > 0)
            scene.VertexNormals[i] /= normal_counts[i];
    }
}

