//
// Created by aysu on 04.10.2025.
//

#include "Parser.h"
#include "../functions/overloads.h"
#include <cmath>

int getInt(json j){ return std::stoi(j.get<std::string>());}
real getReal(json j){ return (real) std::stod(j.get<std::string>());}
namespace Parser
{
    bool PRINTINIT = false;
    uint32_t type1_triStartID;
    uint32_t type2_sphereStartID;
    uint32_t type3_meshStartID;
    uint32_t type4_instStartID;
    uint32_t type5_planeStartID;

    uint32_t trans1_transStartID;
    uint32_t trans2_scaleStartID;
    uint32_t trans3_rotStartID;
    uint32_t trans4_compStartID;
}

json Parser::getJsonDataFromFile(std::string inpFile)
{
    std::ifstream f(inpFile);
    json j = json::parse(f);
    return j;
}


void Parser::parseScene(std::string inpFile, SceneInput& sceneInput, uint32_t maxDepth, real DefaultShadowEps,
                        real DefaultIntersEps, real print_init)
{
    PRINTINIT = print_init;
    json inp = getJsonDataFromFile(inpFile);
    std::string root = inpFile.substr(0, inpFile.find_last_of('/')) + "/";
    if (root.starts_with(inpFile)) root = "";
    // std::cout << "root: " << root << std::endl;

    if (PRINTINIT) std::cout << "Scene Input: " << std::endl;

    uint32_t MaxRecursionDepth = inp["Scene"].contains("MaxRecursionDepth")
                                       ? std::stoi(inp["Scene"]["MaxRecursionDepth"].get<std::string>())
                                       : maxDepth;
    if (PRINTINIT) std::cout << "MaxRecursionDepth: " << MaxRecursionDepth << std::endl;

    sceneInput.BackgroundColor = inp["Scene"].contains("BackgroundColor")
                                     ? Color(inp["Scene"]["BackgroundColor"].get<std::string>())
                                     : Color(0, 0, 0);
    if (PRINTINIT) std::cout << "BackgroundColor: " << sceneInput.BackgroundColor << std::endl;

    sceneInput.ShadowRayEpsilon = inp["Scene"].contains("ShadowRayEpsilon")
                                      ? std::stod(inp["Scene"]["ShadowRayEpsilon"].get<std::string>())
                                      : DefaultShadowEps;
    if (PRINTINIT) std::cout << "ShadowRayEpsilon: " << sceneInput.ShadowRayEpsilon << std::endl;

    sceneInput.IntersectionTestEpsilon = inp["Scene"].contains("IntersectionTestEpsilon")
                                             ? std::stod(inp["Scene"]["IntersectionTestEpsilon"].get<std::string>())
                                             : DefaultIntersEps;
    if (PRINTINIT) std::cout << "IntersectionTestEpsilon: " << sceneInput.IntersectionTestEpsilon << std::endl;

    if (inp["Scene"].contains("Transformations"))
    {
        getTransformations(inp["Scene"]["Transformations"], sceneInput);
    }

    getCameras(inp["Scene"]["Cameras"], sceneInput, MaxRecursionDepth); // TOOD: max ve min recursion depth

    BRDF *brdf_ptr = new Phong(0,true,false,false,1);
    sceneInput.BRDFs.push_back(brdf_ptr);

    if (inp["Scene"].contains("BRDFs"))
    {
        getBRDFs(inp["Scene"]["BRDFs"], sceneInput);
    }
    getMaterials(inp["Scene"]["Materials"]["Material"], sceneInput);


    getVertexData(inp["Scene"], sceneInput);
    if (PRINTINIT) std::cout << "vertices done" << std::endl;

    if (inp["Scene"].contains("TexCoordData"))
    {
        getTexCoords(inp["Scene"]["TexCoordData"], sceneInput);
    }
    if (inp["Scene"].contains("Textures"))
    {
        getTextures(inp["Scene"]["Textures"], sceneInput,root);
    }
    getLights(inp["Scene"]["Lights"], sceneInput);
    getObjects(inp["Scene"]["Objects"], sceneInput, root);
}


void Parser::getCameras(json inp, SceneInput& sceneInput, uint32_t MaxRecDepth)
{
    json& Cameras = inp["Camera"];
    int numCameras = Cameras.size();
    if (Cameras.is_object()) addCamera(Cameras, sceneInput,MaxRecDepth);
    else for (int i = 0; i < numCameras; i++) addCamera(Cameras[i], sceneInput,MaxRecDepth);
}

