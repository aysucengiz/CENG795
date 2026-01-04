//
// Created by vicy on 10/18/25.
//

#ifndef CENG795_PPM_H
#define CENG795_PPM_H

#include <stdexcept>
#include "typedefs.h"

#include "../dataTypes/base/SceneData.h"

namespace PPM
{
    void write_ppm(const char* filename, unsigned char* data, int width, int height);
    void write_ldr(const char* filename, unsigned char* data, int width, int height);
    void write_hdr(const char* filename, std::vector<Color> &data, int width, int height);
    void write_exr(const char* filename, std::vector<Color> &data, int width, int height);

    unsigned char *read_image_ldr(const char* filename, int& width, int& height, int& channels_in_file, int desired_channels);
    float *read_image_hdr(const char* filename, int& width, int& height, int& channels_in_file, int desired_channels);
    float *read_image_exr(const char* filename, int &width, int &height, int &channels_in_file, int desired_channels);
};


#endif //CENG795_PPM_H