//
// Created by aysu on 04.10.2025.
//

#ifndef CENG477_PARSER_H
#define CENG477_PARSER_H

#include <iostream>
#include <fstream>
#include <iostream>

#include "dataTypes/DataTypes.h"
#include <jsoncpp/json/json.h>

#define PRINTINIT false


extern SceneInput sceneInput;

namespace Parser {
    void parseScene(std::string inpFile);
    std::vector<int> parseTrioFromString(std::string inp);
    Json::Value getJsonDataFromFile(std::string inpFile);

    void getCameras(Json::Value inp);
    void getLights(Json::Value inp);
    void getMaterials(Json::Value inp);
    void getVertexData(Json::Value inp);
    void getObjects(Json::Value inp);
};

std::ostream& operator<<(std::ostream& os, const Color& c);
std::ostream& operator<<(std::ostream& os, const Vector& v);
std::ostream& operator<<(std::ostream& os, const Vertex& v);
std::ostream& operator<<(std::ostream& os, const PointLight & pl);
std::ostream& operator<<(std::ostream& os, const SceneInput& s);

#endif //CENG477_PARSER_H