void Parser::getBRDFs(json brdfs, SceneInput& sceneInput)
{
    if (brdfs.contains("OriginalBlinnPhong"))
    {
        json& BRDFs = brdfs["OriginalBlinnPhong"];
        int numBRDFs = BRDFs.size();
        if (BRDFs.is_object()) addBRDF(BRDFs, sceneInput, "OriginalBlinnPhong");
        else for (int i = 0; i < numBRDFs; i++) addBRDF(BRDFs[i], sceneInput, "OriginalBlinnPhong");
    }

    if (brdfs.contains("OriginalPhong"))
    {
        json& BRDFs = brdfs["OriginalPhong"];
        int numBRDFs = BRDFs.size();
        if (BRDFs.is_object()) addBRDF(BRDFs, sceneInput, "OriginalPhong");
        else for (int i = 0; i < numBRDFs; i++) addBRDF(BRDFs[i], sceneInput, "OriginalPhong");
    }
    if (brdfs.contains("ModifiedBlinnPhong"))
    {
        json& BRDFs = brdfs["ModifiedBlinnPhong"];
        int numBRDFs = BRDFs.size();
        if (BRDFs.is_object()) addBRDF(BRDFs, sceneInput, "ModifiedBlinnPhong");
        else for (int i = 0; i < numBRDFs; i++) addBRDF(BRDFs[i], sceneInput, "ModifiedBlinnPhong");
    }
    if (brdfs.contains("ModifiedPhong"))
    {
        json& BRDFs = brdfs["ModifiedPhong"];
        int numBRDFs = BRDFs.size();
        if (BRDFs.is_object()) addBRDF(BRDFs, sceneInput, "ModifiedPhong");
        else for (int i = 0; i < numBRDFs; i++) addBRDF(BRDFs[i], sceneInput, "ModifiedPhong");
    }

    if (brdfs.contains("TorranceSparrow"))
    {
        json& BRDFs = brdfs["TorranceSparrow"];
        int numBRDFs = BRDFs.size();
        if (BRDFs.is_object()) addBRDF(BRDFs, sceneInput, "TorranceSparrow");
        else for (int i = 0; i < numBRDFs; i++) addBRDF(BRDFs[i], sceneInput, "TorranceSparrow");
    }
}


void Parser::getTextures(json &inp, SceneInput& sceneInput, std::string root)
{
    getImages(inp, sceneInput, root);
    getTextureMaps(inp, sceneInput);
}

void Parser::getImages(json &inp, SceneInput& sceneInput, std::string root)
{

    if (inp.contains("Images"))
    {
        json& Images = inp["Images"]["Image"];
        uint32_t numImages = Images.size();
        if (Images.is_object()) addImage(Images, sceneInput, root);
        else for (int i = 0; i < numImages; i++) addImage(Images[i], sceneInput, root);
    }

}

void Parser::addImage(json s, SceneInput& sceneInput, std::string root)
{
    Image *temp = new Image(std::stoi(s["_id"].get<std::string>()), root + s["_data"].get<std::string>());
    sceneInput.images.push_back(temp);
    if (PRINTINIT)
        std::cout << sceneInput.images[sceneInput.images.size()-1] << std::endl;
}



void Parser::getTextureMaps(json inp, SceneInput& sceneInput)
{
    if (inp.contains("TextureMap"))
    {
        json& TextureMaps = inp["TextureMap"];
        uint32_t numTextureMaps = TextureMaps.size();
        if (TextureMaps.is_object()) addTextureMap(TextureMaps, sceneInput);
        else for (int i = 0; i < numTextureMaps; i++) addTextureMap(TextureMaps[i], sceneInput);
    }
}

void Parser::addTextureMap(json s, SceneInput& sceneInput)
{
    TextureType t = getTextureType(s["_type"].get<std::string>());
    DecalMode dm = getDecalMode(s["DecalMode"]);
    real normalizer = s.contains("Normalizer") ? getReal(s["Normalizer"]) : 255.0;
    Texture *temp = nullptr;
    int id = std::stoi(s["_id"].get<std::string>());
    if (t == TextureType::IMAGE)
    {
        Interpolation interp = s.contains("Interpolation") ?  getInterpolation(s["Interpolation"].get<std::string>()) : Interpolation::NEAREST;
        temp = new ImageTexture(id, dm, getImageFromId(getInt(s["ImageId"]),sceneInput), interp,normalizer);
    }
    else if (t == TextureType::PERLIN)
    {
        temp = new PerlinTexture(id, dm, getConversionFunc(s["NoiseConversion"]),
            s.contains("NoiseScale") ? getReal(s["NoiseScale"]) : 1,
            s.contains("NumOctaves") ? getInt(s["NumOctaves"]) : 1);
    }
    else if (t==TextureType::CHECKERBOARD)
    {
        temp = new CheckerTexture(id, dm, Color(s["BlackColor"]), Color(s["WhiteColor"]),
            getReal(s["Scale"]), getReal(s["Offset"]));
    }

    if (s.contains("BumpFactor")) temp->bumpFactor = getReal(s["BumpFactor"]);
    sceneInput.textures.push_back(temp);
    if ( temp->decalMode == DecalMode::REPLACE_BACKGROUND) sceneInput.BackgroundTexture = temp;
    if (PRINTINIT) std::cout << temp << std::endl;
}


void Parser::addLight(json light, SceneInput& sceneInput, std::string type)
{
    std::shared_ptr<Transformation> t;
    if (light.contains("Transformations"))
    {
        t = getTransFromStr(light["Transformations"].get<std::string>(), sceneInput.transforms)->clone();
    }
    else
    {
        t = std::make_shared<Composite>();
    }
    t->getNormalTransform();

    Light* pl;

    if (type == "Area")
    {
        pl = new AreaLight(
            std::stoi(light["_id"].get<std::string>()) - 1,
            (*t * Vec4r(Vertex(light["Position"]))).getVertex(),
            Color(light["Radiance"]),
            (t->normalTransform * Vec4r(Vec3r(light["Normal"]))).getVec3r(),
            std::stod(light["Size"].get<std::string>())
        );
    }
    else if (type == "Spot")
    {
        pl = new SpotLight(
            std::stoi(light["_id"].get<std::string>()) - 1,
            (*t * Vec4r(Vertex(light["Position"]))).getVertex(),
            Color(light["Intensity"]),
            (*t * Vec4r(Vec3r(light["Direction"]))).getVec3r(), // TODO: normal transform mu ??
            getReal(light["CoverageAngle"]),
            light.contains("FalloffAngle") ?getReal(light["FalloffAngle"]): getReal(light["FallOffAngle"])
        );
    }
    else if (type == "Directional")
    {
        pl = new DirectionalLight(
            std::stoi(light["_id"].get<std::string>()) - 1,
            Color(light["Radiance"]),
            Vec3r(light["Direction"]));
    }
    else if (type == "Spherical")
    {
        std::string tstr = light.contains("_type") ? light["_type"] : "latlong";
        TextureLightType tip = tstr == "latlong" ? TextureLightType::LATLONG : TextureLightType::PROBE;

        std::string sstr = light.contains("Sampler") ? light["Sampler"] : "cosine";
        Sampler samp = sstr == "uniform" ? Sampler::UNIFORM : Sampler::COSINE;

        pl = new TextureLight(
            std::stoi(light["_id"].get<std::string>()) - 1,
            getImageFromId(getInt(light["ImageId"]),sceneInput),
            samp, tip);

        sceneInput.BackgroundLight = dynamic_cast<TextureLight*>(pl);
    }
    else
    {
        pl = new Light(
            std::stoi(light["_id"].get<std::string>()) - 1,
            (*t * Vec4r(Vertex(light["Position"]))).getVertex(),
            Color(light["Intensity"])
        );
    }

    sceneInput.PointLights.push_back(pl);

    // if (PRINTINIT) std::cout << *pl << std::endl;
}


