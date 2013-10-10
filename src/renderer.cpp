#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>
#include "lodepng.h"

Renderer::Renderer(fea::MessageBus& messageBus) : bus(messageBus)
{
	movingUp = movingLeft = movingRight = movingDown = false;
	bus.addMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.addMessageSubscriber<WindowResizeMessage>(*this);
	bus.addMessageSubscriber<InputActionMessage>(*this);
}

Renderer::~Renderer()
{
	bus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.removeMessageSubscriber<WindowResizeMessage>(*this);
	bus.removeMessageSubscriber<InputActionMessage>(*this);
}

void Renderer::makeTexture(std::string path, uint32_t width, uint32_t height, GLuint& textureId)
{
    std::vector<unsigned char> image; //the raw pixels

    //decode
    unsigned error = lodepng::decode(image, width, height, path);

    //if there's an error, display it
    if(error) std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;

    //the pixels are now in the vector "image", 4 bytes per pixel, ordered RGBARGBA..., use it as texture, draw it, ...

    glGenTextures(1, &textureId);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, (GLsizei)width, (GLsizei)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &image[0]);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::setup()
{
	glewInit();
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
    glEnable(GL_TEXTURE_2D);

    glEnable(GL_DEPTH_TEST);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0f, 1.0f, 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

    makeTexture("data/textures/blocks.png", 256, 256, blockTexture);
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
    uint32_t width;
    uint32_t height;
	std::tie(width, height) = received.data;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, ((float)width)/((float)height), 0.1f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
    glViewport(0, 0, width, height);
}

void Renderer::render()
{
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

    glBindTexture(GL_TEXTURE_2D, blockTexture);

	for(auto& chunk : chunks)
	{
		chunk.DrawVBO();
	}
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::cameraUpdate()
{
	glm::vec3 m;
	if(movingRight){
		m.x += moveSpeed;	
	}
	if(movingLeft){
		m.x -= moveSpeed;	
	}
	if(movingUp){
		m.z += moveSpeed;	
	}
	if(movingDown){
		m.z -= moveSpeed;	
	}
	if(elevate){
		m.y += moveSpeed;
	}
	if(delevate){
		m.y -= moveSpeed;
	}
	camSpeed += m;

	cam.MoveForward(camSpeed.z);
	cam.Strafe(camSpeed.x);
	
	cam.AddPosition(glm::vec3(0, camSpeed.y, 0));	

	cam.Update();
	camSpeed *= 0.995f;

	setCameraMatrix(cam.GetMatrix());

}

void Renderer::handleMessage(const InputActionMessage& received)
{
    int action;
	std::tie(action) = received.data;

    switch(action)
    {
        case InputAction::FORWARDS:
            movingUp = true;
        break;
        case InputAction::BACKWARDS:
            movingDown = true;
        break;
        case InputAction::LEFT:
            movingLeft = true;
        break;
        case InputAction::RIGHT:
            movingRight = true;
        break;
        case InputAction::JUMP:
            elevate = true;
        break;
        case InputAction::CROUCH:
            delevate = true;
        break;

        case InputAction::STOPFORWARDS:
            movingUp = false;
        break;
        case InputAction::STOPBACKWARDS:
            movingDown = false;
        break;
        case InputAction::STOPLEFT:
            movingLeft = false;
        break;
        case InputAction::STOPRIGHT:
            movingRight = false;
        break;
        case InputAction::STOPJUMP:
            elevate = false;
        break;
        case InputAction::STOPCROUCH:
            delevate = false;
        break;
    }
}
