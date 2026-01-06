#include "Camera.h"
#include <random>
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"
#include "../../fileManagement/PPM.h"
#include "../../fileManagement/stb_image.h"


ToneMap::ToneMap(const std::string& cam_imname, const std::string& extension, TMOType tmo, std::array<real, 2> options,
                 real g, real s)
    : tmoType(tmo), key(options[0]), burnout(options[1]), gamma(g), saturation(s)
{
    size_t dotPos = cam_imname.find_last_of('.');
    std::string baseName = cam_imname.substr(0, dotPos);
    imname = baseName + extension;

    bir_gamma = 1.0 / gamma;
}

real ToneMap::TMO(real L) const
{
    switch (tmoType)
    {
    case TMOType::PHOTOGRAPHIC:
        L = TMOPhotographic(L);
        break;
    case TMOType::ACES:
        L = TMOACES(L);
        break;
    case TMOType::FILMIC:
        L = TMOFilmic(L);
        break;
    default:
        std::cout << "Unrecognized tmoType" << std::endl;
        return 0;
    }
    return L;
}


real ToneMap::MapFilmic(real L)
{
    constexpr  real a = 0.22;
    constexpr  real b = 0.30;
    constexpr  real c = 0.10;
    constexpr  real d = 0.20;
    constexpr  real e = 0.01;
    constexpr  real f = 0.30;
    constexpr  real e_f = e / f;
    constexpr  real cb = c * b;
    constexpr  real de = d * e;
    constexpr  real df = d * f;
    real num = (L * (L * a + cb)) + de;
    real den = L * (L * a + b) + df;
    return num / den - e_f;
}

real ToneMap::MapACES(real L)
{
    constexpr  real a = 2.51;
    constexpr  real b = 0.03;
    constexpr  real c = 2.43;
    constexpr  real d = 0.59;
    constexpr  real e = 0.14;
    real num = L * (L * a + b);
    real den = L * (L * c + d) + e;
    return num / den;
}

real CameraImage::ComputeMiddleGray()
{
    constexpr real eps = 1e-6;
    real total = 0.0;
    int len = luminances.size();
    for (int i = 0; i < len; i++)
    {
        total += log(eps + luminances[i]);
    }
    return exp(total / len);
}

void CameraImage::compute_luminances()
{
    luminances.clear();
    int len = width * height;
    for (int curr_pixel = 0; curr_pixel < len; curr_pixel++)
    {
        luminances.push_back(luminance(HDRimage[curr_pixel]));
    }
    std::sort(luminances.begin(), luminances.end());
    // std::cout << len << std::endl;
}


real ToneMap::TMOFilmic(real L) const
{
    return MapFilmic(L) / MapFilmic(white_point);
}

real ToneMap::TMOACES(real L) const
{
    return MapACES(L) / MapACES(white_point);
}

real ToneMap::TMOPhotographic(real L) const
{
    if (burnout == 0)
    {
        return L / (L + 1.0);
    }
    else
    {
        return L * (1 + L / (white_point * white_point)) / (1 + L);
    }
}

Color ToneMap::gamma_correct(Color inp) const
{
    real r = pow(inp.r, bir_gamma);
    real g = pow(inp.g, bir_gamma);
    real b = pow(inp.b, bir_gamma);
    Color c = Color(r, g, b);
    return c;
}


Color ToneMap::tonemap(Color inp, int x, int y) const
{
    real Yi = luminance(inp);
    real Yi_keyed = (key / camera_image->middle_gray) * Yi;
    real Yo = TMO(Yi_keyed);
    Color final_color = inverseLuminance(inp, Yi, Yo, saturation);
    final_color = clampColor(final_color, 0.0, 1.0);
    final_color = gamma_correct(final_color);
    final_color = final_color * 255.0;
    Color final = Color(std::round(final_color.r), std::round(final_color.g), std::round(final_color.b));
    return final;
}

void ToneMap::writeColour(uint32_t curr_color, Color final_color, int x, int y) const
{
    Color tonemapped = tonemap(final_color, x, y);
    image[curr_color] = tonemapped.r;
    image[curr_color + 1] = tonemapped.g;
    image[curr_color + 2] = tonemapped.b;
}

