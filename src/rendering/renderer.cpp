#include "renderer.h"
#include "vbocreator.h"
#include <featherkit/rendering/opengl.h>
#include <iostream>
#include "../utilities/lodepng.h"


DebugRenderer Renderer::sDebugRenderer;
Renderer::Renderer(fea::MessageBus& messageBus) : bus(messageBus), mPlayerId(-1)
{
	mTimer.start();
	bus.addMessageSubscriber<UpdateChunkVboMessage>(*this);
	bus.addMessageSubscriber<ChunkDeletedMessage>(*this);
	bus.addMessageSubscriber<WindowResizeMessage>(*this);
	bus.addMessageSubscriber<AddGfxEntityMessage>(*this);
	bus.addMessageSubscriber<MoveGfxEntityMessage>(*this);
	bus.addMessageSubscriber<RotateGfxEntityMessage>(*this);
	bus.addMessageSubscriber<RemoveGfxEntityMessage>(*this);
	bus.addMessageSubscriber<PlayerFacingBlockMessage>(*this);
	bus.addMessageSubscriber<PlayerIdMessage>(*this);
	bus.addMessageSubscriber<PlayerConnectedToEntityMessage>(*this);
	mCameraPitch = 0;
	mCameraYaw = 0;
}

Renderer::~Renderer()
{
	bus.removeMessageSubscriber<UpdateChunkVboMessage>(*this);
	bus.removeMessageSubscriber<ChunkDeletedMessage>(*this);
	bus.removeMessageSubscriber<WindowResizeMessage>(*this);
	bus.removeMessageSubscriber<AddGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<MoveGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<RotateGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<RemoveGfxEntityMessage>(*this);
	bus.removeMessageSubscriber<PlayerFacingBlockMessage>(*this);
	bus.removeMessageSubscriber<PlayerIdMessage>(*this);
	bus.removeMessageSubscriber<PlayerConnectedToEntityMessage>(*this);
}

void Renderer::makeTexture(const std::string& path, uint32_t width, uint32_t height, GLuint& textureId)
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
	GLenum err = glewInit();
	if(GLEW_OK != err)
	{
		fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
	}else
	{
		fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
	}
	
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
	projectionMatrix = glm::perspective(80.f, 1.f, 0.1f, 1000.f);
    mFrustum.setCamInternals(80.f, 1.f, 0.1f, 1000.f);
	//glMultMatrixf(glm::value_ptr(projectionMatrix));
	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();

	makeTexture("data/textures/blocks.png", 256, 256, blockTexture);
	//Setting it to this because haha.
	cam.SetPosition(glm::vec3(0.0f, 50.0f, -50.0f));
	mShaderProgram.setShaderPaths("data/vert", "data/frag");	
}
void Renderer::setCameraMatrix(const glm::mat4& m){

	//glMatrixMode(GL_MODELVIEW);
	//glLoadIdentity();
	//glMultMatrixf(glm::value_ptr(m));
}

void Renderer::handleMessage(const UpdateChunkVboMessage& received)
{
    Chunk* mainChunk;
    Chunk* topChunk;
    Chunk* bottomChunk;
    Chunk* frontChunk;
    Chunk* backChunk;
    Chunk* leftChunk;
    Chunk* rightChunk;

    std::tie(mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk) = received.data;

    VBOCreator vboCreator;

    auto vboEntry = vbos.find(mainChunk->getLocation());

    if(vboEntry == vbos.end())
    {
        vbos.emplace(mainChunk->getLocation(), vboCreator.generateChunkVBO(mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk));
    }
    else
    {
        vbos.at(mainChunk->getLocation()).destroyBuffers();
        vbos.erase(mainChunk->getLocation());
        vbos.emplace(mainChunk->getLocation(), vboCreator.generateChunkVBO(mainChunk, topChunk, bottomChunk, frontChunk, backChunk, leftChunk, rightChunk));
    }
}

