#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>

void Renderer::setup()
{
    glewInit();
    glTranslatef(0.0f, 0.0f, 30.0f);
}

void Renderer::addChunk(const Chunk& chunk)
{
    VBOCreator vboCreator;
    chunks.push_back(vboCreator.generateChunkVBO(chunk));
}

void Renderer::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    for(auto& chunk : chunks)
    {
        std::cout << "hej!\n";
        glBindBuffer(GL_ARRAY_BUFFER, chunk.getVerticesId());
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, chunk.getTriangleAmount());
    }
}
