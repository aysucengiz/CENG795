//
// Created by vicy on 12/06/25.
//

#ifndef CENG795_TEXTUREMAP_H
#define CENG795_TEXTUREMAP_H

#include <functional>
#include <random>

#include "../base/SceneData.h"


namespace Convert
{
    real Abs(real inp);
    real Linear(real inp);
}

struct MipMap
{
    int width, height;
    std::vector<std::vector<Color>> colorData;
};

class Image
{
public:
    uint32_t _id;
    int channels_in_file;
    std::vector<MipMap> mipmaps;
    std::string filename;
    Image(uint32_t id, std::string filename);
    ~Image();

    void CreateMipMap();
};



class Texture
{
public:
    virtual ~Texture() = default;
    uint32_t _id;
    DecalMode decalMode;
    real bumpFactor = 1.0;
    Texture(uint32_t i, DecalMode d) : _id(i), decalMode(d)
    {}
    virtual Color TextureColor(const Vertex& vert, Texel& tex, real level) = 0;
    virtual TextureType getTextureType() = 0;
    virtual bool IsMipMapped() { return false; }
};

class ImageTexture : public Texture
{

private:
    Color bilinear(Texel texel, real level);
    Color trilinear(Texel texel, real level);
    Color nearest(Texel texel, real level);
    // TODO: galiba mipmapping optionalmış sona bırakalım
    std::function<Color(Texel,real)> interpolate;

public:
    Color ImageColor(int x, int y, int level, bool wrap = false);
    Interpolation interpolation;
    Image *image;
    real normalizer;
    Color TextureColor(const Vertex& vert, Texel& tex, real level) override;
    TextureType getTextureType() override;
    ImageTexture(uint32_t id, DecalMode d, Image *image, Interpolation interp, real normalizer);

    bool IsMipMapped() override { return image->mipmaps.size() > 1; }
};

class PerlinNoise
{
public:
    static int P[512];

    static real lerp(real t, real a, real b);
    static real grad(int hash, Vertex vert);
    static Vec3r fade(Vertex vert);
    static real fade(real t);
    static int perm[256];
    static real perlin(real x, real y, real z);
    static bool initialized;

    static void init()
    {
        if (!initialized) {
            for (int i=0; i < 256 ; i++) perm[i] = i;
            std::shuffle(perm, perm + 256, std::mt19937());
            for (int i=0; i < 256 ; i++) P[256+i] = P[i] = perm[i];
            initialized = true;
        }
        }
};

class PerlinTexture : public Texture
{
public:

    TextureType getTextureType() override;
    std::function<real(real)> convertNoise;
    real NoiseScale;
    int NumOctaves;
    PerlinTexture(uint32_t id,DecalMode d,std::function<real(real)> c, real ns, int no)
    : Texture(id, d), NumOctaves(no), convertNoise(c), NoiseScale(ns)
    {
        PerlinNoise::init();
    }
    Color TextureColor(const Vertex& vert, Texel& tex, real level) override;

};

class CheckerTexture : public Texture
{
public:
    TextureType getTextureType() override;
    Color blackColor;
    Color whiteColor;
    real scale;
    real offset;
    CheckerTexture(uint32_t id,DecalMode d, Color bc, Color wc, real s, real offs);
    Color TextureColor(const Vertex& vert, Texel& tex, real level) override;
    bool IsOnWhite(real i);
    bool IsOnWhite(Vertex vert);

};


#endif //CENG795_TEXTUREMAP_H