void Parser::getLights(json inp, SceneInput& sceneInput)
{
    sceneInput.AmbientLight = inp.contains("AmbientLight") ? Color(inp["AmbientLight"]) : Color();
    if (PRINTINIT) std::cout << "AmbientLight: " << sceneInput.AmbientLight << std::endl;

    if (inp.contains("PointLight"))
    {
        json& pointLights = inp["PointLight"];
        int numPointLights = pointLights.size();
        if (pointLights.is_object()) addLight(pointLights, sceneInput, "Point");
        else for (int i = 0; i < numPointLights; i++) addLight(pointLights[i], sceneInput, "Point");
    }
    if (inp.contains("AreaLight"))
    {
        json& pointLights = inp["AreaLight"];
        int numPointLights = pointLights.size();
        if (pointLights.is_object()) addLight(pointLights, sceneInput, "Area");
        else for (int i = 0; i < numPointLights; i++) addLight(pointLights[i], sceneInput, "Area");
    }
    if (inp.contains("SpotLight"))
    {
        json& spotLights = inp["SpotLight"];
        int numSpotLights = spotLights.size();
        if (spotLights.is_object()) addLight(spotLights, sceneInput, "Spot");
        else for (int i = 0; i < numSpotLights; i++) addLight(spotLights[i], sceneInput, "Spot");
    }
    if (inp.contains("DirectionalLight"))
    {
        json& directionalLights = inp["DirectionalLight"];
        int numdirectionalLights = directionalLights.size();
        if (directionalLights.is_object()) addLight(directionalLights, sceneInput, "Directional");
        else for (int i = 0; i < numdirectionalLights; i++) addLight(directionalLights[i], sceneInput, "Directional");
    }
    if (inp.contains("SphericalDirectionalLight"))
    {
        json& directionalLights = inp["SphericalDirectionalLight"];
        int numdirectionalLights = directionalLights.size();
        if (directionalLights.is_object()) addLight(directionalLights, sceneInput, "Spherical");
        else for (int i = 0; i < numdirectionalLights; i++) addLight(directionalLights[i], sceneInput, "Spherical");
    }
}


void Parser::getMaterials(json inp, SceneInput& sceneInput)
{
    int numMaterials = inp.size();
    if (inp.is_object()) addMaterial(inp, sceneInput);
    else for (int i = 0; i < numMaterials; i++) addMaterial(inp[i], sceneInput);
}

void Parser::getTransformations(json inp, SceneInput& sceneInput)
{
    // getTriangles
    trans1_transStartID = sceneInput.transforms.size();
    if (inp.contains("Translation"))
    {
        json& Translations = inp["Translation"];
        uint32_t numTranslation = Translations.size();
        if (Translations.is_object()) addTranslation(Translations, sceneInput);
        else for (int i = 0; i < numTranslation; i++) addTranslation(Translations[i], sceneInput);
    }

    // getSpheres
    trans2_scaleStartID = sceneInput.transforms.size();
    if (inp.contains("Scaling"))
    {
        json& Scalings = inp["Scaling"];
        uint32_t numScalings = Scalings.size();
        if (Scalings.is_object()) addScaling(Scalings, sceneInput);
        else for (int i = 0; i < numScalings; i++) addScaling(Scalings[i], sceneInput);
    }

    // getMeshes
    trans3_rotStartID = sceneInput.transforms.size();
    if (inp.contains("Rotation"))
    {
        json& Rotations = inp["Rotation"];
        int numRotations = Rotations.size();
        if (Rotations.is_object()) addRotation(Rotations, sceneInput);
        else for (int i = 0; i < numRotations; i++) addRotation(Rotations[i], sceneInput);
    }
    trans4_compStartID = sceneInput.transforms.size();
    if (inp.contains("Composite"))
    {
        json& Composites = inp["Composite"];
        int numComposites = Composites.size();
        if (Composites.is_object()) addComposite(Composites, sceneInput);
        else for (int i = 0; i < numComposites; i++) addComposite(Composites[i], sceneInput);
    }
}


