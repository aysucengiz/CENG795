//
// Created by aysu on 04.10.2025.
//

#ifndef CENG477_PARSER_H
#define CENG477_PARSER_H

#include <iostream>
#include <fstream>
#include <iostream>

#include "../dataTypes/DataTypes.h"
#include "../dataTypes/Object.h"
#include "../dataTypes/helpers.h"

#include "json.hpp"

using json = nlohmann::json;


#define PRINTINIT false
#define DEFAULT_MAX_DEPTH 10
#define DEFAULT_SHADOW_EPS 1e-3
#define DEFAULT_INTERS_EPS 1e-6

// TODO: dosya içindeki tüm dosyaları halletme, ve bu pathi otomatik bulma ekle
#define JSON_FILES "jsonFiles/inputs/akif_uslu/"

#include <chrono>

namespace Parser {
    void parseScene(std::string inpFile, SceneInput &sceneInput);
    std::vector<int> parseTrioFromString(std::string inp);
    json getJsonDataFromFile(std::string inpFile);

    void getCameras(json inp, SceneInput &sceneInput);
    void getLights(json inp, SceneInput &sceneInput);
    void getMaterials(json inp, SceneInput &sceneInput);
    void getVertexData(json inp, SceneInput &sceneInput);
    void getObjects(json inp, SceneInput &sceneInput);


    void addTriangle(json tri, SceneInput &sceneInput, uint32_t &curr_id);
    void addSphere(json s, SceneInput &sceneInput, uint32_t &curr_id);
    void addMesh(json mes, SceneInput &sceneInput, uint32_t &curr_id);
    void addMaterial(json inp, SceneInput &sceneInput);
    void addLight(json pointLights, SceneInput &sceneInput);
    void addCamera(json Camera, SceneInput &sceneInput);
    void addPlane(json p, SceneInput &sceneInput, uint32_t &curr_id);
};


#endif //CENG477_PARSER_H