void ToneMap::writeToImage(std::string output_path)
{
    percentile_index = (100.0 - burnout) / 100.0 * (camera_image->luminances.size()-1);
    white_point = camera_image->luminances[percentile_index];
    white_point = (key / camera_image->middle_gray) * white_point;
    std::cout << percentile_index << std::endl;
    black_point = camera_image->luminances[camera_image->luminances.size()-1 - percentile_index];
    uint32_t curr_pixel = 0;
    for (int y = 0; y < camera_image->height; y++)
        for (int x = 0; x < camera_image->width; x++)
        {
            writeColour(curr_pixel, camera_image->HDRimage[curr_pixel / 3], x,
                        y);
            curr_pixel += 3;
        }
    PPM::write_ldr((output_path + imname).c_str(), image, camera_image->width, camera_image->height);
}


CameraImage::CameraImage(uint32_t width, uint32_t height, std::string imname) : width(width), height(height),
    ImageName(imname)
{
    size = width * height * 3;
    HDRimage.resize(size / 3);
    if (contains(imname, ".hdr")) outputType = OutputType::HDR;
    else if (contains(imname, ".exr")) outputType = OutputType::EXR;
    else
    {
        outputType = OutputType::LDR;
        LDRimage = new unsigned char[size];
    }
}

CameraSamples::CameraSamples(SamplingType st, uint32_t numSamples) : numSamples(numSamples)
{
    initializeSamples(st, samplesPixel);
    initializeSamples(st, samplesCamera);
    initializeSamples(st, samplesLight);
    initializeSamples(st, samplesGlossy);
    initializeSamples2D(st, samplesTime);
}

Camera::Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double, 4> locs, real nd, uint32_t width,
               uint32_t height, std::string imname,
               uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st, std::vector<ToneMap> tms, std::string handedness)
    : _id(id),
      sampleData(new CameraSamples(st, numSamples)),
      imageData(new CameraImage(width, height, imname)),
      Position(pos), nearDistance(nd), FocusDistance(focusDistance), ApertureSize(apertureSize)
{
    if (tms.size() > 0)
    {
        tonemaps.clear();
        for (int i = 0; i < tms.size(); i++)
        {
            tonemaps.push_back(tms.at(i));
            tonemaps[i].image = new unsigned char[imageData->size];
        }
    }

    Gaze = g.normalize();
    V = x_product(u.normalize(), -Gaze).normalize();
    Up = x_product(-Gaze, V).normalize();
    if (handedness == "left") V = -V;

    l = locs[0];
    r = locs[1];
    b = locs[2];
    t = locs[3];
}

Camera::Camera(const Camera& other)
{
    _id = other._id;
    Position = other.Position;
    Gaze = other.Gaze;
    Up = other.Up;
    V = other.V;
    l = other.l;
    r = other.r;
    b = other.b;
    t = other.t;
    nearDistance = other.nearDistance;
    FocusDistance = other.FocusDistance;
    ApertureSize = other.ApertureSize;

    tonemaps.clear();
    for (int i = 0; i < other.tonemaps.size(); i++)
    {
        tonemaps.push_back(other.tonemaps[i]);
    }
    sampleData = other.sampleData;
    imageData = other.imageData;
}

Camera& Camera::operator=(const Camera& other)
{
    if (this == &other) return *this;

    _id = other._id;
    Position = other.Position;
    Gaze = other.Gaze;
    Up = other.Up;
    V = other.V;
    l = other.l;
    r = other.r;
    b = other.b;
    t = other.t;
    nearDistance = other.nearDistance;
    FocusDistance = other.FocusDistance;
    ApertureSize = other.ApertureSize;
    tonemaps.clear();
    for (int i = 0; i < other.tonemaps.size(); i++)
    {
        tonemaps.push_back(other.tonemaps[i]);
    }
    sampleData = other.sampleData;
    imageData = other.imageData;
    return *this;
}

CameraImage::~CameraImage()
{
}


std::mt19937 gRandomGeneratorC;

