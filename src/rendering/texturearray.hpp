#pragma once
#include <stdint.h>
#include "opengl.hpp"
#include <fea/rendering/color.hpp>
#include "../utilities/glm.hpp"
#include <functional>
#include <memory>

class TextureArray
{
    public:
        TextureArray();
        TextureArray(const TextureArray& other) = delete;
        TextureArray& operator=(const TextureArray& other) = delete;
        TextureArray(TextureArray&& other);
        TextureArray& operator=(TextureArray&& other);
        GLuint getId() const;
        void create(const glm::uvec2& size, uint32_t layers, const uint8_t* imageData, bool smooth = false);
        const glm::uvec2& getSize() const;
        uint32_t getLayers() const;
        void destroy();
        ~TextureArray();
    private:
        GLuint mId;
        glm::uvec2 mSize;
        uint32_t  mLayers;

};
