

struct ToneMap{
    std::string extension;
    TMOTYpe TMO;
    real gamma;
    real saturation;
    std::array<real,2> TMOOptions;
}

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
    ToneMap tonemap;

    Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname,
        uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st);


    void initializeSamples2D(SamplingType st, std::vector<real> &samples);
    void initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples);
    Vertex getPos(int i) const;
};