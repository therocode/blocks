#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>
#include "../utilities/lodepng.h"

Renderer::Renderer(fea::MessageBus& messageBus) : bus(messageBus)
{
	mTimer.start();
	bus.addMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.addMessageSubscriber<WindowResizeMessage>(*this);
	bus.addMessageSubscriber<AddGfxEntityMessage>(*this);
	bus.addMessageSubscriber<MoveGfxEntityMessage>(*this);
	bus.addMessageSubscriber<RemoveGfxEntityMessage>(*this);
	bus.addMessageSubscriber<CurrentlyFacingBlockMessage>(*this);
	bus.addMessageSubscriber<PlayerConnectedToEntityMessage>(*this);
}

Renderer::~Renderer()
{
	bus.removeMessageSubscriber<ChunkCreatedMessage>(*this);
	bus.removeMessageSubscriber<WindowResizeMessage>(*this);
	bus.removeMessageSubscriber<AddGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<MoveGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<RemoveGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<CurrentlyFacingBlockMessage>(*this);
	bus.removeMessageSubscriber<PlayerConnectedToEntityMessage>(*this);
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
	glClearColor((float)0x00 / 255.f, (float)0xb2 / 255.f, (float)0xff / 255.f, 0.0f);

	//glEnable(GL_LIGHTING);
	//glEnable(GL_LIGHT0);
	//GLfloat lightpos[] = {100.f, 100.f, 100.f};
	//glLightfv(GL_LIGHT0, GL_POSITION, lightpos);
	//glEnable(GL_TEXTURE_2D);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//I create a projection matrix, instead of gluproejction.
	projectionMatrix = glm::perspective(80.f, 1.f, 0.1f, 100.f);
	//glMultMatrixf(glm::value_ptr(projectionMatrix));
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	makeTexture("data/textures/blocks.png", 256, 256, blockTexture);
	//Setting it to this because haha.
	cam.SetPosition(glm::vec3(0.0f, 50.0f, -50.0f));
	mShaderProgram.setShaderPaths("data/vert", "data/frag");
}
void Renderer::setCameraMatrix(glm::mat4 m){
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glMultMatrixf(glm::value_ptr(m));
}

void Renderer::handleMessage(const ChunkCreatedMessage& received)
{
	ChunkCoordinate coordinate;
	VoxelTypeArray types;

	std::tie(coordinate, types) = received.data;

    Chunk chunk(coordinate, types);

	VBOCreator vboCreator;
	vbos.push_back(vboCreator.generateChunkVBO(chunk));
}

void Renderer::handleMessage(const CurrentlyFacingBlockMessage& received)
{
	glm::vec3 v;
	std::tie(v) = received.data;
	mCurrentlyFacingBlock = v;
}
void Renderer::handleMessage(const WindowResizeMessage& received)
{
	uint32_t width;
	uint32_t height;
	std::tie(width, height) = received.data;
	//I create a projection matrix, instead of gluproejction.
	projectionMatrix = glm::perspective(80.f, ((float)width)/((float)height), 0.1f, 150.f);

	mScreenSize = glm::vec2((float)width, (float)height);

	//printf("Set screen size, %f, %f\n", screenSize.x, screenSize.y);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(70.0f, ((float)width)/((float)height), 0.1f, 100.0f);
	//glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);


}

void Renderer::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    //connect to player. right now it will always connect to player id 0, but that could be changed.

    size_t playerId;
    size_t entityId;

    std::tie(playerId, entityId) = received.data;

    if(playerId == 0)
    {
        mCameraEntity = entityId;
    }
}

