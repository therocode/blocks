#pragma once
#include "glm.hpp"
#include "rectangle.hpp"
#include "../resources/texture.hpp"
#include <gimgui/data/element.hpp>
#include <gimgui/data/renderdata.hpp>
#include <gimgui/logic/renderdatagenerator.hpp>
#include <cstdint>

class IVec2Adaptor
{
    public:
        using Native = glm::ivec2;
        IVec2Adaptor() = default;
        IVec2Adaptor(int32_t x, int32_t y);
        IVec2Adaptor(const glm::ivec2& ivec2);
        IVec2Adaptor operator+(const IVec2Adaptor& other) const;
        int32_t x() const;
        int32_t y() const;
        void x(int32_t x);
        void y(int32_t y);
    private:
        glm::ivec2 mIVec2;
};

class RectangleAdaptor
{
    public:
        using Native = Rectangle;
        RectangleAdaptor() = default;
        RectangleAdaptor(const IVec2Adaptor& start, const IVec2Adaptor& size);
        RectangleAdaptor(const Rectangle& rectangle);
        IVec2Adaptor start() const;
        IVec2Adaptor size() const;
    private:
        Rectangle mRectangle;
};

class ColorAdaptor
{
    public:
        using Native = glm::u8vec4;
        ColorAdaptor() = default;
        ColorAdaptor(uint8_t r, uint8_t g, uint8_t b, uint8_t a = 255);
        ColorAdaptor(const glm::u8vec4& color);
        uint8_t r() const;
        uint8_t g() const;
        uint8_t b() const;
        uint8_t a() const;
    private:
        glm::u8vec4 mColor;
};

class TextureAdaptor
{   
    public:
        TextureAdaptor(Texture& texture);
        void initialize(uint32_t width, uint32_t height);
        void resize(uint32_t width, uint32_t height);
        void writeBitmap(uint32_t x, uint32_t y, const gim::BitMap& bitMap);
    private:
        Texture& mTexture;
};
