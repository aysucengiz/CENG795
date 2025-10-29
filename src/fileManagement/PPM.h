//
// Created by vicy on 10/18/25.
//

#ifndef CENG795_PPM_H
#define CENG795_PPM_H

#include <stdexcept>



namespace PPM
{
    void write_ppm(const char* filename, unsigned char* data, int width, int height);
    void write_stb(const char* filename, unsigned char* data, int width, int height);

};


#endif //CENG795_PPM_H