//
// Created by vicy on 10/18/25.
//

#include "PPM.h"


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