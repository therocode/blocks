#pragma once
#include "../world/worldsystem.hpp"
#include "../networking/serverclientbridge.hpp"
#include "../world/worldmessages.hpp"
#include "../script/scriptsystem.hpp"
#include "../utilities/fpscontroller.hpp"
#include "../utilities/logger.hpp"
#include "../entity/entitysystem.hpp"
#include "../gameinterface.hpp"
#include "../networking/servernetworkingsystem.hpp"

class ClientConnection;
class ClientConnectionListener;
class NetworkParameters;
using ClientId = size_t;

class Server
{
    public:
        Server(fea::MessageBus& bus, const NetworkParameters& parameters);
        ~Server();
        void doLogic();
    private:
        fea::MessageBus& mBus;
        Logger mLogger;
        Timer mTimer;
        WorldSystem mWorldSystem;
        ScriptSystem mScriptSystem;
        EntitySystem mEntitySystem;
        ServerNetworkingSystem mServerNetworkingSystem;
        GameInterface mGameInterface;
        FPSController mFPSController;
};
