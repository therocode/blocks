#include "server.hpp"
#include "../lognames.hpp"
#include "../entity/controllers/playercontroller.hpp"
#include "../entity/controllers/physicscontroller.hpp"
#include "../entity/controllers/collisioncontroller.hpp"
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
	mEntitySystem.addController(std::unique_ptr<EntityController>(new HighlightController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new WorldController(mBus)));
	mEntitySystem.addController(std::unique_ptr<EntityController>(new PlayerController(mBus, mGameInterface)));

    mFPSController.setMaxFPS(60);

    mResourceSystem = std::unique_ptr<ResourceSystem>(new ResourceSystem(bus, "assets", {"meta", "wld", "att", "ent"})); 

    mBus.send(GameStartMessage{});
}

Server::~Server()
{
    mBus.send(LogMessage{"Shutting down server", gServerName, LogLevel::INFO});
}

void Server::doLogic()
{
    mBus.send(FrameMessage{mFrameNumber, (int32_t) mTimer.getDeltaTime()});

    mFPSController.frameEnd();
    mFrameNumber++;

    if(mSignalCatcher.getSignal() == QUIT_SIGNAL)
    {
        mBus.send(LogMessage{"Received quit signal. Requesting to quit!", gServerName, LogLevel::INFO});
        mQuit = true;
    }
}

bool Server::requestedQuit() const
{
    return mQuit;
}
