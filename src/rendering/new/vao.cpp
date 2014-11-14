#include "vao.hpp"
#include <fea/assert.hpp>

VAO::VAO()
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

void VAO::bind()
{
    glBindVertexArray(mVAOId);
}
