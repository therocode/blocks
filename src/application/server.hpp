#pragma once
#include "../world/worldsystem.hpp"
#include "../world/worldmessages.hpp"
#include "../script/scriptsystem.hpp"
#include "../utilities/fpscontroller.hpp"
#include "../utilities/logger.hpp"
#include "../entity/entitysystem.hpp"
#include "../gameinterface.hpp"
#include "../networking/servernetworkingsystem.hpp"

class NetworkParameters;

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

        uint32_t mFrameNumber;
};
