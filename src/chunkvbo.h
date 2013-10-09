#pragma once
#include <featherkit/rendering/opengl.h>

class ChunkVBO
{
    public:
        ChunkVBO();
        ~ChunkVBO();
        void setVertexData(uint32_t dataSize, float* vertexData);
        void setTexCoordsData(uint32_t dataSize, float* texCoordsData);
        GLuint getVerticesId() const;
        uint32_t getTriangleAmount() const;
    private:
        GLuint verticesId;
        GLuint texCoordsId;
};