void Renderer::handleMessage(const ChunkDeletedMessage& received)
{
    ChunkCoordinate coordinate;

    std::tie(coordinate) = received.data;

    vbos.at(coordinate).destroyBuffers();
    vbos.erase(coordinate);
}

void Renderer::handleMessage(const PlayerFacingBlockMessage& received)
{
	glm::vec3 v;
    size_t playerId;

	std::tie(playerId, v) = received.data;

    if(playerId == mPlayerId)
	    mCurrentlyFacingBlock = v;
}
void Renderer::handleMessage(const WindowResizeMessage& received)
{
	uint32_t width;
	uint32_t height;
	std::tie(width, height) = received.data;
	//I create a projection matrix, instead of gluproejction.
	projectionMatrix = glm::perspective(80.f, ((float)width)/((float)height), 0.1f, 1000.f);
    mFrustum.setCamInternals(80.f, ((float)width)/((float)height), 0.1f, 1000.f);

	mScreenSize = glm::vec2((float)width, (float)height);

	//printf("Set screen size, %f, %f\n", screenSize.x, screenSize.y);

	//glMatrixMode(GL_PROJECTION);
	//glLoadIdentity();
	//gluPerspective(70.0f, ((float)width)/((float)height), 0.1f, 100.0f);
	//glMatrixMode(GL_MODELVIEW);
	glViewport(0, 0, width, height);


}

void Renderer::handleMessage(const PlayerIdMessage& received)
{
    size_t playerId;

    std::tie(mPlayerId) = received.data;
}

void Renderer::handleMessage(const PlayerConnectedToEntityMessage& received)
{
    size_t playerId;
    size_t entityId;

    std::tie(playerId, entityId) = received.data;

    if(playerId == mPlayerId)
    {
        mCameraEntity = entityId;
    }
}

