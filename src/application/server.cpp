#include "server.hpp"
#include "../lognames.hpp"
#include "../entity/controllers/playercontroller.hpp"
#include "../entity/controllers/physicscontroller.hpp"
#include "../entity/controllers/collisioncontroller.hpp"
#include "../entity/controllers/gfxcontroller.hpp"
#include "../entity/controllers/movementcontroller.hpp"
#include "../entity/controllers/highlightcontroller.hpp"

Server::Server(fea::MessageBus& bus, const NetworkParameters& parameters) : 
    mBus(bus),
    mWorldSystem(mBus),
	mEntitySystem(mBus),
    mGameInterface(mWorldSystem, mEntitySystem),
    mServerNetworkingSystem(mBus, mGameInterface, parameters),
    mLogger(mBus, LogLevel::VERB),
    mScriptSystem(mBus, mGameInterface),
    mFrameNumber(0)
{
    mTimer.start();

	mEntitySystem.addController(std::unique_ptr<EntityController>(new PhysicsController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new CollisionController(mBus, mGameInterface)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new MovementController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new GfxController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new HighlightController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mGameInterface)));

    mFPSController.setMaxFPS(60);
    mBus.send(GameStartMessage{});
}

Server::~Server()
{
    mBus.send(LogMessage{"Shutting down server", serverName, LogLevel::INFO});
}

void Server::doLogic()
{
    mBus.send(FrameMessage{mFrameNumber, (int32_t) mTimer.getDeltaTime()});

    mFPSController.frameEnd();
    mFrameNumber++;
}
