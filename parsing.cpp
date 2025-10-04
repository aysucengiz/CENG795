//
// Created by Aysu on 04-Oct-25.
//
#include "rapidjson/document.h"
#include "rapidjson/filereadstream.h"
#include <iostream>

rapidjson::Document getJsonDataFromFile(){
    // Aysu: taken from geeksforgeeks
    //       (https://www.geeksforgeeks.org/cpp/rapidjson-file-read-write-in-cpp/)
    FILE* fp = fopen("data.json", "r");

    // Use a FileReadStream to read the data from the file
    char readBuffer[65536];
    rapidjson::FileReadStream is(fp, readBuffer, sizeof(readBuffer));

    // Parse the JSON data
    // using a Document object
    rapidjson::Document d;
    d.ParseStream(is);

    // Close the file
    fclose(fp);

    // Access the data in the JSON document
    std::cout << d["Scene"].GetString() << std::endl;
s
}