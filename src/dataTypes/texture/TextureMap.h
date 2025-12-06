//
// Created by vicy on 12/06/25.
//

#ifndef CENG795_TEXTUREMAP_H
#define CENG795_TEXTUREMAP_H

#include "../base/SceneData.h"

class Image
{
public:
    uint32_t id;
    const char *data;
};

struct NoiseConversion
{
};

class TextureMap
{
    uint32_t _id;
    TextureType type;
    DecalMode decalMode;

};

class ImageTextureMap : public TextureMap
{
    Image &image;
};

class NoiseTextureMap : public TextureMap
{
    NoiseConversion convert;
    real NoiseScale;
};


#endif //CENG795_TEXTUREMAP_H