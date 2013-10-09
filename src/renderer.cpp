#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>

void Renderer::setup()
{
    glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
}

void Renderer::addChunk(const Chunk& chunk)
{
    VBOCreator vboCreator;
    chunks.push_back(vboCreator.generateChunkVBO(chunk));
}

void Renderer::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(0.0f, 0.0f, -30.0f);

    for(auto& chunk : chunks)
    {
        glBindBuffer(GL_ARRAY_BUFFER, chunk.getVerticesId());
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, chunk.getTriangleAmount());
        glDisableClientState(GL_VERTEX_ARRAY);
        std::cout << "hej and id is " << chunk.getVerticesId() << " and amount is " << chunk.getTriangleAmount() << " and error is: " <<  glGetError() << "!\n";
    }
}
