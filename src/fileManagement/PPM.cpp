//
// Created by vicy on 10/18/25.
//

#include "PPM.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <iostream>

#include "stb_image.h"
#include "stb_image_write.h"

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


void PPM::write_stb(const char* filename, unsigned char* data, int width, int height)
{
    if (stbi_write_png(filename, width, height, 3, data, width * 3)) return;

    std::runtime_error("Failed to save image!");
}


unsigned char* PPM::read_image(const char* filename, int &width, int &height, int &channels_in_file, int desired_channels)
{
    unsigned char* data = stbi_load(filename, &width, &height, &channels_in_file, desired_channels);

    if(!data)
    {
        std::cerr << "Adresin yanlış adresin. Bir kere de düzgün hallet şu adres işini seni file özürlüsü.\n";
    }
    return data;
}

