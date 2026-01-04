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


void PPM::write_hdr(const char* filename, std::vector<Color> &data, int width, int height)
{
    // use stb
    float *outdata = new float[width * height];
    for (int i = 0; i < width * height; i++)
    {
        outdata[3*i] = data[i].r;
        outdata[3*i+1] = data[i].g;
        outdata[3*i+2] = data[i].b;
    }
    if (stbi_write_hdr(filename, width, height, 3, outdata)) return;

    std::runtime_error("Failed to save image!");
}


void PPM::write_exr(const char* filename, std::vector<Color> &data, int width, int height)
{
    EXRHeader header;
    InitEXRHeader(&header);

    EXRImage image;
    InitEXRImage(&image);

    image.num_channels = 3;

    int size = width * height;

    std::vector<float> images[3];
    images[0].resize(size);
    images[1].resize(size);
    images[2].resize(size);

    for (int i = 0; i <size; i++)
    {
        images[0][i] = data[i].r;
        images[1][i] = data[i].g;
        images[2][i] = data[i].b;
    }


    float* image_ptr[3];
    image_ptr[0] = &(images[2].at(0)); // B
    image_ptr[1] = &(images[1].at(0)); // G
    image_ptr[2] = &(images[0].at(0)); // R

    image.images = (unsigned char**)image_ptr;
    image.width = width;
    image.height = height;

    header.num_channels = 3;
    header.channels = (EXRChannelInfo *)malloc(sizeof(EXRChannelInfo) * header.num_channels);
    // Must be (A)BGR order, since most of EXR viewers expect this channel order.
    strncpy(header.channels[0].name, "B", 255); header.channels[0].name[strlen("B")] = '\0';
    strncpy(header.channels[1].name, "G", 255); header.channels[1].name[strlen("G")] = '\0';
    strncpy(header.channels[2].name, "R", 255); header.channels[2].name[strlen("R")] = '\0';

    header.pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
    header.requested_pixel_types = (int *)malloc(sizeof(int) * header.num_channels);
    for (int i = 0; i < header.num_channels; i++) {
        header.pixel_types[i] = TINYEXR_PIXELTYPE_FLOAT; // pixel type of input image
        header.requested_pixel_types[i] = TINYEXR_PIXELTYPE_HALF; // pixel type of output image to be stored in .EXR
    }

    const char* err = NULL; // or nullptr in C++11 or later.
    int ret = SaveEXRImageToFile(&image, &header, filename, &err);
    if (ret != TINYEXR_SUCCESS) {
        fprintf(stderr, "Save EXR err: %s\n", err);
        FreeEXRErrorMessage(err); // free's buffer for an error message
    }
    printf("Saved exr file. [ %s ] \n", filename);


    free(header.channels);
    free(header.pixel_types);
    free(header.requested_pixel_types);
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