void CameraSamples::initializeSamples(SamplingType st, std::vector<std::array<real, 2>>& samples)
{
    samples.clear();
    if (numSamples == 1)
    {
        samples.push_back({0.5, 0.5});
        return;
    }

    samples.reserve(numSamples);
    std::pair<int, int> row_col = closestFactors(numSamples);
    switch (st)
    {
    case SamplingType::UNIFORM:
        {
            real spacing_x = 1.0 / (row_col.first + 1);
            real spacing_y = 1.0 / (row_col.second + 1);
            for (int y = 0; y < row_col.second; y++)
                for (int x = 0; x < row_col.first; x++)
                    samples.push_back({x * spacing_x, y * spacing_y});
        }
        break;
    case SamplingType::STRATIFIED:
        {
            // std::cout << row_col.first << " " << row_col.second << std::endl;
            real spacing_x = 1.0 / real(row_col.first);
            real spacing_y = 1.0 / real(row_col.second);
            for (int y = 0; y < row_col.second; y++)
                for (int x = 0; x < row_col.first; x++)
                    samples.push_back({(x + getRandom()) * spacing_x, (y + getRandom()) * spacing_y});
        }
        break;
    case SamplingType::N_ROOKS:
        {
            std::vector<int> cols(numSamples);
            for (int i = 0; i < numSamples; i++) cols[i] = i;
            std::shuffle(cols.begin(), cols.end(), gRandomGeneratorC);

            real spacing = 1.0 / real(numSamples);
            for (int i = 0; i < numSamples; i++)
                samples.push_back({(cols[i] + getRandom()) * spacing, (i + getRandom()) * spacing});
        }
        break;
    case SamplingType::MULTI_JITTERED:
        {
            std::vector<int> cols(numSamples);
            for (int i = 0; i < numSamples; i++) cols[i] = i;
            std::shuffle(cols.begin(), cols.end(), gRandomGeneratorC);

            std::vector<int> rows(numSamples);
            for (int i = 0; i < numSamples; i++) rows[i] = i;
            std::shuffle(rows.begin(), rows.end(), gRandomGeneratorC);

            real spacing = 1.0 / real(numSamples);
            for (int i = 0; i < numSamples; i++)
                samples.push_back({(rows[i] + getRandom()) * spacing, (cols[i] + getRandom()) * spacing});
        }
        break;
    case SamplingType::RANDOM:
    default:
        samples.reserve(numSamples);
        for (int i = 0; i < numSamples; i++) samples.push_back({getRandom(), getRandom()});
        break;
    }

    // for (auto sample : samples) std::cout << "sample: "<< sample[0] << " " << sample[1]  << std::endl;
}


void CameraSamples::initializeSamples2D(SamplingType st, std::vector<real>& samples)
{
    samples.clear();
    if (numSamples == 1)
    {
        samples.push_back(0.5);
        return;
    }

    samples.reserve(numSamples);
    switch (st)
    {
    case SamplingType::UNIFORM:
        {
            real spacing = 1.0 / (numSamples + 1);
            for (int x = 0; x < numSamples; x++)
                samples.push_back((x + 1.0) * spacing);
        }
        break;
    case SamplingType::STRATIFIED:
    case SamplingType::N_ROOKS:
    case SamplingType::MULTI_JITTERED:
        {
            real spacing = 1.0 / real(numSamples);
            for (int x = 0; x < numSamples; x++)
                samples.push_back((x + getRandom()) * spacing);
        }
        break;
    case SamplingType::RANDOM:
    default:
        samples.reserve(numSamples);
        for (int i = 0; i < numSamples; i++) samples.push_back(getRandom());
        break;
    }

    // for (auto sample : samples) std::cout << "sample: "<< sample[0] << " " << sample[1]  << std::endl;
}

Vertex Camera::getPos(int i) const
{
    if (ApertureSize > 0) return Position + (Up * (sampleData->samplesCamera[i][0] - 0.5) + V * (sampleData->
        samplesCamera[i][1] - 0.5)) * ApertureSize;
    else return Position;
}


void CameraImage::writeColour(uint32_t& curr_pixel, Color& final_color)
{
    HDRimage[curr_pixel] = final_color;
    curr_pixel++;
}

void CameraImage::writeToImage(std::string output_path)
{
    compute_luminances();
    middle_gray = ComputeMiddleGray();

    if (outputType == OutputType::LDR)
    {
        for (int curr_pixel = 0; curr_pixel < size; curr_pixel += 3)
        {
            Color& curr_color = HDRimage[curr_pixel / 3];
            LDRimage[curr_pixel] = clamp_int(curr_color.r, 0, 255);
            LDRimage[curr_pixel + 1] = clamp_int(curr_color.g, 0, 255);
            LDRimage[curr_pixel + 2] = clamp_int(curr_color.b, 0, 255);
        }
    }
    if (outputType == OutputType::LDR)
        PPM::write_ldr((output_path + ImageName).c_str(), LDRimage, width, height);
    else if (outputType == OutputType::HDR)
    {
        PPM::write_hdr((output_path + ImageName).c_str(), HDRimage, width, height);
    }
    else if (outputType == OutputType::EXR)
    {
        PPM::write_exr((output_path + ImageName).c_str(), HDRimage, width, height);
    }
}

void Camera::writeToImage(std::string output_path)
{
    imageData->writeToImage(output_path);
    for (int i = 0; i < tonemaps.size(); i++)
    {
        tonemaps[i].camera_image = imageData;
        tonemaps[i].image = new unsigned char[imageData->size];
        tonemaps[i].writeToImage(output_path);
    }
}
