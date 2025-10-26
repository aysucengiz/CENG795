//
// Created by vicy on 10/18/25.
//

#ifndef CENG795_PPM_H
#define CENG795_PPM_H

#include <stdexcept>

// TODO: You will save the resulting images in the PNG format. You can use a library of your choice
//      for saving PNG images. A popular choice is the stb-image library1.

namespace PPM
{
    void write_ppm(const char* filename, unsigned char* data, int width, int height);

};


#endif //CENG795_PPM_H