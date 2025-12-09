//
// Created by vicy on 12/06/25.
//

#include "TextureMap.h"

#include "../../fileManagement/PPM.h"
#include "../../functions/overloads.h"
#include "../../functions/helpers.h"

real Convert::Abs(real inp) { return std::abs(inp); }
real Convert::Linear(real inp) { return inp; }

TextureType ImageTexture::getTextureType(){ return TextureType::IMAGE;}
TextureType PerlinTexture::getTextureType(){ return TextureType::PERLIN;}
TextureType CheckerTexture::getTextureType(){ return TextureType::CHECKERBOARD;}

/// IMAGE ////
Image::Image(uint32_t id, std::string filename) : _id(id)
{
    data = PPM::read_image(filename.c_str(), width, height, channels_in_file, 3);
    uint32_t curr_idx = 0;
    for (int y = 0; y < height; y++)
    {
        colorData.push_back(std::vector<Color>());
        for (int x = 0; x < height; x++)
        {
            // TODO: channels in file nasıl etkiliyor?
            colorData[y].push_back(Color(data[curr_idx], data[curr_idx + 1], data[curr_idx + 2]));
            curr_idx += 3;
        }
    }
}


/// IMAGE TEXTURE ////
Color ImageTexture::TextureColor(Vertex &vert, Texel &tex)
{
    return interpolate(tex);
}

Color ImageTexture::ImageColor(real x, real y)
{
    return image.colorData[y][x];
}

Color ImageTexture::nearest(Texel texel)
{
    return ImageColor(std::round(texel.u), std::round(texel.v));
}

Color ImageTexture::bilinear(Texel texel)
{
    Texel pq = {std::floor(texel.u), std::floor(texel.v)};
    Texel delta = {texel.u - pq.u, texel.v - pq.v};
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

int operator*(int _cpp_par_, int _cpp_par_);

Color PerlinTexture::TextureColor(Vertex& vert, Texel& tex)
{
    Color result = Color(0, 0, 0);
    vert = vert * NoiseScale;
    real pow_2_i;
    for (int i = 0; i < NumOctaves; i++)
    {
        pow_2_i = pow(2, i);
        result += (real)pow(2, -i) * PerlinNoise::perlin(vert.x * pow_2_i, vert.y * pow_2_i, vert.z * pow_2_i);
    }
    return result;
}

real clampfloor(real i) { return std::floor(i) & 255; }
real getFloatPart(real i) { return i - std::floor(i); }

real PerlinNoise::perlin(real x, real y, real z)
{
    Vertex XYZ = Vertex(clampfloor(x), clampfloor(y), clampfloor(z));
    Vertex xyz = Vertex(getFloatPart(x), getFloatPart(y), getFloatPart(z));
    Vec3r gradient = fade(xyz);
    int A = P[XYZ.x] + XYZ.y;
    int B = P[XYZ.x + 1] + XYZ.y;
    int AA = P[A] + XYZ.z;
    int BA = P[B] + XYZ.z;
    int AB = P[A + 1] + XYZ.z;
    int BB = P[B + 1] + XYZ.z;
    std::array<real, 8> all_gradients = {
        grad(P[AA], xyz),
        grad(P[BA], {xyz.x - 1, xyz.y, xyz.z}),
        grad(P[AB], {xyz.x, xyz.y - 1, xyz.z}),
        grad(P[BB], {xyz.x - 1, xyz.y - 1, xyz.z}),
        grad(P[AA + 1], {xyz.x, xyz.y, xyz.z - 1}),
        grad(P[BA + 1], {xyz.x - 1, xyz.y, xyz.z - 1}),
        grad(P[AB + 1], {xyz.x, xyz.y - 1, xyz.z - 1}),
        grad(P[BB + 1], {xyz.x - 1, xyz.y - 1, xyz.z - 1}),

    };
    return lerp3D(gradient, all_gradients);
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

Color CheckerTexture::TextureColor(Vertex& vert, Texel& tex)
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
