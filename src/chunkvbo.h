#pragma once
#include <featherkit/rendering/opengl.h>

class ChunkVBO
{
    public:
        ChunkVBO();
        void setVertexData(uint32_t dataSize, float* vertexData);
        void setTexCoordsData(uint32_t dataSize, float* texCoordsData);
        ~ChunkVBO();
    private:
        GLuint verticesId;
        GLuint texCoordsId;
};
