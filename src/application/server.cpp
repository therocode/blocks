#include "server.hpp"
#include "../lognames.hpp"
#include "../entity/controllers/playercontroller.hpp"
#include "../entity/controllers/physicscontroller.hpp"
#include "../entity/controllers/collisioncontroller.hpp"
#include "../entity/controllers/gfxcontroller.hpp"
#include "../entity/controllers/movementcontroller.hpp"
#include "../entity/controllers/highlightcontroller.hpp"
#include "../entity/controllers/worldcontroller.hpp"

Server::Server(fea::MessageBus& bus, const NetworkParameters& parameters) : 
    mBus(bus),
    mWorldSystem(mBus),
	mEntitySystem(mBus),
    mGameInterface(mWorldSystem, mEntitySystem),
    mServerNetworkingSystem(mBus, mGameInterface, parameters),
    mLogger(mBus, LogLevel::VERB),
    mScriptSystem(mBus, mGameInterface),
    mFrameNumber(0),
    mQuit(false)
{
    mTimer.start();

	mEntitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(mBus, mGameInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new MovementController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new GfxController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new HighlightController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new WorldController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mGameInterface)));

    mFPSController.setMaxFPS(60);
    mBus.send(GameStartMessage{});
}

Server::~Server()
{
    mBus.send(LogMessage{"Shutting down server", serverName, LogLevel::INFO});
}

#include <iostream>

void Server::doLogic()
{
	std::cout << "Sending FrameMessage" << std::endl;
    mBus.send(FrameMessage{mFrameNumber, (int32_t) mTimer.getDeltaTime()});

    mFPSController.frameEnd();
    mFrameNumber++;

    if(mSignalCatcher.getSignal() == QUIT_SIGNAL)
    {
        mBus.send(LogMessage{"Received quit signal. Requesting to quit!", serverName, LogLevel::INFO});
        mQuit = true;
    }
}

bool Server::requestedQuit() const
{
    return mQuit;
}
