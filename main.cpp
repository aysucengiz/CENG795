#include <iostream>
#include <fstream>
#include <iostream>

#include "dataTypes/DataTypes.h"
#include <jsoncpp/json/json.h>
#include "Parser.h"


SceneInput sceneInput;

int main(void) {
    Parser::parseScene("jsonFiles/test.json");
    std::cout << sceneInput;
    return 0;
}
