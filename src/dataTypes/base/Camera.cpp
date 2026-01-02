#include "Camera.h"
#include <random>
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"
#include "../../fileManagement/PPM.h"
#include "../../fileManagement/stb_image.h"


ToneMap::ToneMap(const std::string& cam_imname, const std::string& extension, TMOType tmo, std::array<real,2> options, real g, real s)
    : tmoType(tmo), key(options[0]), burnout(options[1]), gamma(g), saturation(s)
{
    size_t dotPos = cam_imname.find_last_of('.');
    std::string baseName = cam_imname.substr(0, dotPos);
    imname = baseName + extension;

    bir_gamma = 1.0 / gamma;
    if(tmoType == TMOType::PHOTOGRAPHIC)
        tmofunc = std::bind(&ToneMap::TMOPhotographic, this, std::placeholders::_1);
    else if(tmoType == TMOType::ACES)
        tmofunc = std::bind(&ToneMap::TMOACES, this, std::placeholders::_1);
    else if(tmoType == TMOType::FILMIC)
        tmofunc = std::bind(&ToneMap::TMOFilmic, this, std::placeholders::_1);
}

real ToneMap::TMO(real L) const
{
    if(tmoType == TMOType::PHOTOGRAPHIC)
        return TMOPhotographic(L);
    else if(tmoType == TMOType::ACES)
        return TMOACES(L);
    else if(tmoType == TMOType::FILMIC)
        return TMOFilmic(L);
    else
        return TMOPhotographic(L);
}

real ToneMap::TMOPhotographic(real L) const
{
    real L_scaled = (key / camera_image->middle_gray) * L;
    return L_scaled / (L_scaled + 1.0);
}

real ToneMap::MapFilmic(real L)
{
    static real a = 0.22, b = 0.30, c = 0.10, d = 0.20, e = 0.01, f = 0.30;
    static real e_f = e/f;
    static real cb = c*b;
    static real de = d*e;
    static real df = d*f;
    real num = (L*(L*a + cb)) + de;
    real den = L*(L*a + b) + df;
    return num / den - e_f;
}

real ToneMap::MapACES(real L)
{
    static real a = 2.51, b = 0.03, c = 2.43, d = 0.59, e = 0.14;
    real num = L * (L * a + b);
    real den = L * (L * c + d) + e;
    return num / den;
}

real CameraImage::ComputeMiddleGray()
{
    static real eps = 0.00001;
    real total = 0.0;
    int len = size / 3;
    for (int i = 0; i < len; i++)
    {
        real Lw = luminance(HDRimage[i]);
        total += log2(eps + Lw);
    }
    return exp(total / len);
}

void CameraImage::MinMaxLuminance()
{
    real maxLuminance = -1.0;
    real minLuminance = INFINITY;
    int len = size/3;
    for (int curr_pixel = 0; curr_pixel < len; curr_pixel++)
    {
        real lum = luminance(HDRimage[curr_pixel]);
        maxLuminance = std::max(lum,maxLuminance);
        minLuminance = std::max(lum,minLuminance);
    }
    white_point = maxLuminance;
    black_point = minLuminance;
}


real ToneMap::TMOFilmic(real L) const
{
    return MapFilmic(L)/MapFilmic(camera_image->white_point);
}

real ToneMap::TMOACES(real L) const
{
    return MapACES(L)/MapACES(camera_image->white_point);
}

Color ToneMap::gamma_correct(Color inp) const
{
    Color clamped_color = clampColor(inp, 0.0, 1.0);

    return Color(
        pow(clamped_color.r, bir_gamma) * 255.0,
        pow(clamped_color.g, bir_gamma) * 255.0,
        pow(clamped_color.b, bir_gamma) * 255.0
    );
}


Color ToneMap::tonemap(Color inp) const
{
    real Yi = luminance(inp);
    real Yo = TMO(Yi);
    Color non_gamma_corrected = inverseLuminance(inp, Yi, Yo, saturation);
    Color gamma_corrected = gamma_correct(non_gamma_corrected);
    Color final = Color(std::round(gamma_corrected.r), std::round(gamma_corrected.g), std::round(gamma_corrected.b));
    return final;

}

void ToneMap::writeColour(uint32_t curr_color, Color final_color) const
{
    Color tonemapped = tonemap(final_color);
    image[curr_color  ] = tonemapped.r;
    image[curr_color+1] = tonemapped.g;
    image[curr_color+2] = tonemapped.b;
}

void ToneMap::writeToImage(std::string output_path) const
{
    uint32_t  curr_pixel= 0;
    for(int y=0; y<camera_image->height; y++)
        for(int x=0; x<camera_image->width; x++)
        {
            writeColour(curr_pixel, camera_image->HDRimage[curr_pixel/3]);
            curr_pixel +=3;
        }
    PPM::write_ldr((output_path + imname).c_str(), image, camera_image->width,camera_image->height);
}



