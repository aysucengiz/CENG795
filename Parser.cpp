//
// Created by aysu on 04.10.2025.
//

#include "Parser.h"


std::ostream& operator<<(std::ostream& os, const Color& c) {
    os << c.r << " " << c.g << " " << c.b;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vector& v) {
    os << v.i << " " << v.j << " " << v.k;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Vertex& v) {
    os << v.x << " " << v.y << " " << v.z;
    return os;
}

std::ostream& operator<<(std::ostream& os, const PointLight& pl) {
    os << "Point light:\n\t_id:" << pl._id
                   << "\n\tPosition:" << pl.Position
                   << "\n\tIntensity:" << pl.Intensity;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Material& m) {
    os << "Material:\n\t_id:" << m._id
       << "\n\ttype:" << m.materialType
       << "\n\tAmbientReflectance:" << m.AmbientReflectance
       << "\n\tDiffuseReflectance:" << m.DiffuseReflectance
       << "\n\tSpecularReflectance:" << m.SpecularReflectance
       << "\n\tPhongExponent:" << m.PhongExponent
       << "\n\tMirrorReflectance:" << m.MirrorReflectance;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Triangle& t) {
    os << "Triangle:\n\t_id:" << t._id
       << "\n\tindices:" << t.indices[0] << " " << t.indices[1] << " "  << t.indices[2]
       << "\n\tmaterial:" << t.material;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Sphere& s) {
    os << "Sphere:\n\t_id:" << s._id
              << "\n\tcenter:" << s.center
              << "\n\tradius:" << s.radius
              << "\n\tmaterial:" << s.material;
    return os;
}

std::ostream& operator<<(std::ostream& os, const Camera& c) {
    os << "Camera:\n\t_id:" << c._id
       << "\n\tPosition:" << c.Position
       << "\n\tGaze:" << c.Gaze
       << "\n\tUp:" << c.Up
       << "\n\tplane:" << c.l << " "<< c.r << " "<< c.t << " "<< c.b << " "<< c.z
       << "\n\tImageResolution:" << c.ImageResolution[0] << " "<< c.ImageResolution[1]
       << "\n\tImageName:" << c.ImageName;
    return os;
}


// copied from chatgpt because Im sick from writing the others
std::ostream& operator<<(std::ostream& os, const SceneInput& s) {
    os << "MaxRecursionDepth: " << s.MaxRecursionDepth << "\n";
    os << "BackgroundColor: " << s.BackgroundColor << "\n";
    os << "ShadowRayEpsilon: " << s.ShadowRayEpsilon << "\n";
    os << "IntersectionTestEpsilon: " << s.IntersectionTestEpsilon << "\n";

    os << "AmbientLight: " << s.AmbientLight << "\n";

    os << "Cameras:\n";
    for (const auto& cam : s.Cameras)
        os << "  " << cam << "\n";

    os << "PointLights:\n";
    for (const auto& pl : s.PointLights)
        os << "  " << pl << "\n";

    os << "Materials:\n";
    for (const auto& mat : s.Materials)
        os << "  " << mat << "\n";

    os << "Vertices:\n";
    for (const auto& v : s.Vertices)
        os << "  " << v << "\n";

    os << "Triangles:\n";
    for (const auto& t : s.Triangles)
        os << "  " << t << "\n";

    os << "Spheres:\n";
    for (const auto& sph : s.Spheres)
        os << "  " << sph << "\n";

    return os;
}



Json::Value Parser::getJsonDataFromFile(std::string inpFile){
    std::ifstream inp(inpFile, std::ifstream::binary);
    Json::Value input;
    inp >> input;
    return input;
}


