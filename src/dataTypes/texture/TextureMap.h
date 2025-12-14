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


class Image
{
public:
    uint32_t _id;
    int channels_in_file;
    std::vector<std::vector<Color>> colorData;
    int width;
    int height;
    std::string filename;
    Image(uint32_t id, std::string filename);
    ~Image();
};



class Texture
{
public:
    virtual ~Texture() = default;
    uint32_t _id;
    DecalMode decalMode;
    Texture(uint32_t i, DecalMode d) : _id(i), decalMode(d)
    {}
    virtual Color TextureColor(const Vertex& vert, Texel& tex) = 0;
    virtual TextureType getTextureType() = 0;
};

class ImageTexture : public Texture
{

private:
    Color nearest(Texel texel);
    Color bilinear(Texel texel);
    Color trilinear(Texel texel);
    Color ImageColor(int x, int y);
    // TODO: galiba mipmapping optionalmış sona bırakalım
    std::function<Color(Texel)> interpolate;

public:
    Interpolation interpolation;
    Image *image;
    Color TextureColor(const Vertex& vert, Texel& tex) override;
    TextureType getTextureType() override;
    ImageTexture(uint32_t id, DecalMode d, Image *image, Interpolation interp): Texture(id, d), image(image)
    {
        interpolation = interp;
        if (interp == Interpolation::NEAREST)
            interpolate = [this](Texel t){ return nearest(t); };
        else if (interp == Interpolation::BILINEAR)
            interpolate = [this](Texel t){ return bilinear(t); };
        else if (interp == Interpolation::TRILINEAR)
            interpolate = [this](Texel t){ return trilinear(t); };
    };
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
            std::shuffle(perm, perm + 256, rand());
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
    Color TextureColor(const Vertex& vert, Texel& tex) override;

};

class CheckerTexture : public Texture
{
public:
    TextureType getTextureType() override;
    Color blackColor;
    Color whiteColor;
    real scale;
    real offset;
    CheckerTexture(uint32_t id,DecalMode d, Color bc, Color wc, real s, real offs) :
    Texture(id, d), blackColor(bc), whiteColor(wc), scale(s), offset(offs) {}
    Color TextureColor(const Vertex& vert, Texel& tex) override;
    bool IsOnWhite(real i);
    bool IsOnWhite(Vertex vert);

};


#endif //CENG795_TEXTUREMAP_H