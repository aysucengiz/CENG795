//
// Created by vicy on 12/06/25.
//

#include "TextureMap.h"

#include "../../fileManagement/PPM.h"
#include "../../functions/overloads.h"
#include "../../functions/helpers.h"
#include "fileManagement/stb_image.h"

int PerlinNoise::P[512];
bool PerlinNoise::initialized = false;
int PerlinNoise::perm[256];

real Convert::Abs(real inp) { return std::min((real)1.0,std::abs(inp)); }
real Convert::Linear(real inp) { return std::min(1.0, std::max(inp * 0.5 + 0.5,0.0)); }

TextureType ImageTexture::getTextureType(){ return TextureType::IMAGE;}
TextureType PerlinTexture::getTextureType(){ return TextureType::PERLIN;}
TextureType CheckerTexture::getTextureType(){ return TextureType::CHECKERBOARD;}

/// IMAGE ////
Image::Image(uint32_t id, std::string filename) : _id(id), filename(filename)
{
    unsigned char *data = PPM::read_image(filename.c_str(), width, height, channels_in_file, 3);

    if (data)
    {
        uint32_t curr_idx = 0;
        colorData.resize(height, std::vector<Color>(width));
        for (int y = 0; y < height; y++)
        {
            for (int x = 0; x < width; x++)
            {
                colorData[y][x] = Color(data[curr_idx], data[curr_idx+1], data[curr_idx+2]);
                curr_idx += 3;
            }
        }
    }
    else std::cout << "Image could not be read" << std::endl;
}

Image::~Image()
{
}


/// IMAGE TEXTURE ////
Color ImageTexture::TextureColor(const Vertex &vert, Texel &tex)
{
    Color interpolated_color = interpolate(tex);
    return  interpolated_color/ 255.0;
}

Color ImageTexture::ImageColor(int x, int y)
{

    y = std::max(0, std::min(y , image->height-1));
    x = std::max(0, std::min(x , image->width-1));
    return image->colorData[y][x];
}

Color ImageTexture::nearest(Texel texel)
{
    Texel xy = {texel.u * (image->width), texel.v * (image->height)};
    return ImageColor(std::round(xy.u - 0.5), std::round(xy.v - 0.5));
}

Color ImageTexture::bilinear(Texel texel)
{
    Texel xy = {texel.u * (image->width), texel.v * (image->height)};
    Texel pq = {std::floor(xy.u), std::floor(xy.v)};
    Texel delta = {xy.u - pq.u, xy.v - pq.v};
    Color result = ImageColor(pq.u, pq.v) * (1 - delta.u) * (1 - delta.v)
        + ImageColor(pq.u + 1, pq.v) * (delta.u) * (1 - delta.v)
        + ImageColor(pq.u, pq.v + 1) * (1 - delta.u) * (delta.v)
        + ImageColor(pq.u + 1, pq.v + 1) * (delta.u) * (delta.v);
    return result;
}

Color ImageTexture::trilinear(Texel texel)
{
    // TODO: mipmapping
    return bilinear(texel);
}

//// Perlin Texture ////

Color PerlinTexture::TextureColor(const Vertex& vert, Texel& tex)
{
    real result = 0.0;
    Vertex vert2 = vert * NoiseScale;
    real pow_2_i = 1;
    for (int i = 0; i < NumOctaves; i++)
    {
        pow_2_i = 1 << i;
        result = result +  (real)pow(2, -i) * PerlinNoise::perlin(vert2.x * pow_2_i, vert2.y * pow_2_i, vert2.z * pow_2_i);
    }
    result = convertNoise(result);
    return Color(result, result, result);
}

int clampfloor(real i) { return (int) std::floor(i) & 255; }
real getFloatPart(real i) { return i - std::floor(i); }

real PerlinNoise::perlin(real x, real y, real z)
{
    int XYZ[3] = {clampfloor(x), clampfloor(y), clampfloor(z)};
    Vertex xyz = Vertex(getFloatPart(x), getFloatPart(y), getFloatPart(z));
    Vec3r uvw = fade(xyz);
    int A = P[XYZ[0]] + XYZ[1];
    int B = P[XYZ[0] + 1] + XYZ[1];
    int AA = P[A] + XYZ[2];
    int BA = P[B] + XYZ[2];
    int AB = P[A + 1] + XYZ[2];
    int BB = P[B + 1] + XYZ[2];
    std::array<real, 8> gradients = {
        grad(P[AA], xyz),
        grad(P[BA], {xyz.x - 1, xyz.y, xyz.z}),
        grad(P[AB], {xyz.x, xyz.y - 1, xyz.z}),
        grad(P[BB], {xyz.x - 1, xyz.y - 1, xyz.z}),
        grad(P[AA + 1], {xyz.x, xyz.y, xyz.z - 1}),
        grad(P[BA + 1], {xyz.x - 1, xyz.y, xyz.z - 1}),
        grad(P[AB + 1], {xyz.x, xyz.y - 1, xyz.z - 1}),
        grad(P[BB + 1], {xyz.x - 1, xyz.y - 1, xyz.z - 1}),

    };
    return lerp3D(uvw, gradients);
}

real PerlinNoise::grad(int hash, Vertex vert)
{
    int h = hash & 15;
    real u = h < 8 ? vert.x : vert.y;
    real v = h < 4 ? vert.y : h == 12 || h == 14 ? vert.x : vert.z;
    return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}


real PerlinNoise::fade(real t)
{
    t = fabs(t);
    if (t >= 1) return 0;
    return t * t * t * (t * (t * -6 + 15) - 10) + 1;
}

Vec3r PerlinNoise::fade(Vertex vert)
{
    Vec3r result = Vec3r(fade(vert.x), fade(vert.y), fade(vert.z));
    return result;
}

Color CheckerTexture::TextureColor(const Vertex& vert, Texel& tex)
{
    return IsOnWhite(vert) ? whiteColor : blackColor;
}

bool CheckerTexture::IsOnWhite(real i)
{
    return ((int) std::floor((i + offset) * scale)) % 2 == 1;
}
bool CheckerTexture::IsOnWhite(Vertex vert)
{
    bool xorXY = IsOnWhite(vert.x) != IsOnWhite(vert.y);
    return xorXY == IsOnWhite(vert.z);
}
