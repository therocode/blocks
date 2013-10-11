#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>
#include "lodepng.h"

Renderer::Renderer(fea::MessageBus& messageBus) : bus(messageBus)
{
	mTimer.start();
	movingUp = movingLeft = movingRight = movingDown = false;
	bus.addMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.addMessageSubscriber<WindowResizeMessage>(*this);
	bus.addMessageSubscriber<InputActionMessage>(*this);
	bus.addMessageSubscriber<MouseMovedMessage>(*this);
}

Renderer::~Renderer()
{
	bus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.removeMessageSubscriber<WindowResizeMessage>(*this);
	bus.removeMessageSubscriber<InputActionMessage>(*this);
	bus.removeMessageSubscriber<MouseMovedMessage>(*this);
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
	//I create a projection matrix, instead of gluproejction.
	projectionMatrix = glm::perspective(80.f, 1.f, 0.1f, 100.f);
	glMultMatrixf(glm::value_ptr(projectionMatrix));
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
	int dT = mTimer.getDeltaTime();
	camSpeed *= 0.999f;

	float moveSpeedT = (float) dT / 1000.f * moveSpeed;
	if(movingRight){
		m.x += moveSpeedT;	
	}
	if(movingLeft){
		m.x -= moveSpeedT;	
	}
	if(movingUp){
		m.z += moveSpeedT;	
	}
	if(movingDown){
		m.z -= moveSpeedT;	
	}
	if(elevate){
		m.y += moveSpeedT;
	}
	if(delevate){
		m.y -= moveSpeedT;
	}
	camSpeed += m;

	cam.MoveForward(camSpeed.z);
	cam.Strafe(camSpeed.x);
	
	cam.AddPosition(glm::vec3(0, camSpeed.y, 0));	

	cam.Update();

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
        case InputAction::MOUSELEFT:
            mouseDown = true;
        break;
        case InputAction::STOPMOUSELEFT:
            mouseDown = false;
        break;
    }
}

void Renderer::handleMessage(const MouseMovedMessage& received)
{
    float newX, newY;
    float mspeed = 0.01f;
	std::tie(newX, newY) = received.data;
    if(mouseDown)
        cam.AddDirection((newX - lastX)  * mspeed,(newY - lastY) * mspeed);
    lastX = newX;
    lastY = newY;
}