CameraImage::CameraImage(uint32_t width, uint32_t height, std::string imname) : width(width), height(height), ImageName(imname)
{
    size = width * height * 3;
    outputType = OutputType::HDR;
    HDRimage.resize(size/3);
    if (!contains(imname,".hdr") && !contains(imname,".exr"))
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

Camera::Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname,
    uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st, std::vector<ToneMap> tms)
    : _id(id),
    sampleData(new CameraSamples(st, numSamples)),
    imageData(new CameraImage(width,height,imname)),
    Position(pos), nearDistance(nd), FocusDistance(focusDistance), ApertureSize(apertureSize)
{
    if (tms.size() > 0){
        tonemaps.clear();
        for(int i=0; i < tms.size(); i++){
            tonemaps.push_back(tms.at(i));
            tonemaps[i].image = new unsigned char[imageData->size];
        }
    }

    Gaze = g.normalize();
    V = x_product(u.normalize(), -Gaze).normalize();
    Up = x_product(-Gaze, V).normalize();

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
    for (int i=0; i< other.tonemaps.size(); i++)
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
    for (int i=0; i< other.tonemaps.size(); i++)
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
void CameraSamples::initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples)
{
    samples.clear();
    if (numSamples == 1)
    {
        samples.push_back({0.5,0.5});
        return;
    }

    samples.reserve(numSamples);
    std::pair<int,int> row_col = closestFactors(numSamples);
    switch (st)
    {
    case SamplingType::UNIFORM:
        {
            real spacing_x = 1.0 / (row_col.first+1);
            real spacing_y = 1.0 / (row_col.second+1);
            for (int y=0; y < row_col.second; y++)
                for (int x=0; x < row_col.first; x++)
                    samples.push_back({x*spacing_x, y*spacing_y});
        }break;
    case SamplingType::STRATIFIED:
        {
            // std::cout << row_col.first << " " << row_col.second << std::endl;
            real spacing_x = 1.0 / real(row_col.first);
            real spacing_y = 1.0 / real(row_col.second);
            for (int y=0; y < row_col.second; y++)
                for (int x=0; x < row_col.first; x++)
                    samples.push_back({(x+getRandom())*spacing_x, (y+getRandom())*spacing_y});
        }
        break;
    case SamplingType::N_ROOKS:
        {
            std::vector<int> cols(numSamples);
            for (int i = 0; i < numSamples; i++) cols[i] = i;
            std::shuffle(cols.begin(), cols.end(), gRandomGeneratorC);

            real spacing = 1.0 / real(numSamples);
            for (int i = 0; i < numSamples; i++)
                    samples.push_back({(cols[i]+getRandom())*spacing, (i+getRandom())*spacing});
        }
        break;
    case SamplingType::MULTI_JITTERED:
        {
            std::vector<int> cols(numSamples);
            for (int i = 0; i < numSamples; i++) cols[i] = i;
            std::shuffle(cols.begin(), cols.end(), gRandomGeneratorC);

            std::vector<int> rows(numSamples);
            for (int i = 0; i <numSamples; i++) rows[i] = i;
            std::shuffle(rows.begin(), rows.end(), gRandomGeneratorC);

            real spacing = 1.0 / real(numSamples);
            for (int i=0; i < numSamples; i++)
                    samples.push_back({(rows[i]+getRandom())*spacing, (cols[i]+getRandom())*spacing});
        }
        break;
    case SamplingType::RANDOM:
    default:
        samples.reserve(numSamples);
        for (int i = 0; i < numSamples; i++) samples.push_back({getRandom(),getRandom()});
        break;
    }

    // for (auto sample : samples) std::cout << "sample: "<< sample[0] << " " << sample[1]  << std::endl;



}


void CameraSamples::initializeSamples2D(SamplingType st, std::vector<real> &samples)
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
            real spacing = 1.0 / (numSamples+1);
            for (int x=0; x <numSamples; x++)
               samples.push_back((x+1.0)*spacing);
        }break;
    case SamplingType::STRATIFIED:
    case SamplingType::N_ROOKS:
    case SamplingType::MULTI_JITTERED:
        {

            real spacing = 1.0 / real(numSamples);
            for (int x=0; x <numSamples; x++)
                samples.push_back((x+getRandom())*spacing);
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
    if (ApertureSize > 0)  return Position + (Up *(sampleData->samplesCamera[i][0]-0.5) + V * (sampleData->samplesCamera[i][1]-0.5))*ApertureSize;
    else                   return Position;
}


void CameraImage::writeColour(uint32_t& curr_pixel, Color& final_color)
{
    HDRimage[curr_pixel++] = final_color;
}

void CameraImage::writeToImage(std::string output_path)
{
    MinMaxLuminance();
    middle_gray = ComputeMiddleGray();

    if (outputType == OutputType::LDR)
    {
        for(int curr_pixel=0; curr_pixel<size; curr_pixel+=3)
        {
            Color &curr_color = HDRimage[curr_pixel/3];
            LDRimage[curr_pixel  ]  = clamp(curr_color.r, 0, 255);
            LDRimage[curr_pixel+1]  = clamp(curr_color.g, 0, 255);
            LDRimage[curr_pixel+2]  = clamp(curr_color.b, 0, 255);
        }
    }
    if (outputType == OutputType::LDR)
        PPM::write_ldr((output_path  + ImageName).c_str(), LDRimage, width, height);
    else if (outputType == OutputType::HDR){
        real *ptr = new real[size];
        for (int i=0; i<size; i+=3)
        {
            ptr[i] = HDRimage[i/3].r;
            ptr[i+1] = HDRimage[i/3].g;
            ptr[i+2] = HDRimage[i/3].b;
        }
        PPM::write_hdr((output_path  + ImageName).c_str(),ptr , width, height);
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