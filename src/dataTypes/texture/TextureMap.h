//
// Created by vicy on 12/06/25.
//

#ifndef CENG795_TEXTUREMAP_H
#define CENG795_TEXTUREMAP_H

#include <functional>

#include "../base/SceneData.h"


namespace Convert
{
    real Abs(real inp);
    real Linear(real inp);
}

namespace Interpolate
{

}


class Image
{
public:
    uint32_t _id;
    unsigned char *data;
    int channels_in_file;
    std::vector<std::vector<Color>> colorData;
    int width;
    int height;
    Image(uint32_t id, std::string filename);
};

struct Decal
{
    DecalMode decalMode;
};

class Texture
{
public:
    virtual ~Texture() = default;
    uint32_t _id;
    TextureType type;
    Decal decal;
    Texture(uint32_t i, TextureType t, DecalMode d) : _id(i), type(t), decal(d) {}
    virtual Color TextureColor(Vertex vert) = 0;
};

class ImageTexture : public Texture
{
public:
    Color nearest(Texel texel);
    Color bilinear(Texel texel);
    Color trilinear(Texel texel);
    Color TextureColor(Vertex vert) override;
    Color ImageColor(real x, real y);
    Image &image;
    // TODO: galiba mipmapping optionalmış sona bırakalım
    std::function<Color(Texel)> interpolate;

    ImageTexture(uint32_t id, TextureType t, DecalMode d, Image &image, Interpolation interp): Texture(id, t, d), image(image)
    {
        if (interp == Interpolation::NEAREST)
            interpolate = [this](Texel t){ return nearest(t); };
        else if (interp == Interpolation::BILINEAR)
            interpolate = [this](Texel t){ return bilinear(t); };
        else if (interp == Interpolation::TRILINEAR)
            interpolate = [this](Texel t){ return trilinear(t); };
    };
};

class PerlinTexture : public Texture
{
public:
    static int P[512];

    std::function<real(real)> convertNoise;
    real NoiseScale;
    int NumOctaves;
    PerlinTexture(uint32_t id, TextureType t, DecalMode d,std::function<real(real)> c, real ns, int no) : Texture(id, t, d), NumOctaves(no), convertNoise(c), NoiseScale(ns) {}
    Color TextureColor(Vertex vert) override;
    Color perlin(real x, real y, real z);
};

class CheckerTexture : public Texture
{
public:
    Color blackColor;
    Color whiteColor;
    real scale;
    real offset;
    CheckerTexture(uint32_t id, TextureType t, DecalMode d, Color bc, Color wc, real s, real offs) :
    Texture(id, t, d), blackColor(bc), whiteColor(wc), scale(s), offset(offs) {}
    Color TextureColor(Vertex vert) override;

};


#endif //CENG795_TEXTUREMAP_H