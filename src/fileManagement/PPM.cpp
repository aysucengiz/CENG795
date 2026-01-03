//
// Created by vicy on 10/18/25.
//

#include "PPM.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION
#include <iostream>

#include "stb_image.h"
#include "stb_image_write.h"
#define TINYEXR_IMPLEMENTATION
#include "tinyexr.h"


void PPM::write_ppm(const char* filename, unsigned char* data, int width, int height)
{
    FILE* outfile = fopen(filename, "wb");
    if (!outfile) throw std::runtime_error("Error: The ppm file cannot be opened for writing.");
    fprintf(outfile, "P6\n%d %d\n255\n", width, height);
    size_t written = fwrite(data, 3, width * height, outfile);

    if (written != static_cast<size_t>(width * height))
    {
        fclose(outfile);
        throw std::runtime_error("Error: incomplete write to file.");
    }

    fclose(outfile);
}


void PPM::write_ldr(const char* filename, unsigned char* data, int width, int height)
{
    if (stbi_write_png(filename, width, height, 3, data, width * 3)) return;

    std::runtime_error("Failed to save image!");
}


void PPM::write_hdr(const char* filename, real* data, int width, int height)
{
    if (stbi_write_png(filename, width, height, 3, data, width * 3)) return;

    std::runtime_error("Failed to save image!");
}



unsigned char* PPM::read_image_ldr(const char* filename, int &width, int &height, int &channels_in_file, int desired_channels)
{
    unsigned char* data = stbi_load(filename, &width, &height, &channels_in_file, desired_channels);

    if(!data)
    {
        std::cerr << "Adresin yanlış adresin. Bir kere de düzgün hallet şu adres işini seni file özürlüsü.\n";
    }
    return data;
}


float* PPM::read_image_hdr(const char* filename, int &width, int &height, int &channels_in_file, int desired_channels)
{
    float* data = stbi_loadf(filename, &width, &height, &channels_in_file, desired_channels);

    if(!data)
    {
        std::cerr << "Adresin yanlış adresin. Bir kere de düzgün hallet şu adres işini seni file özürlüsü.\n";
        std::cerr << "stbi_loadf failed: "
                  << stbi_failure_reason() << std::endl;
    }
    return data;
}

float* PPM::read_image_exr(const char* filename, int &width, int &height, int &channels_in_file, int desired_channels)
{
    float* data;
    const char* err = nullptr;

    int ret = LoadEXR(&data, &width, &height, filename, &err);

    if (ret != TINYEXR_SUCCESS) {
        std::cerr << "EXR load failed: " << (err ? err : "unknown") << std::endl;
        FreeEXRErrorMessage(err);
        return nullptr;
    }

    channels_in_file = 4; // LoadEXR always returns RGBA
    return data;
}



