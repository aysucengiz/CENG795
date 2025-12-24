#ifndef CENG795_CAMERA_H_
#define CENG795_CAMERA_H_

#include "Vectors.h"
#include "../../typedefs.h"
#include <vector>

struct ToneMap{
    unsigned char *image = nullptr;
    std::string imname;
    TMOType TMO;
    std::array<real,2> TMOOptions;
    real gamma;
    real saturation;

    ToneMap(std::string cam_imname,std::string extension, TMOType tmo, std::array<real,2> options, real g, real s)
        : TMO(tmo), TMOOptions(options), gamma(g), saturation(s)
    {
        size_t dotPos = cam_imname.find_last_of('.');
        std::string baseName = cam_imname.substr(0, dotPos);
       imname = baseName + extension;
    }

    void writeColour(uint32_t curr_color, Color final_color) const;
    void writeToImage(std::string output_path) const;
};

class Camera{
public:
    uint32_t _id;
    Vertex Position;
    Vec3r Gaze;
    Vec3r Up;
    Vec3r V;
    real l;
    real r;
    real b;
    real t;
    real nearDistance;
    uint32_t width;
    uint32_t height;
    std::string ImageName;
    uint32_t numSamples;
    real FocusDistance;
    real ApertureSize;
    std::vector<std::array<real, 2>> samplesCamera;
    std::vector<std::array<real, 2>> samplesPixel;
    std::vector<std::array<real, 2>> samplesLight;
    std::vector<std::array<real, 2>> samplesGlossy;
    std::vector<real> samplesTime;
    OutputType outputType;
    std::vector<ToneMap> tonemaps;
    unsigned char *LDRimage = nullptr;
    real *HDRimage = nullptr;

    Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname,
        uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st, std::vector<ToneMap> tms);

    ~Camera();


    void initializeSamples2D(SamplingType st, std::vector<real> &samples);
    void initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples);
    Vertex getPos(int i) const;
    void writeColour(uint32_t& curr_pixel, Color& final_color) const;
    void writeToImage(std::string output_path) const;
};

#endif