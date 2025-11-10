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
#include "../dataTypes/functions/helpers.h"

#include "json.hpp"

using json = nlohmann::json;


namespace Parser {

    void parseScene(std::string inpFile, SceneInput &sceneInput);
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
    void addInstance(std::string transformations, Object *original, SceneInput &sceneInput);
    void addInstance(json p, SceneInput &sceneInput, uint32_t &curr_id);

    void addTranslation(json Camera, SceneInput &sceneInput);
    void addScaling(json p, SceneInput &sceneInput);
    void addRotation(json p, SceneInput &sceneInput);

    Transformation *getTransFromStr(std::string transStr, std::vector<Transformation *> &transforms);
    Object *getOriginalObjPtr(ObjectType ot, int ot_id, std::vector<Object *> &objs);



};


#endif //CENG477_PARSER_H
