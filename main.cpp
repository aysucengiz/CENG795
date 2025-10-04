#include <iostream>
#include "rapidjson/document.h"
#include <fstream>
#include <iostream>
#include "dataTypes/DataTypes.h"

using namespace std;
using namespace rapidjson;

SceneInput sceneInput;



void parseScene(char* inpFile){

    // Open the file
    ifstream file(inpFile);

    // Read the entire file into a string
    string json((istreambuf_iterator<char>(file)),
                istreambuf_iterator<char>());

    // Create a Document object
    // to hold the JSON data
    Document doc;

    // Parse the JSON data
    doc.Parse(json.c_str());

    // Check for parse errors
    if (doc.HasParseError()) {
        cerr << "Error parsing JSON: "
             << doc.GetParseError() << endl;
        return 1;
    }
}

int main(void) {
    parseScene("test.json");
    return 0;
}