void Parser::parseScene(std::string inpFile){
    Json::Value inp = getJsonDataFromFile(inpFile);
    if(PRINTINIT) std::cout << "Scene Input: " << std::endl;

    sceneInput.MaxRecursionDepth = std::stoi(inp["Scene"]["MaxRecursionDepth"].asString());
    if(PRINTINIT) std::cout << "MaxRecursionDepth: " << sceneInput.MaxRecursionDepth << std::endl;

    sceneInput.BackgroundColor = Color(inp["Scene"]["BackgroundColor"].asString());
    if(PRINTINIT) std::cout << "BackgroundColor: " << sceneInput.BackgroundColor << std::endl;

    sceneInput.ShadowRayEpsilon = std::stod(inp["Scene"]["ShadowRayEpsilon"].asString());
    if(PRINTINIT) std::cout << "ShadowRayEpsilon: " << sceneInput.ShadowRayEpsilon << std::endl;

    sceneInput.IntersectionTestEpsilon = std::stod(inp["Scene"]["IntersectionTestEpsilon"].asString());
    if(PRINTINIT) std::cout << "IntersectionTestEpsilon: " << sceneInput.IntersectionTestEpsilon << std::endl;

    getCameras(inp["Scene"]["Cameras"]);
    getLights(inp["Scene"]["Lights"]);
    getMaterials(inp["Scene"]["Materials"]["Material"]);
    getVertexData(inp["Scene"]["VertexData"]);
    getObjects(inp["Scene"]["Objects"]);
}


void Parser::getCameras(Json::Value inp){
    Json::Value& Cameras = inp["Camera"];
    //int numCameras = Cameras.size();
    Camera c(
            std::stoi(Cameras["_id"].asString()),
            Vertex(Cameras["Position"].asString()),
            Vector(Cameras["Gaze"].asString()),
            Vector(Cameras["Up"].asString()),
            Cameras["NearPlane"].asString(),
            Cameras["ImageResolution"].asString(),
            Cameras["ImageName"].asString()
    );
    sceneInput.Cameras.push_back(c);

    if(PRINTINIT) std::cout << c << std::endl;
}
void Parser::getLights(Json::Value inp){
    sceneInput.AmbientLight = Color(inp["AmbientLight"].asString());
    if(PRINTINIT) std::cout << "AmbientLight: " << sceneInput.AmbientLight << std::endl;

    Json::Value& pointLights = inp["PointLight"];
    int numPointLights = pointLights.size();
    for(int i=0; i < numPointLights; i++){
        PointLight pl(
                std::stoi(pointLights[i]["_id"].asString()),
                Vertex(pointLights[i]["Position"].asString()),
                Color(pointLights[i]["Intensity"].asString())
        );
        sceneInput.PointLights.push_back(pl);

        if(PRINTINIT) std::cout << pl << std::endl;
    }


}

void Parser::getMaterials(Json::Value inp){

    int numMaterials = inp.size();
    for(int i=0; i < numMaterials; i++){
        std::string typeString = "";
        Color mr;


        if (inp[i].isMember("_type")) {
            typeString = inp[i]["_type"].asString();
            mr = Color(inp[i]["MirrorReflectance"].asString());
        }

        Material m(
                std::stoi(inp[i]["_id"].asString()),
                Color(inp[i]["AmbientReflectance"].asString()),
                Color(inp[i]["DiffuseReflectance"].asString()),
                Color(inp[i]["SpecularReflectance"].asString()),
                std::stoi(inp[i]["PhongExponent"].asString()),
                typeString,
                mr
        );
        sceneInput.Materials.push_back(m);

        if(PRINTINIT) std::cout << m << std::endl;
    }


}

void Parser::getVertexData(Json::Value inp){
    std::istringstream verticesStream(inp.asString());
    double x,y,z;

    while (verticesStream >> x >> y >> z) {
        sceneInput.Vertices.push_back(Vertex(x, y, z));
        if(PRINTINIT) std::cout << x << " " << y <<" " <<  z << std::endl;
    }

}

void Parser::getObjects(Json::Value inp){
    // getTriangles
    Json::Value& Triangles = inp["Triangle"];
    int numTriangles = Triangles.size();
    for(int i=0; i < numTriangles; i++){
        Triangle t(
                std::stoi(Triangles[i]["_id"].asString()),
                Triangles[i]["Indices"].asString(),
                std::stoi(Triangles[i]["Material"].asString())
        );
        sceneInput.Triangles.push_back(t);

        if(PRINTINIT) std::cout << t << std::endl;
    }


    // getSpheres
    Json::Value& Spheres = inp["Sphere"];
    int numSpheres = Spheres.size();
    for(int i=0; i < numSpheres; i++){
        Sphere s(
                std::stoi(Spheres[i]["_id"].asString()),
                std::stoi(Spheres[i]["Center"].asString()),
                std::stod(Spheres[i]["Radius"].asString()),
                std::stoi(Spheres[i]["Material"].asString())
        );
        sceneInput.Spheres.push_back(s);

        if(PRINTINIT) std::cout << s << std::endl;
    }

}