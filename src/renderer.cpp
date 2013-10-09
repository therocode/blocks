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

    glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	projectionMatrix = glm::perspective(80.f, 1.f, 0.1f, 100.f);
	glMultMatrixf(glm::value_ptr(projectionMatrix));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}
void Renderer::setCameraMatrix(glm::mat4 m){
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(m));
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
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT );
//	glLoadIdentity();
//	glTranslatef(-5.0f - hehe, -8.0f, -30.0f);
	for(auto& chunk : chunks)
	{
		chunk.DrawVBO();
	}
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
}

void Renderer::hehehe()
{
	hehe += 0.3f;
}
