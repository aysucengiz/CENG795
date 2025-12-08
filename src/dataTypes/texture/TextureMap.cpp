//
// Created by vicy on 12/06/25.
//

#include "TextureMap.h"

#include "../../fileManagement/PPM.h"

real Convert::Abs(real inp){ return std::abs(inp); }
real Convert::Linear(real inp){ return inp; }



/// IMAGE ////
Image::Image(uint32_t id, std::string filename) : _id(id)
{
    data = PPM::read_image(filename.c_str(), width, height, channels_in_file, 3);
    uint32_t curr_idx = 0;
    for (int y=0; y < height; y++)
    {
        colorData.push_back(std::vector<Color>());
        for (int x=0; x < height; x++)
        {
            // TODO: channels in file nasıl etkiliyor?
            colorData[y].push_back(Color(data[curr_idx],data[curr_idx+1],data[curr_idx+2]));
            curr_idx += 3;
        }
    }
}



/// IMAGE TEXTURE ////
Color ImageTexture::TextureColor(Vertex vert)
{
    return interpolate(texel);
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
    Texel pq = {std::floor(texel.u),std::floor(texel.v)};
    Texel delta = {texel.u - pq.u, texel.v - pq.v};
    Color result = ImageColor(pq.u,        pq.v) * (1 - delta.u) * (1 - delta.v)
                 + ImageColor(pq.u + 1,    pq.v) *     (delta.u) * (1 - delta.v)
                 + ImageColor(pq.u,    pq.v + 1) * (1 - delta.u) *     (delta.v)
                 + ImageColor(pq.u + 1,pq.v + 1) *     (delta.u) *     (delta.v);
    return result;
}

Color ImageTexture::trilinear(Texel texel)
{
    // TODO: mipmapping
    return bilinear(texel);
}

//// Perlin Texture ////

Color PerlinTexture::TextureColor(Vertex vert)
{
    Color result = Color(0, 0, 0);
    for (int i = 0; i < NumOctaves; i++) result += pow(2,-i) * perlin(vert.x*2,vert.y*2, vert.z*2);
    return result;
}

Color PerlinTexture::perlin(real x, real y, real z)
{

}
