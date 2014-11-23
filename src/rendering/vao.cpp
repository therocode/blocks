#include "vao.hpp"
#include <fea/assert.hpp>

VAO::VAO() :
    mIsBound(false) 
{
    glGenVertexArrays(1, &mVAOId);

    FEA_ASSERT(mVAOId != 0, "Generated zero vao");
}

VAO::~VAO()
{
    glDeleteVertexArrays(1, &mVAOId);
}

GLuint VAO::getId() const
{
    return mVAOId;
}

bool VAO::isBound() const
{
    return mIsBound;
}

void VAO::bind()
{
    glBindVertexArray(mVAOId);
    mIsBound = true;
}

void VAO::unbind()
{
    glBindVertexArray(0);
    mIsBound = false;
}

void VAO::setVertexAttribute(GLuint index, const uint32_t floatAmount, const float* data)
{
    bool wasBound = isBound();

    if(!isBound())
        bind();

    glEnableVertexAttribArray(index);
    mEnabledVertexAttributes.push_back(index);
    glVertexAttribPointer(index, floatAmount, GL_FLOAT, false, 0, data);

    if(!wasBound)
        unbind();
}

void VAO::setVertexAttribute(GLuint index, const uint32_t floatAmount, const Buffer& dataBuffer)
{
    bool wasBound = isBound();

    if(!isBound())
        bind();

    glEnableVertexAttribArray(index);
    mEnabledVertexAttributes.push_back(index);
    dataBuffer.bind();
    glVertexAttribPointer(index, floatAmount, GL_FLOAT, false, 0, nullptr);

    if(!wasBound)
        unbind();
}

void VAO::setInstanceAttribute(GLuint index, const uint32_t floatAmount, const Buffer& dataBuffer, uint32_t divisor)
{
    bool wasBound = isBound();

    if(!isBound())
        bind();

    glEnableVertexAttribArray(index);
    mEnabledVertexAttributes.push_back(index);
    dataBuffer.bind();
    glVertexAttribPointer(index, floatAmount, GL_FLOAT, false, 0, nullptr);
    glVertexAttribDivisor(index, divisor);

    if(!wasBound)
        unbind();
}
