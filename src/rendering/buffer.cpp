#include "buffer.hpp"
#include <fea/assert.hpp>

Buffer::Buffer(int32_t type) :
    mType(type),
    mElementAmount(0)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");
}

Buffer::Buffer(const std::vector<float>& data) :
    mType(ARRAY_BUFFER)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");

    setData(data);
}

Buffer::Buffer(const std::vector<uint32_t>& data) :
    mType(ELEMENT_ARRAY_BUFFER)
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");

    setData(data);
}

Buffer::~Buffer()
{
    glDeleteBuffers(1, &mBufferId);
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
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}

void Buffer::setData(const std::vector<uint32_t>& data)
{
    mElementAmount = data.size();
    bind();
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, data.size() * sizeof(uint32_t), data.data(), GL_STATIC_DRAW); //ELEMENT!!! NOOO, it should lasdk
}

int32_t Buffer::getElementAmount() const
{
    return mElementAmount;
}