void Parser::getVertexData(json inp, SceneInput& sceneInput)
{
    if (!inp.contains("VertexData")) return;
    std::istringstream verticesStream(inp["VertexData"]["_data"].get<std::string>());
    std::string ori = "xyz";
    if (inp["VertexData"].contains("_type")) ori = inp["VertexData"]["_type"].get<std::string>();
    int x = ori.find('x');
    int y = ori.find('y');
    int z = ori.find('z');
    real mapped[3];
    while (verticesStream >> mapped[0] >> mapped[1] >> mapped[2])
    {
        sceneInput.Vertices.push_back(CVertex(sceneInput.Vertices.size(), Vertex(mapped[x], mapped[y], mapped[z]),
                                              Vec3r()));

        //if(PRINTINIT) std::cout << sceneInput.Vertices[sceneInput.Vertices.size()-1] << std::endl;
    }
}


void Parser::getObjects(json inp, SceneInput& sceneInput, std::string root)
{
    // getTriangles
    uint32_t curr_id = sceneInput.objects.size();
    type1_triStartID = curr_id;
    if (inp.contains("Triangle"))
    {
        json& Triangles = inp["Triangle"];
        uint32_t numTriangles = Triangles.size();
        if (Triangles.is_object()) addTriangle(Triangles, sceneInput, curr_id);
        else for (int i = 0; i < numTriangles; i++) addTriangle(Triangles[i], sceneInput, curr_id);
    }
    type2_sphereStartID = curr_id;

    // getSpheres
    if (inp.contains("Sphere"))
    {
        json& Spheres = inp["Sphere"];
        uint32_t numSpheres = Spheres.size();
        if (Spheres.is_object()) addSphere(Spheres, sceneInput, curr_id);
        else for (int i = 0; i < numSpheres; i++) addSphere(Spheres[i], sceneInput, curr_id);
    }
    type3_meshStartID = curr_id;

    // getMeshes
    // std::cout << "mesh" << std::endl;
    if (inp.contains("Mesh"))
    {
        json& Meshes = inp["Mesh"];
        int numMeshes = Meshes.size();
        if (Meshes.is_object()) addMesh(Meshes, sceneInput, curr_id, root);
        else for (int i = 0; i < numMeshes; i++) addMesh(Meshes[i], sceneInput, curr_id, root);
    }
    type4_instStartID = curr_id;
    // std::cout <<"---------------------mesh done------------------" << std::endl;


    // getMeshInstances
    if (inp.contains("MeshInstance"))
    {
        if (PRINTINIT) std::cout << "mesh inst exists" << std::endl;
        json& MeshInstances = inp["MeshInstance"];
        int numMeshInstances = MeshInstances.size();
        if (MeshInstances.is_object()) addInstance(MeshInstances, sceneInput, curr_id);
        else for (int i = 0; i < numMeshInstances; i++) addInstance(MeshInstances[i], sceneInput, curr_id);
    }

    sceneInput.numObjects = sceneInput.objects.size();
    type5_planeStartID = curr_id;

    // getPlanes
    if (inp.contains("Plane"))
    {
        json& planes = inp["Plane"];
        int numPlanes = planes.size();
        if (planes.is_object()) addPlane(planes, sceneInput, curr_id);
        else for (int i = 0; i < numPlanes; i++) addPlane(planes[i], sceneInput, curr_id);
    }
    sceneInput.numPlanes = sceneInput.objects.size();


    for (int i = 0; i < sceneInput.Vertices.size(); i++)
    {
        sceneInput.Vertices[i].n = sceneInput.Vertices[i].n.normalize();
    }
}


void Parser::getNearFromFovY(real FovY, double nearDistance, double aspect, std::array<double, 4>& nearPlane)
{
    real t = tan(FovY * 0.5 / 180 * M_PI) * nearDistance;
    nearPlane[0] = -t * aspect;
    nearPlane[1] = t * aspect;
    nearPlane[2] = -t;
    nearPlane[3] = t;
}

