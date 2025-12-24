#include "Camera.h"
#include <random>
#include "../../functions/helpers.h"
#include "../../functions/overloads.h"


Camera::Camera(uint32_t id, Vertex pos, Vec3r g, Vec3r u, std::array<double,4> locs, real nd, uint32_t width, uint32_t height, std::string imname,
    uint32_t numSamples, real focusDistance, real apertureSize, SamplingType st, std::vector<ToneMap> tms)
    : _id(id), Position(pos), nearDistance(nd), ImageName(imname), width(width), height(height),
      numSamples(numSamples), FocusDistance(focusDistance), ApertureSize(apertureSize)
{

    if (contains(imname,".hdr") || contains(imname,".exr"))
    {
        outputType = OutputType::HDR;
        HDRimage = new real[width * height * 3];
    }
    else
    {
        outputType = OutputType::LDR;
        LDRimage = new unsigned char[width * height * 3];
    }

    if (tms.size() > 0){
        tonemaps = tms;
        for(int i=0; i < tonemaps.size(); i++){
            tonemaps[i].image = new unsigned char[width * height * 3];
        }
    }

    Gaze = g.normalize();
    V = x_product(u.normalize(), -Gaze).normalize();
    Up = x_product(-Gaze, V).normalize();

    l = locs[0];
    r = locs[1];
    b = locs[2];
    t = locs[3];
    initializeSamples(st, samplesPixel);
    initializeSamples(st, samplesCamera);
    initializeSamples(st, samplesLight);
    initializeSamples(st, samplesGlossy);
    initializeSamples2D(st, samplesTime);
}


Camera::~Camera()
{
    if(LDRimage) delete[] LDRimage;
    if(HDRimage) delete[] HDRimage;
}


std::mt19937 gRandomGeneratorC;
void Camera::initializeSamples(SamplingType st, std::vector<std::array<real, 2>> &samples)
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


void Camera::initializeSamples2D(SamplingType st, std::vector<real> &samples)
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
    if (ApertureSize > 0)  return Position + (Up *(samplesCamera[i][0]-0.5) + V * (samplesCamera[i][1]-0.5))*ApertureSize;
    else                   return Position;
}

void Camera::writeColour(uint32_t& curr_pixel, Color& final_color) const
{

    for (int i = 0; i < tonemaps.size(); i++) tonemaps[i].writeColour(curr_pixel, final_color);
    if (outputType == OutputType::LDR)
    {

        LDRimage[curr_pixel++] = clamp(final_color.r, 0, 255);
        LDRimage[curr_pixel++] = clamp(final_color.g, 0, 255);
        LDRimage[curr_pixel++] = clamp(final_color.b, 0, 255);
    }
    else
    {
        HDRimage[curr_pixel++] = final_color.r;
        HDRimage[curr_pixel++] = final_color.g;
        HDRimage[curr_pixel++] = final_color.b;
    }


}

void Camera::writeToImage(std::string output_path) const
{
    if (outputType == OutputType::LDR)
        PPM::write_ldr((output_path  + ImageName).c_str(), LDRimage, width, height);
    else if (cam.outputType == OutputType::HDR)
        PPM::write_hdr((output_path  + ImageName).c_str(), HDRimage, width, height);

    for (int i = 0; i < tonemaps.size(); i++) tonemaps[i].writeToImage(output_path);

}
