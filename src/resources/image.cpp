#include "image.hpp"

Image::Image()
{
}

Image::Image(const glm::uvec2& size, const std::vector<uint8_t>& pixelData) :
    mSize(size),
    mPixels(pixelData)
{
}

Image::Image(const glm::uvec2& size, const fea::Color& color) :
    mSize(size),
    mPixels(size.x * size.y * 4)
{
    for(uint32_t x = 0; x < mSize.x; x++)
    {
        for(uint32_t y = 0; y < mSize.y; y++)
        {
            mPixels[(x + y * mSize.x) * 4 + 0] = color.r();
            mPixels[(x + y * mSize.x) * 4 + 1] = color.g();
            mPixels[(x + y * mSize.x) * 4 + 2] = color.b();
            mPixels[(x + y * mSize.x) * 4 + 3] = color.a();
        }
    }
}

const uint8_t* Image::getPixelsPointer() const
{
    return mPixels.data();
}

const glm::uvec2& Image::getSize() const
{
    return mSize;
}
