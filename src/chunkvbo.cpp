#include "chunkvbo.h"

ChunkVBO::ChunkVBO()
{
    glGenBuffers(1, &verticesId);
    glGenBuffers(1, &texCoordsId);
}

ChunkVBO::~ChunkVBO()
{
    //glDeleteBuffers(1, &verticesId);
    //glDeleteBuffers(1, &texCoordsId);
}

void ChunkVBO::setVertexData(uint32_t dataSize, float* vertexData)
{
    glBindBuffer(GL_ARRAY_BUFFER, verticesId);
    glBufferData(GL_ARRAY_BUFFER, dataSize, vertexData, GL_STATIC_DRAW);
}

void ChunkVBO::setTexCoordsData(uint32_t dataSize, float* texCoordsData)
{
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
    glBufferData(GL_ARRAY_BUFFER, dataSize, texCoordsData, GL_STATIC_DRAW);
}

GLuint ChunkVBO::getVerticesId() const
{
    return verticesId;
}

uint32_t ChunkVBO::getTriangleAmount() const
{
    return 16 * 16 * 16;
}