void Renderer::render()
{
	bus.sendMessage<CameraUpdatedMessage>(CameraUpdatedMessage(cam.GetPosition(), cam.GetDirection()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	mShaderProgram.bind();
	mShaderProgram.setUniform("worldToCamera", cam.GetMatrix());
	mShaderProgram.setUniform("cameraToClip",  projectionMatrix);
	mShaderProgram.setUniform("modelToWorld",  glm::mat4(1.f));
	mShaderProgram.setTexture("tex0", blockTexture);
	mShaderProgram.setUniform("screenSize", mScreenSize);
	//I set these because easier to debug.
#ifndef EMSCRIPTEN
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(projectionMatrix));
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glMultMatrixf(glm::value_ptr(cam.GetMatrix()));
#endif

	glBindTexture(GL_TEXTURE_2D, blockTexture);

	for(auto& vbo : vbos)
	{
		vbo.DrawVBO(mShaderProgram);
	}
	float matr[16]=	{	1.f, 0.f, 0.f, 0.f, 
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	for(auto& billboard : billboards)
	{
		glm::vec3 cameraDir = billboard.second.mPosition - cam.GetPosition();
		cameraDir.y = 0;
        if(glm::length2(cameraDir) != 0)
		    cameraDir = glm::normalize(cameraDir);
        else
            cameraDir.z = 1.0f;
#if 1
		matr[12] = billboard.second.mPosition.x;
		matr[13] = billboard.second.mPosition.y;		
		matr[14] = billboard.second.mPosition.z;
		matr[0] = -cameraDir.z;
		matr[10]= -cameraDir.z;
		matr[2] =  cameraDir.x;		
		matr[8] = -cameraDir.x;
		mShaderProgram.setUniformMat4("modelToWorld",  &matr[0]);
#else
		glm::mat4 modelToWorld = glm::mat4(1.f);
		modelToWorld[3][0] = billboard.second.mPosition.x;
		modelToWorld[3][1] = billboard.second.mPosition.y;
		modelToWorld[3][2] = billboard.second.mPosition.z;
		modelToWorld[0][0] = -cameraDir.z;
		modelToWorld[2][2] = -cameraDir.z;
		modelToWorld[0][2] = cameraDir.x;
		modelToWorld[2][0] = -cameraDir.x;
		mShaderProgram.setUniform("modelToWorld",  modelToWorld);
#endif

		billboard.second.mVbo.DrawVBO(mShaderProgram);
	}
	mShaderProgram.unbind();
	glBindTexture(GL_TEXTURE_2D, 0);
	glDisable(GL_DEPTH_TEST);
	glPointSize(4.f);
	glColor3f(1.f, 0.f, 0.f);
	glBegin(GL_LINE_STRIP);
	glm::vec3 v = mCurrentlyFacingBlock;//glm::floor(cam.GetPosition()) + glm::vec3(0.5);

	glm::vec3 p = v + glm::vec3(0.5f);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-0.5f, 0.5f, 0.5f);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(0.5f, 0.5f, -0.5f);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(-0.5f, 0.5f, -0.5f);
	glVertex3f(p.x, p.y, p.z);

	glEnd();
	glEnable(GL_DEPTH_TEST);

}

void Renderer::cameraUpdate()
{
    cam.SetPosition(mCameraPosition);
    //camera must be updated from mPitch and mYaw
	cam.Update();

	setCameraMatrix(cam.GetMatrix());
}

void Renderer::handleMessage(const AddGfxEntityMessage& received)
{
	size_t id;
	glm::vec3 position;

	std::tie(id, position) = received.data;

	VBOCreator vboCreator;

	Billboard newBillboard(vboCreator.generateBoardVBO(glm::vec2(1.0f, 1.0f)), position);

	billboards.emplace(id, newBillboard);
}

void Renderer::handleMessage(const MoveGfxEntityMessage& received)
{
	size_t id;
	glm::vec3 position;

	std::tie(id, position) = received.data;

	billboards.at(id).mPosition = position;

    if(id == mCameraEntity)
    {
        mCameraPosition = position;
    }
}

void Renderer::handleMessage(const RotateGfxEntityMessage& received)
{
    size_t id;
    float pitch;
    float yaw;

    std::tie(id, pitch, yaw) = received.data;

    billboards.at(id).mPitch = pitch;
    billboards.at(id).mYaw = yaw;

    if(id == mCameraEntity)
    {
        mCameraPitch = pitch;
        mCameraYaw = yaw;
    }
}

void Renderer::handleMessage(const RemoveGfxEntityMessage& received)
{
	size_t id;

	std::tie(id) = received.data;

	billboards.at(id).mVbo.DestroyVBO();
	billboards.erase(id);
}
