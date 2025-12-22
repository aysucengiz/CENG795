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

real Convert::Abs(real inp) {return std::min((real)1.0,std::abs(inp)); }
real Convert::Linear(real inp) { return std::min(1.0, std::max(inp * 0.5 + 0.5,0.0)); }

TextureType ImageTexture::getTextureType(){ return TextureType::IMAGE;}
TextureType PerlinTexture::getTextureType(){ return TextureType::PERLIN;}
TextureType CheckerTexture::getTextureType(){ return TextureType::CHECKERBOARD;}

/// IMAGE ////
Image::Image(uint32_t id, std::string filename) : _id(id), filename(filename)
{

    MipMap m0;
    unsigned char *data = PPM::read_image(filename.c_str(), m0.width, m0.height, channels_in_file, 3);

    if (data)
    {
        uint32_t curr_idx = 0;
        m0.colorData.resize(m0.height, std::vector<Color>(m0.width));
        for (int y = 0; y < m0.height; y++)
        {
            for (int x = 0; x < m0.width; x++)
            {
                m0.colorData[y][x] = Color(data[curr_idx], data[curr_idx+1], data[curr_idx+2]);
                curr_idx += 3;
            }
        }
        mipmaps.push_back(m0);
    }
    else std::cout << "Image could not be read" << std::endl;
}

Image::~Image()
{
}

void Image::CreateMipMap()
{
    int curr_level = 0;
    std::array<int,2> xs, ys;
    while (mipmaps[curr_level].width > 1 || mipmaps[curr_level].height > 1)
    {
        curr_level++;
        const MipMap& prev = mipmaps[curr_level-1];
        MipMap curr;

        curr.width  = std::max(1, prev.width  / 2);
        curr.height = std::max(1, prev.height / 2);

        curr.colorData.resize(curr.height, std::vector<Color>(curr.width));

        for (int y = 0; y < curr.height; y++)
        {
            for (int x = 0; x < curr.width; x++)
            {
                Color fin(0.0,0.0,0.0);

                xs = {
                    std::min(prev.width  - 1, 2 * x),
                    std::min(prev.width  - 1, 2 * x + 1)
                };

                ys = {
                    std::min(prev.height  - 1, 2 * y),
                    std::min(prev.height  - 1, 2 * y + 1)
                };

                for (int i = 0; i < 2; i++)
                    for (int j = 0; j < 2; j++)
                        fin += prev.colorData[ys[i]][xs[j]];

                curr.colorData[y][x] = fin * 0.25;
            }
        }
        mipmaps.push_back(curr);
    }
}


/// IMAGE TEXTURE ////
ImageTexture::ImageTexture(uint32_t id, DecalMode d, Image *image, Interpolation interp, real normalizer): Texture(id, d), image(image), normalizer(normalizer)
{
    interpolation = interp;
    if (interp == Interpolation::NEAREST)
    {
        interpolate = [this](Texel t, real l){ return nearest(t,l); };
    }
    else if (interp == Interpolation::BILINEAR)
    {
        interpolate = [this](Texel t, real l){ return bilinear(t,l); };
    }
    else if (interp == Interpolation::TRILINEAR)
    {
        interpolate = [this](Texel t, real l){ return trilinear(t,l); };
        if (image->mipmaps.size() <= 1) image->CreateMipMap();
    }
};


Color ImageTexture::TextureColor(const Vertex &vert, Texel &tex, real level)
{
    Texel tiled = {(real) fmod(tex.u,1.0), (real) fmod(tex.v,1.0)};
    Color interpolated_color = interpolate(tiled, level);
    return  interpolated_color / normalizer;
}

Color ImageTexture::ImageColor(int x, int y, int level, bool wrap)
{
    level = std::max(std::min((int) image->mipmaps.size()-1,level),0);
    MipMap& currMipMap = image->mipmaps[level];
    if (wrap)
    {
        while (x < 0) x += currMipMap.width;
        while (y < 0) y += currMipMap.height;
        while (x >= currMipMap.width) x -= currMipMap.width;
        while (y >= currMipMap.height) y -= currMipMap.height;
    }
    else
    {
        y = std::max(0, std::min(y , currMipMap.height-1));
        x = std::max(0, std::min(x , currMipMap.width-1));
    }

    Color c = currMipMap.colorData[y][x];
    return c;
}

Color ImageTexture::nearest(Texel tex, real level)
{
    Texel xy = {(real) fmod(tex.u,1.0) * (image->mipmaps[level].width), (real) fmod(tex.v,1.0)  * (image->mipmaps[level].height)};
    return ImageColor(std::round(xy.u - 0.5), std::round(xy.v - 0.5), level);
}

Color ImageTexture::bilinear(Texel tex, real level)
{
    Texel xy = {tex.u * (image->mipmaps[level].width), tex.v  * (image->mipmaps[level].height)};

    Texel pq = {std::floor(xy.u), std::floor(xy.v)};
    Texel delta = {xy.u - pq.u, xy.v - pq.v};
    Color result = ImageColor(pq.u, pq.v, level) * (1 - delta.u) * (1 - delta.v)
        + ImageColor(pq.u + 1, pq.v, level) * (delta.u) * (1 - delta.v)
        + ImageColor(pq.u, pq.v + 1, level) * (1 - delta.u) * (delta.v)
        + ImageColor(pq.u + 1, pq.v + 1, level) * (delta.u) * (delta.v);
    return result;
}

Color ImageTexture::trilinear(Texel tex, real level)
{
    // level *=5.0;

    int level0 = std::max(0,(int) std::floor(level));
    int level1 = std::min((int) image->mipmaps.size()-1,level0+1);
    real t = level - level0;
    Color c0, c1;
    c0 = bilinear(tex, level0);
    c1 = bilinear(tex, level1);
    Color fin = c0 * (1 - t) + c1 * (t);
    return fin;
}

//// Perlin Texture ////

Color PerlinTexture::TextureColor(const Vertex& vert, Texel& tex, real level)
{
    real result = 0.0;
    Vertex vert2 = vert * NoiseScale;
    int pow_2_i = 1;
    real pow_2__i = 1.0;
    for (int i = 0; i < NumOctaves; i++)
    {
        real perlin_res = PerlinNoise::perlin(vert2.x * pow_2_i, vert2.y * pow_2_i, vert2.z * pow_2_i);
        result = result +  pow_2__i * perlin_res;
        pow_2_i = pow_2_i << 1;
        pow_2__i *= 0.5;
    }
    result = convertNoise(result) ;
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
    return t * t * t * (t * (t * 6 - 15) + 10);
}

Vec3r PerlinNoise::fade(Vertex vert)
{
    Vec3r result = Vec3r(fade(vert.x), fade(vert.y), fade(vert.z));
    return result;
}


CheckerTexture::CheckerTexture(uint32_t id,DecalMode d, Color bc, Color wc, real s, real offs) :
Texture(id, d), blackColor(bc), whiteColor(wc), scale(s), offset(offs) {}

Color CheckerTexture::TextureColor(const Vertex& vert, Texel& tex, real level)
{

    return IsOnWhite(vert) ? whiteColor : blackColor;
 }

bool CheckerTexture::IsOnWhite(real i)
{
    real place = (i + offset) * scale;
    return ((int) std::floor(place) & 1) == 1;
}
bool CheckerTexture::IsOnWhite(Vertex vert)
{
    bool X = IsOnWhite(vert.x);
    bool Y = IsOnWhite(vert.y);
    bool Z = IsOnWhite(vert.z);
    bool xorXY = X ^ Y;
    return xorXY == Z;
}
