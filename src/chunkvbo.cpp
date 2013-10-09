#include "chunkvbo.h"

ChunkVBO::ChunkVBO()
{
    glGenBuffers(1, &verticesId);
    glGenBuffers(1, &texCoordsId);
    glGenBuffers(1, &normalsId);
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

void ChunkVBO::setTexCoordData(uint32_t dataSize, float* texCoordData)
{
    glBindBuffer(GL_ARRAY_BUFFER, texCoordsId);
    glBufferData(GL_ARRAY_BUFFER, dataSize, texCoordData, GL_STATIC_DRAW);
}

void ChunkVBO::setNormalData(uint32_t dataSize, float* normalData)
{
    glBindBuffer(GL_ARRAY_BUFFER, normalsId);
    glBufferData(GL_ARRAY_BUFFER, dataSize, normalData, GL_STATIC_DRAW);
}

GLuint ChunkVBO::getVerticesId() const
{
    return verticesId;
}

GLuint ChunkVBO::getTexCoordsId() const
{
    return texCoordsId;
}

GLuint ChunkVBO::getNormalsId() const
{
    return normalsId;
}

uint32_t ChunkVBO::getTriangleAmount() const
{
    return 16 * 16 * 16;
}
