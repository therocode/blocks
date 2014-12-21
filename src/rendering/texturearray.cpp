#include "texturearray.hpp"
#include <fea/assert.hpp>
#include <utility>
#include <string>

TextureArray::TextureArray() :
    mId(0),
    mLayers(0)
{
}

TextureArray::TextureArray(TextureArray&& other) :
    mId(0),
    mLayers(0)
{
    std::swap(mId, other.mId);
    std::swap(mSize, other.mSize);
    std::swap(mLayers, other.mLayers);
}

TextureArray& TextureArray::operator=(TextureArray&& other)
{
    std::swap(mId, other.mId);
    std::swap(mSize, other.mSize);
    std::swap(mLayers, other.mLayers);
    return *this;
}

GLuint TextureArray::getId() const
{
    return mId;
}

void TextureArray::create(const glm::uvec2& size, uint32_t layers, const uint8_t* imageData, bool smooth)
{
    FEA_ASSERT(size.x > 0 && size.y > 0 && layers > 0, "Cannot create a texture with a width or height or layers smaller than zero! Given dimensions are " + std::to_string(size.x) + " " + std::to_string(size.y) + " " + std::to_string(layers));
    mSize = size;
    mLayers = layers;

    if(mId)
    {
        destroy();
    }

    glGenTextures(1, &mId);
    FEA_ASSERT(mId != 0, "Failed to create texture. Make sure there is a valid OpenGL context available!");
    glBindTexture(GL_TEXTURE_2D_ARRAY, mId);
    glTexImage3D(GL_TEXTURE_2D_ARRAY, 0, GL_RGBA, (GLsizei)size.x, (GLsizei)size.y, (GLsizei)layers, 0, GL_RGBA, GL_UNSIGNED_BYTE, imageData);

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, smooth ? GL_LINEAR : GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, smooth ? GL_LINEAR : GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D_ARRAY, 0);
}

const glm::uvec2& TextureArray::getSize() const
{
    return mSize;
}

uint32_t TextureArray::getLayers() const
{
    return mLayers;
}

void TextureArray::destroy()
{
    if(mId)
    {
        glDeleteTextures(1, &mId);
        mId = 0;
        mSize = glm::uvec2();
    }
}

TextureArray::~TextureArray()
{
    if(mId)
    {
        destroy();
    }
}
