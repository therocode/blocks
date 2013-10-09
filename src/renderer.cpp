#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>

Renderer::Renderer(fea::MessageBus& messageBus) : bus(messageBus)
{
    bus.addMessageSubscriber<ChunkCreatedMessage>(*this);
}

Renderer::~Renderer()
{
    bus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
}

void Renderer::setup()
{
    glewInit();
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

   // glEnable(GL_LIGHTING);
   // glEnable(GL_LIGHT0);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(80.f,1,0.1,100);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void Renderer::handleMessage(const ChunkCreatedMessage& received)
{
    const ChunkCoordinate* coordinate;
    const Chunk* chunk;

    std::tie(coordinate, chunk) = received.data;
    VBOCreator vboCreator;
    chunks.push_back(vboCreator.generateChunkVBO(*chunk));
}

void Renderer::render()
{
    glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    glLoadIdentity();
    glTranslatef(-5.0f - hehe, -8.0f, -30.0f);

    for(auto& chunk : chunks)
    {
		chunk.DrawVBO();
        /*glBindBuffer(GL_ARRAY_BUFFER, chunk.getVerticesId());
        glEnableClientState(GL_VERTEX_ARRAY);
        glVertexPointer(3, GL_FLOAT, 0, 0);

        glBindBuffer(GL_ARRAY_BUFFER, chunk.getNormalsId());
        glEnableClientState(GL_NORMAL_ARRAY);
        glNormalPointer(GL_FLOAT, 0, 0);

        glDrawArrays(GL_TRIANGLES, 0, chunk.getTriangleAmount());
        glDisableClientState(GL_VERTEX_ARRAY);
        glDisableClientState(GL_NORMAL_ARRAY);
        std::cout << "hej and id is " << chunk.getVerticesId() << " and amount is " << chunk.getTriangleAmount() << " and error is: " <<  glGetError() << "!\n";
    	*/
	}
}

void Renderer::hehehe()
{
    hehe += 0.3f;
}
