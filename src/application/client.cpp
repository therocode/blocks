#include "client.hpp"
#include <iostream>
#include <fea/ui/sdl2windowbackend.hpp>
#include <fea/ui/sdl2inputbackend.hpp>
#include "../lognames.hpp"
#include "../application/applicationmessages.hpp"
#include "../rendering/renderingsystem.hpp"
#include "../input/inputadaptor.hpp"
#include "../world/raycaster.hpp"
#include "../resources/resourcesystem.hpp"


Client::Client(fea::MessageBus& bus, const NetworkParameters& parameters) :
    mFrameNumber(0),
    mBus(bus),
    mLogger(mBus, LogLevel::VERB),
	mWindow(new fea::SDL2WindowBackend()),
	mInputAdaptor(std::unique_ptr<InputAdaptor>(new InputAdaptor(mBus))),
	mQuit(false),
    mClientWorld(bus),
    mPitch(0.0f),
    mYaw(0.0f)
{
    subscribe(mBus, *this);

	mWindow.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32, 32, 0, 3, 3));
    mLockedMouse = true;
	mWindow.lockCursor(true);
	mWindow.setVSyncEnabled(false);
	mRenderingSystem = std::unique_ptr<RenderingSystem>(std::unique_ptr<RenderingSystem>(new RenderingSystem(mBus, {800, 600})));
	mWindow.setFramerateLimit(60);

	mBus.send(WindowResizeMessage{800, 600});

    mFPSCounter.setMaxFPS(0);
    mFPSCounter.setSampleTime(0.5f);
    
    mResourceSystem = std::unique_ptr<ResourceSystem>(new ResourceSystem(bus, "assets", {"iqm",
                                                                                         "png",
                                                                                         "vert",
                                                                                         "frag",
                                                                                         "shad",
                                                                                         "meta"}));

    mClientNetworkingSystem = std::unique_ptr<ClientNetworkingSystem>(new ClientNetworkingSystem(bus, parameters));
}

Client::~Client()
{
	mWindow.close();
	mBus.send(LogMessage{"Shutting down client", gClientName, LogLevel::INFO});
}

void Client::updateVoxelLookAt()
{
	glm::vec3 direction = glm::vec3(glm::cos(mPitch) * glm::sin(mYaw), glm::sin(mPitch), glm::cos(mPitch) * glm::cos(mYaw));

	VoxelCoord block;
	uint32_t face = 0;
	bool f = RayCaster::getVoxelAtRay(mClientWorld.getVoxels(mCurrentWorld), mPosition + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    mBus.send(FacingBlockMessage{f, block});
}

void Client::update()
{
    mBus.send(FrameMessage{mFrameNumber++});
	mInputAdaptor->update();
}

void Client::render()
{
    mFPSCounter.frameBegin();
	mRenderingSystem->render();
	mWindow.swapBuffers();
    mFPSCounter.frameEnd();
    if(mFrame++ % 60 == 0)
    mWindow.setTitle("Blocks | FPS:" + std::to_string((int)mFPSCounter.getAverageFPS()));
}

void Client::handleMessage(const ClientActionMessage& received)
{
	if(received.action == InputAction::QUIT)
	{
		mQuit = true;
	}
}

void Client::handleMessage(const CursorLockedMessage& received)
{
    mWindow.lockCursor(received.locked);
}

void Client::handleMessage(const GameStartMessage& received)
{
}

void Client::handleMessage(const LocalPlayerAttachedToEntityMessage& received)
{
    mCurrentWorld = received.worldId;
    mCurrentEntity = received.entityId;
    mPosition = received.position;
    mHighlightRadius = received.highlightRange;
    mBus.send(HighlightEntityAddRequestedMessage{received.worldId, 0, WorldToChunk::convert(received.position), mHighlightRadius});
}

//void Client::handleMessage(const ClientEnteredWorldMessage& received)
//{
//    mCurrentWorld = received.world;
//
//    auto dehighlighted = mHighlightedChunks.removeHighlightEntity(0);
//
//    auto highlighted = mHighlightedChunks.addHighlightEntity(0, mLastChunk, mHighlightRadius);
//
//    if(highlighted.size() > 0)
//        mBus.send(ClientRequestedChunksMessage{mCurrentWorld, highlighted});
//
//    for(const auto& chunk : dehighlighted)
//        mBus.send(ClientChunkDeletedMessage{chunk});
//}
//
//void Client::handleMessage(const ClientPositionMessage& received)
//{
//    const auto& highlighted = mHighlightedChunks.moveHighlightEntity(0, WorldToChunk::convert(received.position));
//    mLastChunk = WorldToChunk::convert(received.position);
//
//    if(highlighted.first.size() > 0)
//        mBus.send(ClientRequestedChunksMessage{mCurrentWorld, highlighted.first});
//
//    for(const auto& chunk : highlighted.second)
//        mBus.send(ClientChunkDeletedMessage{chunk});
//}

void Client::handleMessage(const ClientEntityMovedMessage& received)
{
    if(received.id == mCurrentEntity)
    {
        mPosition = received.position;
        mBus.send(HighlightEntityMoveRequestedMessage{0, 0, WorldToChunk::convert(received.position)});
        updateVoxelLookAt();
    }
}

bool Client::requestedQuit()
{
	return mQuit;
}

void Client::handleMessage(const MoveGfxEntityMessage& received)
{
}

void Client::handleMessage(const RotateGfxEntityMessage& received)
{
    if(received.id == mCurrentEntity)
    {
        mPitch = received.pitch;
        mYaw = received.yaw;
    }
}
