#include "buffer.hpp"
#include <fea/assert.hpp>

Buffer::Buffer()
{
    glGenBuffers(1, &mBufferId);

    FEA_ASSERT(mBufferId != 0, "Generated zero buffer");
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
    glBindBuffer(GL_ARRAY_BUFFER, mBufferId);
}

void Buffer::setData(const std::vector<float>& data)
{
    bind();
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), data.data(), GL_STATIC_DRAW);
}
