#include "server.hpp"
#include "../lognames.hpp"
#include "../entity/controllers/playercontroller.hpp"
#include "../entity/controllers/physicscontroller.hpp"
#include "../entity/controllers/collisioncontroller.hpp"
#include "../entity/controllers/gfxcontroller.hpp"
#include "../entity/controllers/movementcontroller.hpp"

Server::Server(fea::MessageBus& bus, const NetworkParameters& parameters) : 
    mBus(bus),
    mWorldSystem(mBus),
	mEntitySystem(mBus),
    mServerNetworkingSystem(mBus, parameters),
    mGameInterface(mWorldSystem, mEntitySystem),
    mLogger(mBus, LogLevel::VERB),
    mScriptSystem(mBus, mGameInterface)
{
    mTimer.start();
    mBus.send(LogMessage{"Setting up server", serverName, LogLevel::INFO});

	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mGameInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(mBus, mGameInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(mBus, mGameInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new MovementController(mBus, mGameInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new GfxController(mBus, mGameInterface)));

    mFPSController.setMaxFPS(60);
    mBus.send(GameStartMessage{});
}

Server::~Server()
{
    mBus.send(LogMessage{"Shutting down server", serverName, LogLevel::INFO});
}

void Server::doLogic()
{
    mBus.send(FrameMessage{true});

	mEntitySystem.update(mTimer.getDeltaTime());

    mFPSController.frameEnd();
}
