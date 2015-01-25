#include "buffer.hpp"
#include <fea/assert.hpp>

Buffer::Buffer(int32_t type) :
    mType(type),
    mElementAmount(0)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");
}

Buffer::Buffer(const std::vector<float>& data, int32_t type) :
    mType(type)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");

    setData(data);
}

Buffer::Buffer(const std::vector<uint32_t>& data, int32_t type)  :
    mType(type)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");

    setData(data);
}

Buffer::Buffer(const std::vector<uint8_t>& data, int32_t type)  :
    mType(type)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");

    setData(data);
}

Buffer::~Buffer()
{
    if(mBufferId)
        glDeleteBuffers(1, &mBufferId);
}

Buffer::Buffer(Buffer&& other) : 
    mBufferId(0),
    mType(0),
    mElementAmount(0)
{
    std::swap(mBufferId, other.mBufferId);
    std::swap(mType, other.mType);
    std::swap(mElementAmount, other.mElementAmount);
}

Buffer& Buffer::operator=(Buffer&& other)
{
    std::swap(mBufferId, other.mBufferId);
    std::swap(mType, other.mType);
    std::swap(mElementAmount, other.mElementAmount);

    return *this;
}

GLuint Buffer::getId() const
{
    return mBufferId;
}

void Buffer::bind() const
{
    glBindBuffer(mType == ARRAY_BUFFER ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, mBufferId);
}

void Buffer::setData(const std::vector<float>& data)
{
    mElementAmount = data.size();
    bind();
    glBufferData(mType == ARRAY_BUFFER ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

void Buffer::setData(const std::vector<uint32_t>& data)
{
    mElementAmount = data.size();
    bind();
    glBufferData(mType == ARRAY_BUFFER ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32_t), data.data(), GL_STATIC_DRAW);
}

void Buffer::setData(const std::vector<uint8_t>& data)
{
    mElementAmount = data.size();
    bind();
    glBufferData(mType == ARRAY_BUFFER ? GL_ARRAY_BUFFER : GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint8_t), data.data(), GL_STATIC_DRAW);
}

int32_t Buffer::getElementAmount() const
{
    return mElementAmount;
}
