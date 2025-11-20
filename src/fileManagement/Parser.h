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


    void addTriangle(json tri, SceneInput &sceneInput, uint32_t &curr_id);
    void addSphere(json s, SceneInput &sceneInput, uint32_t &curr_id);
    void addMesh(json mes, SceneInput &sceneInput, uint32_t &curr_id, std::string root);
    void addMaterial(json inp, SceneInput &sceneInput);
    void addLight(json pointLights, SceneInput &sceneInput);
    void addCamera(json Camera, SceneInput &sceneInput);
    void addPlane(json p, SceneInput &sceneInput, uint32_t &curr_id);
    void addInstance(std::string transformations, std::shared_ptr<Object> original, SceneInput &sceneInput);
    void addInstance(json p, SceneInput &sceneInput, uint32_t &curr_id);
    void addTranslation(json Camera, SceneInput &sceneInput);
    void addScaling(json p, SceneInput &sceneInput);
    void addRotation(json p, SceneInput &sceneInput);

    Scale getScaleFromStr(std::string transStr, std::vector<std::unique_ptr<Transformation>>& transforms);
    void getNearFromFovY(int FovY, double nearDistance, double aspect, std::array<double,4> &nearPlane);

    std::unique_ptr<Transformation> getTransFromStr(std::string transStr, std::vector<std::unique_ptr<Transformation>> &transforms);
    std::shared_ptr<Object> getOriginalObjPtr(ObjectType ot, int ot_id, std::deque<std::shared_ptr<Object>> &objs);
};


#endif //CENG477_PARSER_H