void Renderer::render()
{
	GLenum err = glGetError();
	if(err != GL_NO_ERROR){
		printf("Some GL error here: %x\n.", err);
	}

    // glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

	glm::vec3 fullColor = glm::vec3((float)0x00 / 255.f, (float)0xb2 / 255.f, (float)0xff / 255.f);
	glm::vec3 cameraOffset = glm::vec3(0, 0.6f, 0);
	float time = mTimer.getTime() * 1.2f;
	float dT_f = mTimer.getDeltaTime();
	glm::vec3 mV = glm::cross(cam.GetDirection(), glm::vec3(0, 1, 0));
	float sc = 0.4f;
	if(speed > 0){
		cameraOffset.y += glm::sin(time*0.01f)*speed * sc;
		cameraOffset.x += glm::cos(time*0.005f)*speed * sc;
		cameraOffset.z += glm::sin(time*0.005f)*speed * sc;
	}
    for(float t = 0; t < dT_f; t += 1000.f/60.f)
	interpDuck += (duck - interpDuck) * 0.2f;
	cameraOffset.y -= interpDuck;
	duck *= 0.9f;
	cam.SetPosition(originalCameraPos + cameraOffset);
	float y = cam.GetPosition().y;
	glm::vec3 color = glm::mix(fullColor, glm::vec3(0.f), 1.f - glm::clamp((y - 15) * 0.05f, 0.f, 1.f));
	// printf("color: %f, %f, %f\n", color.x, color.y, color.z);
	glClearColor(color.x, color.y, color.z, 1.0f);
	
	bus.sendMessage<CameraUpdatedMessage>(CameraUpdatedMessage(cam.GetPosition(), cam.GetDirection()));
	glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	mShaderProgram.bind();
	cam.Update();

    const glm::vec3& position = cam.GetPosition();
    glm::vec3 lookat = position + glm::normalize(cam.GetDirection());
    glm::vec3 up(0.0f, 1.0f, 0.0f);
    mFrustum.setCamDef(position, lookat, up);

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

	ChunkCoordinate currentChunk = worldToChunk(cam.GetPosition());

	//For looping texture atlas textures, used in chunks.
	mShaderProgram.setUniform("enableBoundsTexture", 1);
	
	for(auto chunk : vbos)
	{	
		const ChunkCoordinate& p = chunk.first;
        glm::vec3 worldPos = (glm::vec3)(p * chunkWidth) + glm::vec3(chunkWidth / 2, chunkWidth / 2, chunkWidth / 2);

        if(mFrustum.sphereInFrustum(worldPos, chunkWidth / 2) != Frustum::OUTSIDE)
        {
          chunk.second.draw(mShaderProgram);
        }
        else
        {
            //std::cout << "culled\n";
        }
	}
//	for(auto& vbo : vbos)
//	{
//		vbo.second.DrawVBO(mShaderProgram);
//	}
	float matr[16]=	{	
		1.f, 0.f, 0.f, 0.f, 
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f
	};
	
	mShaderProgram.setUniform("enableBoundsTexture", 0);
	for(auto& billboard : billboards)
	{
        if(billboard.first == mCameraEntity)
            continue;

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

		billboard.second.mVbo.draw(mShaderProgram);
	}
	mShaderProgram.unbind();
	
	sDebugRenderer.performDrawing();
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	glBlendEquation(GL_FUNC_ADD);
	glBindTexture(GL_TEXTURE_2D, 0);
	//glDisable(GL_DEPTH_TEST);
	glPointSize(4.f);
	
	glBegin(GL_QUADS);
	if(mCurrentlyFacingBlock != mLastFacingBlock)
	{
		mLastFacingBlock = mCurrentlyFacingBlock;
		fadeIn = 0;
	}else if(fadeIn < 0.2f){
		fadeIn +=  dT_f * 0.002f;
	}else fadeIn = 0.2f;
	glColor4f(1.f, 1.f, 1.f, fadeIn);
	glm::vec3 v = mCurrentlyFacingBlock + glm::vec3(0.5f);//glm::floor(cam.GetPosition()) + glm::vec3(0.5);
	float s =  (0.2f - fadeIn) * 2.f + 0.51f ;
	//top
	glm::vec3 p = v + glm::vec3(s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, s, -s);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(-s, s, -s);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(-s, s, s);
	glVertex3f(p.x, p.y, p.z);
	
	//bottom
	p = v + glm::vec3(-s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, -s, -s);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(s, -s, s);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(-s, -s, s);
	glVertex3f(p.x, p.y, p.z);
	
	//left
	p = v + glm::vec3(-s, s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-s, -s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-s, -s, s);
	glVertex3f(p.x, p.y, p.z);

	p = v + glm::vec3(-s, s, s);
	glVertex3f(p.x, p.y, p.z);
	
	//right
	p = v + glm::vec3(s, -s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, s, s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, -s, s);
	glVertex3f(p.x, p.y, p.z);
	
	//-z
	p = v + glm::vec3(s, s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, -s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-s, -s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-s, s, -s);
	glVertex3f(p.x, p.y, p.z);
	
	//z
	p = v + glm::vec3(s, s, s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-s, s, s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(-s, -s, s);
	glVertex3f(p.x, p.y, p.z);
	
	p = v + glm::vec3(s, -s, s);
	glVertex3f(p.x, p.y, p.z);
	
	
	
	glEnd();
	glDisable(GL_BLEND);
	
	glEnable(GL_DEPTH_TEST);
}

void Renderer::cameraUpdate()
{
   // cam.SetPosition(mCameraPosition);
	cam.SetPitchYaw(mCameraPitch, mCameraYaw);
    //camera must be updated from mPitch and mYaw
    originalCameraPos = mCameraPosition;
	
   // std::cout << "i am camera and i will update pitch and yaw: " << mCameraPitch << " " << mCameraYaw << "\n";


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
		glm::vec3 newVel = position - mCameraPosition;
		if(lastVel.y < -0.0f && newVel.y >= 0.f){
			duck = glm::min(-lastVel.y * 400.f, 1.0f);
		}
		speedVec = glm::vec2(newVel.x, newVel.z);
		speed = glm::length(speedVec);
		lastVel = newVel;
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

	billboards.at(id).mVbo.destroyBuffers();
	billboards.erase(id);
}
