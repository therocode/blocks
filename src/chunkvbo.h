#pragma once
#include <featherkit/rendering/opengl.h>

class ChunkVBO
{
    public:
        ChunkVBO();
        ~ChunkVBO();
        void setVertexData(uint32_t dataSize, float* vertexData);
        void setTexCoordData(uint32_t dataSize, float* texCoordsData);
        void setNormalData(uint32_t dataSize, float* normalData);
        GLuint getVerticesId() const;
        GLuint getTexCoordsId() const;
        GLuint getNormalsId() const;
        uint32_t getTriangleAmount() const;
    private:
        GLuint verticesId;
        GLuint texCoordsId;
        GLuint normalsId;
};
