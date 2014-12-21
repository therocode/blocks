#include "image.hpp"

Image::Image()
{
}

Image::Image(const glm::uvec2& size, const std::vector<uint8_t>& pixelData) :
    mSize(size),
    mPixels(pixelData)
{
}

const uint8_t* Image::getPixelsPointer() const
{
    return mPixels.data();
}

const glm::uvec2& Image::getSize() const
{
    return mSize;
}
