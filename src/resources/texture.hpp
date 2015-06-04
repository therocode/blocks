#pragma once
#include <stdint.h>
#include "../rendering/opengl.hpp"
#include <fea/rendering/color.hpp>
#include "../utilities/glm.hpp"
#include <functional>
#include <memory>

class Texture
{
    public:
        Texture();
        Texture(const Texture& other) = delete;
        Texture& operator=(const Texture& other) = delete;
        Texture(Texture&& other);
        Texture& operator=(Texture&& other);
        GLuint getId() const;
        void create(uint32_t width, uint32_t height, const uint8_t* imageData, bool smooth = false, bool interactive = false);
        void create(uint32_t width, uint32_t height, const fea::Color& color, bool smooth = false, bool interactive = false);
        glm::uvec2 getSize() const;
        void destroy();
        void setPixel(uint32_t x, uint32_t y, const fea::Color& color);
        fea::Color getPixel(uint32_t x, uint32_t y) const;
        uint8_t* getPixelData();
        const uint8_t* getPixelData() const;
        void update();
        ~Texture();
        void enlarge(const glm::uvec2& newSize);
    private:
        GLuint mId;
        uint32_t mWidth;
        uint32_t mHeight;
        bool mInteractive;
        std::unique_ptr<uint8_t[]> pixelData;
};
