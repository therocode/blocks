#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>

Renderer::Renderer(fea::MessageBus& messageBus) : bus(messageBus)
{
	bus.addMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.addMessageSubscriber<WindowResizeMessage>(*this);
}

Renderer::~Renderer()
{
	bus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.removeMessageSubscriber<WindowResizeMessage>(*this);
}

void Renderer::setup()
{
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	// glEnable(GL_LIGHTING);
	// glEnable(GL_LIGHT0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.f, 1, 0.1, 100);
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

void Renderer::handleMessage(const WindowResizeMessage& received)
{
    uint32_t height;
    uint32_t width;
	std::tie(height, width) = received.data;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.f, width/height, 0.1, 100);
	glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, heigh);
}

void Renderer::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
//	glLoadIdentity();
//	glTranslatef(-5.0f - hehe, -8.0f, -30.0f);

	for(auto& chunk : chunks)
	{
		chunk.DrawVBO();
	}
}

void Renderer::hehehe()
{
	hehe += 0.3f;
}
