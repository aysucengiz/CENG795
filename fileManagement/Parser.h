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


#define PRINTINIT true
#define DEFAULT_MAX_DEPTH 10
#define DEFAULT_SHADOW_EPS 1e-3
#define DEFAULT_INTERS_EPS 1e-6

namespace Parser {
    void parseScene(std::string inpFile, SceneInput &sceneInput);
    std::vector<int> parseTrioFromString(std::string inp);
    json getJsonDataFromFile(std::string inpFile);

    void getCameras(json inp, SceneInput &sceneInput);
    void getLights(json inp, SceneInput &sceneInput);
    void getMaterials(json inp, SceneInput &sceneInput);
    void getVertexData(json inp, SceneInput &sceneInput);
    void getObjects(json inp, SceneInput &sceneInput);

    void computeTriangleValues(Triangle &t, SceneInput &scene);
};


#endif //CENG477_PARSER_H
