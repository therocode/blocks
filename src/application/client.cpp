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
    mDebugUi(bus),
	mQuit(false),
    mClientWorld(bus)
{
    subscribe(mBus, *this);

	mWindow.create(fea::VideoMode(800, 600, 32), "Blocky", fea::Style::Default, fea::ContextSettings(32, 32, 0, 3, 3));
    mLockedMouse = true;
	mWindow.lockCursor(true);
	mWindow.setVSyncEnabled(true);
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
                                                                                         "meta",
                                                                                         "gfe",
                                                                                         "ttf"}));

    mClientNetworkingSystem = std::unique_ptr<ClientNetworkingSystem>(new ClientNetworkingSystem(bus, parameters, *mResourceSystem));
}

Client::~Client()
{
	mWindow.close();
	mBus.send(LogMessage{"Shutting down client", gClientName, LogLevel::INFO});
}

void Client::updateVoxelLookAt()
{
	glm::vec3 direction = mOrientation * glm::vec3(0.0f, 0.0f, -1.0f);

	VoxelCoord block;
	uint32_t face = 0;
	bool f = RayCaster::getVoxelAtRay(mClientWorld.getVoxels(mCurrentWorld), mPosition + glm::vec3(0, 0.6f, 0), direction, 200.f, face, block);

    mBus.send(FacingBlockMessage{f, block});
}

void Client::update()
{
    mBus.send(FrameMessage{mFrameNumber++});
	mInputAdaptor->update();
    mBus.send(UiRenderDataMessage{mDebugUi.root()});
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

void Client::handleMessage(const OrientateLocalEntityMessage& received)
{
    mOrientation = received.orientation;
}