void Parser::addCamera(json Cameras, SceneInput& sceneInput, uint32_t maxrecdep)
{
    std::array<double, 4> nearPlane;
    Vec3r Gaze;
    double nearDistance = std::stod(Cameras["NearDistance"].get<std::string>());
    std::istringstream ss(Cameras["ImageResolution"].get<std::string>());
    real width, height;
    ss >> width >> height;
    real aspect = width / height;
    TraceType ttype = TraceType::RAY;
    PathTracer* path = nullptr;
    uint32_t maxdep = Cameras.contains("MaxRecursionDepth") ? getInt(Cameras["MaxRecursionDepth"]) : maxrecdep;
    uint32_t mindep = Cameras.contains("MinRecursionDepth") ? getInt(Cameras["MinRecursionDepth"]) : 0;

    if (Cameras.contains("Renderer") && Cameras["Renderer"].get<std::string>() == "PathTracing")
    {
        ttype = TraceType::PATH;
        path = new PathTracer();
        if(Cameras.contains("SplittingFactor")) path->splitting_factor = getInt(Cameras["SplittingFactor"]);
        if(Cameras.contains("RendererParams")) getRendererParams(Cameras["RendererParams"], path);

    }

    if (Cameras.contains("_type") && Cameras["_type"].get<std::string>() == "lookAt")
    {
        // compute near plane here
        real FovY = std::stod(Cameras["FovY"].get<std::string>());
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
        std::shared_ptr<Transformation> t = getTransFromStr(Cameras["Transformations"].get<std::string>(),
                                                            sceneInput.transforms);
        t->getNormalTransform();
        if (Cameras["Transformations"].get<std::string>().find('s') != std::string::npos)
        {
            Scale scale = getScaleFromStr(Cameras["Transformations"].get<std::string>(), sceneInput.transforms);
            if (scale.y != 1)
            {
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

        pos = (*t * Vec4r(pos)).getVertex();
        Gaze = (*t * Vec4r(Vec3r(Gaze))).getVec3r();
        up = Vec3r(Cameras["Up"]);
    }

    std::vector<ToneMap> tonemaps;
    if (Cameras.contains("Tonemap"))
    {
        std::string imname = Cameras["ImageName"].get<std::string>();
        getToneMaps(Cameras, tonemaps, imname);
    }

    sceneInput.Cameras.push_back(Camera(
        std::stoi(Cameras["_id"].get<std::string>()) - 1,
        pos, Gaze, up,
        nearPlane,
        nearDistance,
        width, height,
        Cameras["ImageName"],
        Cameras.contains("NumSamples") ? std::stoi(Cameras["NumSamples"].get<std::string>()) : 1,
        Cameras.contains("FocusDistance") ? std::stod(Cameras["FocusDistance"].get<std::string>()) : 0.0,
        Cameras.contains("ApertureSize") ? std::stod(Cameras["ApertureSize"].get<std::string>()) : 0.0,
        sceneInput.sampling_type,
        tonemaps,
        Cameras.contains("_handedness") ? Cameras["_handedness"].get<std::string>() : "right",
        path,
        mindep,
        maxdep
    ));
    if (PRINTINIT) std::cout << sceneInput.Cameras[sceneInput.Cameras.size()] << std::endl;
}


void Parser::getToneMaps(json inp, std::vector<ToneMap>& tonemaps, std::string &imname)
{
    json& Tonemaps = inp["Tonemap"];
    uint32_t numTonemaps = Tonemaps.size();
    if (Tonemaps.is_object()) addTonemap(Tonemaps, tonemaps, imname);
    else for (int i = 0; i < numTonemaps; i++) addTonemap(Tonemaps[i], tonemaps, imname);
}

void Parser::addTonemap(json ton, std::vector<ToneMap>& tonemaps, std::string &imname)
{
    TMOType tmo = getTMOType(ton["TMO"].get<std::string>());

    std::istringstream ss(ton["TMOOptions"].get<std::string>());
    std::array<real, 2> options;
    ss >> options[0] >> options[1];

    tonemaps.push_back(ToneMap(imname,
                               ton["Extension"].get<std::string>(),
                               tmo,
                               options,
                               getReal(ton["Gamma"]),
                               getReal(ton["Saturation"])));
}

Scale Parser::getScaleFromStr(std::string transStr, std::vector<std::shared_ptr<Transformation>>& transforms)
{
    std::istringstream ss(transStr);
    Scale s(1.0, 1.0, 1.0);
    char transChar;
    int transID;
    while (ss >> transChar >> transID)
    {
        if (transChar == 's')
        {
            int startID = trans2_scaleStartID;
            if (transforms[startID + transID - 1]->getTransformationType() == TransformationType::SCALE)
            {
                Scale* temp = dynamic_cast<Scale*>(transforms[startID + transID - 1].get());
                s.x *= temp->x;
                s.y *= temp->y;
                s.z *= temp->z;
            }
        }
    }
    return s;
}


void Parser::addMaterial(json inp, SceneInput& sceneInput)
{
    std::string type = inp.contains("_type") ? inp["_type"].get<std::string>() : "";
    BRDF *brdf_ptr = inp.contains("_BRDF") ? getBRDFFromID(getInt(inp["_BRDF"]),sceneInput.BRDFs) : sceneInput.BRDFs[0];
    uint32_t phong = inp.contains("PhongExponent") ? getInt(inp["PhongExponent"]) : brdf_ptr->exponent;
    Material m(
        std::stoi(inp["_id"].get<std::string>()) - 1,
        Color(inp["AmbientReflectance"]),
        Color(inp["DiffuseReflectance"]),
        Color(inp["SpecularReflectance"]),
        phong,
        type,
        (type == "mirror" && inp.contains("MirrorReflectance")) ? Color(inp["MirrorReflectance"]) : Color(),
        inp.contains("AbsorptionCoefficient") ? Color(inp["AbsorptionCoefficient"]) : Color(),
        inp.contains("RefractionIndex") ? std::stod(inp["RefractionIndex"].get<std::string>()) : 0.0,
        inp.contains("AbsorptionIndex") ? std::stod(inp["AbsorptionIndex"].get<std::string>()) : 0.0,
        inp.contains("Roughness") ? std::stod(inp["Roughness"].get<std::string>()) : 0.0,
        inp.contains("_degamma") ? inp["_degamma"].get<std::string>() : "false",
        brdf_ptr);
    sceneInput.Materials.push_back(m);

    if (PRINTINIT) std::cout << m << std::endl;
}


void Parser::addBRDF(json inp, SceneInput& sceneInput, std::string type)
{
    BRDF *brdf_ptr = nullptr;
    real exponent = getReal(inp["Exponent"]);
    uint32_t id = getInt(inp["_id"]);

    if (type == "OriginalBlinnPhong")
    {
        brdf_ptr = new Phong(id,true,false,false,exponent);
    }
    else if (type == "OriginalPhong")
    {
        brdf_ptr = new Phong(id,false,false,false,exponent);
    }
    else if (type == "ModifiedBlinnPhong")
    {
        brdf_ptr = new Phong(id,true,true,
                        inp.contains("_normalized") ? inp["_normalized"] : "false"
                        ,exponent);
    }
    else if (type == "ModifiedPhong")
    {
        brdf_ptr = new Phong(id,false,true,
                        inp.contains("_normalized") ? inp["_normalized"] : "false"
                        ,exponent);
    }
    else if (type == "TorranceSparrow")
    {
        brdf_ptr = new BRDF_TorranceSparrow(id,inp.contains("_kdfresnel") ? inp["_kdfresnel"] : "false"
                        ,exponent);
    }

    sceneInput.BRDFs.push_back(brdf_ptr);

    // if (PRINTINIT) std::cout << brdf_ptr << std::endl;
}

void Parser::addTriangle(json tri, SceneInput& sceneInput, uint32_t& curr_id)
{
    std::istringstream ss(tri["Indices"].get<std::string>());
    uint32_t ind[3];
    ss >> ind[0] >> ind[1] >> ind[2];
    if (ss.fail())
    {
        throw std::invalid_argument("Invalid triangle indices string: " + tri["Indices"].get<std::string>());
    }

    if (ind[0] == ind[1] || ind[0] == ind[2] || ind[1] == ind[2]) return;
    Triangle* tempt = new Triangle(std::stoi(tri["_id"].get<std::string>()),
                                   sceneInput.Vertices[ind[0] - 1],
                                   sceneInput.Vertices[ind[1] - 1],
                                   sceneInput.Vertices[ind[2] - 1],
                                   sceneInput.Materials[std::stoi(tri["Material"].get<std::string>()) - 1],
                                   tri.contains("Textures")
                                       ? getTexturesFromStr(tri["Textures"], sceneInput)
                                       : std::vector<Texture*>());

    if (tri.contains("Transformations"))
        addInstance(tri["Transformations"].get<std::string>(), tempt,
                    sceneInput, Vec3r(0.0, 0.0, 0.0));
    else sceneInput.objects.push_back(tempt);
    if (PRINTINIT) std::cout << sceneInput.objects[curr_id] << std::endl;
    curr_id++;
}

void Parser::addSphere(json s, SceneInput& sceneInput, uint32_t& curr_id)
{
    if (s.contains("Indices")) addInstance(s.get<std::string>(), sceneInput, curr_id);
    Sphere* temps = new Sphere(
        std::stoi(s["_id"].get<std::string>()),
        sceneInput.Vertices[std::stoi(s["Center"].get<std::string>()) - 1],
        std::stod(s["Radius"].get<std::string>()),
        sceneInput.Materials[std::stoi(s["Material"].get<std::string>()) - 1],
        s.contains("Textures") ? getTexturesFromStr(s["Textures"], sceneInput) : std::vector<Texture*>()
    );

    if (s.contains("Transformations"))
        addInstance(s["Transformations"].get<std::string>(), temps, sceneInput,
                    s.contains("MotionBlur") ? Vec3r(s["MotionBlur"]) : Vec3r());
    else if (s.contains("MotionBlur"))addInstance("", temps, sceneInput, Vec3r(s["MotionBlur"]));
    else sceneInput.objects.push_back(temps);
    if (PRINTINIT) std::cout << sceneInput.objects[curr_id] << std::endl;


    curr_id++;
}

void Parser::addMesh(json mes, SceneInput& sceneInput, uint32_t& curr_id, std::string root)
{
    // std::cout << "will add a new mesh" << std::endl;
    std::string sm = mes.contains("_shadingMode") ? mes["_shadingMode"].get<std::string>() : "flat";
    std::string typeString = "";
    int vertex_offset = mes["Faces"].contains("_vertexOffset") ? getInt(mes["Faces"]["_vertexOffset"]) : 0;
    int texture_offset = mes["Faces"].contains("_textureOffset") ? getInt(mes["Faces"]["_textureOffset"])  : 0;
    if (sceneInput.Materials[std::stoi(mes["Material"].get<std::string>()) - 1].materialType != MaterialType::NONE)
    {
        std::string dataLine;
        bool read_from_file;
        int numVerticesUntilNow = sceneInput.Vertices.size();
        if (mes["Faces"].contains("_plyFile"))
        {
            read_from_file = true;
            dataLine = root + mes["Faces"]["_plyFile"].get<std::string>();

            happly::PLYData plyIn(dataLine);
            auto& vertexElem = plyIn.getElement("vertex");
            bool hasNormals = vertexElem.hasProperty("nx") && vertexElem.hasProperty("ny") && vertexElem.
            hasProperty("nz");

            std::vector<float> xs = vertexElem.getProperty<float>("x");
            std::vector<float> ys = vertexElem.getProperty<float>("y");
            std::vector<float> zs = vertexElem.getProperty<float>("z");


            if (hasNormals)
            {
                sm = "smooth";
                std::vector<float> nxs = vertexElem.getProperty<float>("nx");
                std::vector<float> nys = vertexElem.getProperty<float>("ny");
                std::vector<float> nzs = vertexElem.getProperty<float>("nz");

                for (int j = 0; j < xs.size(); j++)
                {
                    sceneInput.Vertices.push_back(CVertex(numVerticesUntilNow + j, Vertex(xs[j], ys[j], zs[j]),
                                                          Vec3r(nxs[j], nys[j], nzs[j])));
                }
            }
            else
            {
                for (int j = 0; j < xs.size(); j++)
                {
                    sceneInput.Vertices.push_back(
                        CVertex(numVerticesUntilNow + j, Vertex(xs[j], ys[j], zs[j]), Vec3r()));
                }
            }
            bool hasTexCoords = vertexElem.hasProperty("u") && vertexElem.hasProperty("v");
            if (hasTexCoords)
            {
                std::vector<float> us = vertexElem.getProperty<float>("u");
                std::vector<float> vs = vertexElem.getProperty<float>("v");
                for (int j = numVerticesUntilNow ; j < sceneInput.Vertices.size(); j++)
                {
                    // std::cout << us[j-numVerticesUntilNow] << " " << vs[j-numVerticesUntilNow] << std::endl;
                    sceneInput.Vertices[j].t = Texel(us[j-numVerticesUntilNow], vs[j-numVerticesUntilNow]);
                    // std::cout << sceneInput.Vertices[j].t.u << " " << sceneInput.Vertices[j].t.v << std::endl;
                }
            }
        }
        else
        {
            read_from_file = false;
            dataLine = mes["Faces"]["_data"].get<std::string>();
        }

        Mesh* tempm = new Mesh(std::stoi(mes["_id"].get<std::string>()),
                               sm,
                               sceneInput.Materials[std::stoi(
                                   mes["Material"].get<std::string>()) - 1],
                               dataLine,
                               read_from_file,
                               sceneInput.Vertices, sceneInput.pt, sceneInput.MaxObjCount,
                               mes.contains("Textures")
                                   ? getTexturesFromStr(mes["Textures"], sceneInput)
                                   : std::vector<Texture*>(),
                               true,
                               numVerticesUntilNow, true, vertex_offset, texture_offset);
        if (mes.contains("Transformations"))
            addInstance(mes["Transformations"].get<std::string>(), tempm, sceneInput,
                        mes.contains("MotionBlur") ? Vec3r(mes["MotionBlur"]) : Vec3r(0.0, 0.0, 0.0));
        else if (mes.contains("MotionBlur"))addInstance("", tempm, sceneInput, Vec3r(mes["MotionBlur"]));
        else sceneInput.objects.push_back(tempm);

        curr_id++;
        if (PRINTINIT) std::cout << *tempm << std::endl;
        //std::cout <<  temp_m << std::endl;
    }
}

void Parser::addInstance(std::string transformations, Object* original, SceneInput& sceneInput,
                         Vec3r motion)
{
    std::vector<Texture*> textures;
    if (original->NormalTexture) textures.push_back(original->NormalTexture);
    if (original->DiffuseTexture) textures.push_back(original->DiffuseTexture);
    if (original->SpecularTexture) textures.push_back(original->SpecularTexture);
    if (original->AllTexture) textures.push_back(original->AllTexture);
    sceneInput.objects.push_back(new Instance(
        original->_id,
        original,
        getTransFromStr(transformations, sceneInput.transforms),
        original->material,
        motion,
        textures,
        true
    ));
    if (PRINTINIT) std::cout << sceneInput.objects[sceneInput.objects.size() - 1] << std::endl;
}

void Parser::addInstance(json s, SceneInput& sceneInput, uint32_t& curr_id)
{
    Object* orig_obj = getOriginalObjPtr(ObjectType::MESH,
                                         std::stoi(s["_baseMeshId"].get<std::string>()),
                                         sceneInput.objects);
    // std::cout << "original: " << orig_obj->_id << std::endl;
    std::string resetTransform = s.contains("_resetTransform") ? s["_resetTransform"].get<std::string>() : "false";
    if (resetTransform == "true")
    {
        while (orig_obj->getObjectType() == ObjectType::INSTANCE)
        {
            orig_obj = (dynamic_cast<Instance*>(orig_obj)->original);
        }
    }
    if (PRINTINIT) std::cout << "motionblur: " << (s.contains("MotionBlur")
                                                       ? Vec3r(s["MotionBlur"])
                                                       : Vec3r(0.0, 0.0, 0.0)) << std::endl;
    sceneInput.objects.push_back(new Instance(
        std::stoi(s["_id"].get<std::string>()),
        orig_obj,
        getTransFromStr(s["Transformations"].get<std::string>(), sceneInput.transforms),
        s.contains("Material")
            ? sceneInput.Materials[std::stoi(s["Material"].get<std::string>()) - 1]
            : orig_obj->material,
        s.contains("MotionBlur") ? Vec3r(s["MotionBlur"]) : Vec3r(0.0, 0.0, 0.0),
        s.contains("Textures") ? getTexturesFromStr(s["Textures"], sceneInput) : std::vector<Texture*>(),
        false
    ));

    if (PRINTINIT) std::cout << sceneInput.objects[sceneInput.objects.size() - 1] << std::endl;
    curr_id++;
}


void Parser::addPlane(json p, SceneInput& sceneInput, uint32_t& curr_id)
{
    Plane* tempp = new Plane(
        std::stoi(p["_id"].get<std::string>()),
        sceneInput.Vertices[std::stoi(p["Point"].get<std::string>()) - 1].v,
        p["Normal"].get<std::string>(),
        sceneInput.Materials[std::stoi(p["Material"].get<std::string>()) - 1],
        p.contains("Textures") ? getTexturesFromStr(p["Textures"], sceneInput) : std::vector<Texture*>()
    );

    if (p.contains("Transformations"))
        addInstance(p["Transformations"].get<std::string>(), tempp, sceneInput,
                    p.contains("MotionBlur") ? Vec3r(p["MotionBlur"]) : Vec3r());
    else if (p.contains("MotionBlur"))addInstance("", tempp, sceneInput, Vec3r(p["MotionBlur"]));
    else sceneInput.objects.push_back(tempp);
    if (PRINTINIT) std::cout << sceneInput.objects[curr_id] << std::endl;

    curr_id++;
}


void Parser::addTranslation(json t, SceneInput& sceneInput)
{
    sceneInput.transforms.push_back(std::make_shared<Translate>(Vertex(t["_data"])));
    if (PRINTINIT) std::cout << sceneInput.transforms[sceneInput.transforms.size() - 1] << std::endl;
}

void Parser::addScaling(json t, SceneInput& sceneInput)
{
    sceneInput.transforms.push_back(std::make_shared<Scale>(Vertex(t["_data"])));
    if (PRINTINIT) std::cout << sceneInput.transforms[sceneInput.transforms.size() - 1] << std::endl;
}

void Parser::addRotation(json t, SceneInput& sceneInput)
{
    std::istringstream ss(t["_data"].get<std::string>());
    double angle, x, y, z;
    ss >> angle >> x >> y >> z;
    angle = angle * M_PI / 180;
    if (ss.fail())
    {
        throw std::invalid_argument("Invalid Vertex string: " + t["_data"].get<std::string>());
    }
    sceneInput.transforms.push_back(std::make_shared<Rotate>(Ray(Vertex(0, 0, 0), Vec3r(x, y, z)), angle));
    if (PRINTINIT) std::cout << sceneInput.transforms[sceneInput.transforms.size() - 1] << std::endl;
}


void Parser::addComposite(json t, SceneInput& sceneInput)
{
    std::istringstream ss(t["_data"].get<std::string>());
    M4trix mat;
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            ss >> mat[i][j];

    if (ss.fail())
    {
        throw std::invalid_argument("Invalid Vertex string: " + t["_data"].get<std::string>());
    }
    sceneInput.transforms.push_back(std::make_shared<Composite>(mat));
    if (PRINTINIT) std::cout << sceneInput.transforms[sceneInput.transforms.size() - 1] << std::endl;
}


std::shared_ptr<Transformation> Parser::getTransFromStr(std::string transStr,
                                                        std::vector<std::shared_ptr<Transformation>>& transforms)
{
    std::shared_ptr<Transformation> transformation;
    // std::cout << transStr << std::endl;
    if (transStr == "")
    {
        transformation = std::make_shared<Translate>(Vertex(0, 0, 0));
    }
    else
    {
        std::istringstream ss(transStr);
        std::vector<Transformation*> temp;
        char transChar;
        int transID;
        while (ss >> transChar >> transID)
        {
            int startID;
            if (transChar == 't') startID = trans1_transStartID;
            else if (transChar == 's') startID = trans2_scaleStartID;
            else if (transChar == 'r') startID = trans3_rotStartID;
            else if (transChar == 'c') startID = trans4_compStartID;
            else startID = 0;
            // std::cout << transChar << " " << startID << std::endl;

            temp.push_back(transforms[startID + transID - 1].get());
        }

        transformation = std::make_shared<Composite>(temp);
    }

    return transformation;
}

Object* Parser::getOriginalObjPtr(ObjectType ot, int ot_id, std::deque<Object*>& objs)
{
    for (int startID = 0; startID < objs.size(); startID++)
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

void Parser::getTexCoords(json TexCoords, SceneInput& sceneInput)
{
    if (TexCoords["_type"] == "uv")
    {
        std::istringstream ss(TexCoords["_data"].get<std::string>());
        Texel t(0,0);
        for (int i = 0; i < sceneInput.Vertices.size(); i++)
        {
            ss >> t.u >> t.v;
            sceneInput.Vertices[i].t = t;
        }
    }
}

std::vector<Texture*> Parser::getTexturesFromStr(std::string inp, SceneInput& scene)
{
    std::shared_ptr<Transformation> transformation;
    // std::cout << transStr << std::endl;
    if (inp == "")
    {
        return std::vector<Texture*>();
    }

    std::istringstream ss(inp);
    std::vector<Texture*> temp;
    Texture* tp;
    int id;
    while (ss >> id)
    {
        tp = getTextureWithId(id, scene);
        if (tp != nullptr)temp.push_back(tp);
        else std::cout << "!!!!!!Could not find texture with id " << id << std::endl;
    }
    return temp;
}

Texture* Parser::getTextureWithId(int id, SceneInput& scene)
{
    std::vector<Texture*> textures = scene.textures;
    for (int i = 0; i < textures.size(); i++)
    {
        if (textures[i] != nullptr && textures[i]->_id == id) return textures[i];
    }
    return nullptr;
}

Image *Parser::getImageFromId(int id, SceneInput& scene)
{
    std::vector<Image*> images = scene.images;
    for (int i = 0; i < images.size(); i++)
    {
        if (images[i]->_id == id) return images[i];
    }
    std::cout << "Could not find the id!!!!: " << id << std::endl;
    return images[0];
}

BRDF* Parser::getBRDFFromID(uint32_t id, std::vector<BRDF*> BRDFs)
{
    for (int i = 0; i < BRDFs.size(); i++)
    {
        if (BRDFs[i]->id == id) return BRDFs[i];
    }
    std::cout << "Could not find the id!!!!: " << id << std::endl;
    return BRDFs[0];
}

std::function<real(real)> Parser::getConversionFunc(json inp)
{
    std::string funcname = inp.get<std::string>();
    if (funcname == "absval")return Convert::Abs;
    if (funcname == "linear")return Convert::Linear;
    return Convert::Linear;
}

void Parser::getRendererParams(std::string params, PathTracer *path)
{
    if (params.empty()) return;

    if (params.find("ImportanceSampling") != std::string::npos)
        path->importance_sampling = true;

    if (params.find("NextEventEstimation") != std::string::npos)
        path->NEE = true;

    if (params.find("MIS_BALANCE") != std::string::npos)
        path->MIS_BALANCE = true;

    if (params.find("RussianRoulette") != std::string::npos)
        path->RussianRoulette = true;
}