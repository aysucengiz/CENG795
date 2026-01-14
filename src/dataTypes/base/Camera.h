#ifndef CENG795_CAMERA_H_
#define CENG795_CAMERA_H_

#include "Vectors.h"
#include "../../typedefs.h"
#include "SceneData.h"
#include "SceneData.h"
#include <vector>
#include <functional>
#include "../../path/PathTracing.h"



struct CameraImage
{
    OutputType outputType;
    uint64_t size;
    uint32_t width;
    uint32_t height;
    std::string ImageName;
    real middle_gray;
    unsigned char *LDRimage = nullptr;
    std::vector<Color> HDRimage;
    std::vector<real> luminances;

    ~CameraImage();

    real ComputeMiddleGray();
    void compute_luminances();
    CameraImage(uint32_t width, uint32_t height, std::string imname);

    void writeColour(uint32_t& curr_pixel, Color& final_color);
    void writeToImage(std::string output_path);
};

struct CameraSamples
{
    uint32_t numSamples;
    std::vector<real> samplesTime;
    std::vector<std::array<real, 2>> samplesCamera;
    std::vector<std::array<real, 2>> samplesPixel;
    std::vector<std::array<real, 2>> samplesLight;
    std::vector<std::array<real, 2>> samplesGlossy;

    CameraSamples(SamplingType st, uint32_t numSamples);
    void initializeSamples2D(SamplingType st, std::vector<real> &samples);
    void initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples);

};

struct ToneMap{
    unsigned char *image = nullptr;
    CameraImage *camera_image = nullptr;
    std::string imname;
    TMOType tmoType;
    real gamma, bir_gamma;
    real saturation, key, burnout;
    int percentile_index = 0;
    real white_point;
    real black_point;
    ToneMap(const std::string& cam_imname, const std::string& extension, TMOType tmo, std::array<real,2> options, real g, real s);

    Color gamma_correct(Color inp) const;
    Color degamma_correct(Color inp) const;

    real TMO(real L) const;
    real TMOPhotographic(real L) const;
    real TMOACES(real L) const;
    real TMOFilmic(real L) const;

    static real MapFilmic(real L);
    static real MapACES(real L);
    Color tonemap(Color inp, int x, int y) const;
    ToneMap(const ToneMap &other)
    {
        camera_image = other.camera_image;
        imname = other.imname;
        tmoType = other.tmoType;
        gamma = other.gamma;
        bir_gamma = other.bir_gamma;
        saturation = other.saturation;
        key = other.key;
        burnout = other.burnout;
        white_point = other.white_point;
        black_point = other.black_point;
        percentile_index = other.percentile_index;

    }
    ToneMap& operator=(const ToneMap &other)
    {
        if (this == &other) return *this;
        camera_image = other.camera_image;
        imname = other.imname;
        tmoType = other.tmoType;
        gamma = other.gamma;
        bir_gamma = other.bir_gamma;
        saturation = other.saturation;
        key = other.key;
        burnout = other.burnout;
        white_point = other.white_point;
        black_point = other.black_point;
        percentile_index = other.percentile_index;
        return *this;

    }

    void writeColour(uint32_t curr_color, Color final_color, int x, int y) const;
    void writeToImage(std::string output_path);
};



class Camera{
public:
    TraceType trace_type;
    uint32_t _id;
    Vertex Position;
    Vec3r Gaze, Up, V;
    real l,r,b,t,nearDistance;
    real FocusDistance;
    real ApertureSize;
    std::vector<ToneMap> tonemaps;
    CameraSamples *sampleData;
    CameraImage *imageData;
    PathTracer *pathData = nullptr;

    uint32_t MaxRecursionDepth;
    uint32_t MinRecursionDepth;


    Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double, 4> locs, real nd, uint32_t width,
               uint32_t height, std::string imname,
               uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st, std::vector<ToneMap> tms, std::string handedness,
               PathTracer* path, uint32_t mind, uint32_t maxd);
    Camera(const Camera& other);
    Camera& operator=(const Camera& other);
    ~Camera(){};

    Vertex getPos(int i) const;
    void writeToImage(std::string output_path);
};

#endif