//
// Created by aysu on 04.10.2025.
//

#ifndef CENG477_PARSER_H
#define CENG477_PARSER_H

#include <iostream>
#include <fstream>
#include <iostream>

#include "../dataTypes/base/SceneData.h"
#include "../dataTypes/object/Object.h"
#include "../dataTypes/object/Mesh.h"
#include "../functions/helpers.h"

#include "json.hpp"

using json = nlohmann::json;


namespace Parser {

    void parseScene(std::string inpFile, SceneInput &sceneInput, uint32_t maxDepth, real DefaultShadowEps, real DefaultIntersEps, real print_init);
    std::vector<int> parseTrioFromString(std::string inp);
    json getJsonDataFromFile(std::string inpFile);

    void getCameras(json inp, SceneInput &sceneInput);
    void getLights(json inp, SceneInput &sceneInput);
    void getMaterials(json inp, SceneInput &sceneInput);
    void getTransformations(json inp, SceneInput &sceneInput);
    void getVertexData(json inp, SceneInput &sceneInput);
    void getObjects(json inp, SceneInput &sceneInput, std::string root);
    void getTexCoords(json TexCoords, SceneInput& sceneInput);
    void getTextures(json& inp, SceneInput& sceneInput);
    void getImages(json& inp, SceneInput& sceneInput);
    void getTextureMaps(json Textures, SceneInput& sceneInput);

    void addTriangle(json tri, SceneInput &sceneInput, uint32_t &curr_id);
    void addSphere(json s, SceneInput &sceneInput, uint32_t &curr_id);
    void addMesh(json mes, SceneInput &sceneInput, uint32_t &curr_id, std::string root);
    void addMaterial(json inp, SceneInput &sceneInput);
    void addLight(json pointLights, SceneInput &sceneInput);
    void addCamera(json Camera, SceneInput &sceneInput);
    void addPlane(json p, SceneInput &sceneInput, uint32_t &curr_id);
    void addInstance(std::string transformations, Object* original, SceneInput& sceneInput, Vec3r motion);
    void addInstance(json p, SceneInput &sceneInput, uint32_t &curr_id);
    void addTranslation(json Camera, SceneInput &sceneInput);
    void addScaling(json p, SceneInput &sceneInput);
    void addRotation(json p, SceneInput &sceneInput);
    void addImage(json s, SceneInput& sceneInput);
    void addTextureMap(json s, SceneInput& sceneInput);

    Scale getScaleFromStr(std::string transStr, std::vector<std::shared_ptr<Transformation>>& transforms);
    std::vector<Texture*> getTexturesFromStr(std::string inp,SceneInput &scene);
    void getNearFromFovY(int FovY, double nearDistance, double aspect, std::array<double,4> &nearPlane);

    std::shared_ptr<Transformation> getTransFromStr(std::string transStr, std::vector<std::shared_ptr<Transformation>> &transforms);
    Object* getOriginalObjPtr(ObjectType ot, int ot_id, std::deque<Object*> &objs);
    Texture *getTextureWithId(int id,SceneInput &scene);
    Image &getImageFromId(int id, SceneInput& scene);
    std::function<real(real)> getConversionFunc(json inp);
};


#endif //CENG477_PARSER_H
