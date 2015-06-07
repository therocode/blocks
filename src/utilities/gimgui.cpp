#include "gimgui.hpp"

IVec2Adaptor::IVec2Adaptor(int32_t x, int32_t y):
    mIVec2(x, y)
{
}

IVec2Adaptor::IVec2Adaptor(const glm::ivec2& ivec2):
    mIVec2(ivec2)
{
}

IVec2Adaptor  IVec2Adaptor::operator+(const IVec2Adaptor& other) const
{
    return mIVec2 + other.mIVec2;
}

int32_t IVec2Adaptor::x() const
{
    return mIVec2.x;
}

int32_t IVec2Adaptor::y() const
{
    return mIVec2.y;
}

void IVec2Adaptor::x(int32_t x)
{
    mIVec2.x = x;
}

void IVec2Adaptor::y(int32_t y)
{
    mIVec2.y = y;
}

RectangleAdaptor::RectangleAdaptor(const IVec2Adaptor& start, const IVec2Adaptor& size):
    mRectangle({{start.x(), start.y()}, {size.x(), size.y()}})
{
}

RectangleAdaptor::RectangleAdaptor(const Rectangle& rectangle):
    mRectangle(rectangle)
{
}

ColorAdaptor::ColorAdaptor(uint8_t r, uint8_t g, uint8_t b, uint8_t a):
    mColor(r, g, b, a)
{
}

ColorAdaptor::ColorAdaptor(const fea::Color& color):
    mColor(color)
{
}

uint8_t ColorAdaptor::r() const
{
    return mColor.r();
}

uint8_t ColorAdaptor::g() const
{
    return mColor.g();
}

uint8_t ColorAdaptor::b() const
{
    return mColor.b();
}

uint8_t ColorAdaptor::a() const
{
    return mColor.a();
}

IVec2Adaptor RectangleAdaptor::start() const
{
    return mRectangle.start;
}

IVec2Adaptor RectangleAdaptor::size() const
{
    return mRectangle.size;
}

TextureAdaptor::TextureAdaptor(Texture& texture):
    mTexture(texture)
{
}

void TextureAdaptor::initialize(uint32_t width, uint32_t height)
{
    mTexture.create(width, height, fea::Color::Transparent, false, true);
}

void TextureAdaptor::resize(uint32_t width, uint32_t height)
{
    mTexture.enlarge({width, height});
}

void TextureAdaptor::writeBitmap(uint32_t xPos, uint32_t yPos, const gim::BitMap& bitMap)
{
    for(uint32_t x = 0; x < bitMap.width; x++)
    {   
        for(uint32_t y = 0; y < bitMap.height; y++)
        {   
            uint8_t value = bitMap.pixels[x + y * bitMap.width];

            if(value != 0)
                mTexture.setPixel(xPos + x, yPos + y, fea::Color(value, value, value, value));
        }   
    }   

    mTexture.update();
}
