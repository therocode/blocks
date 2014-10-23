#pragma once
#include "../world/worldsystem.hpp"
#include "../world/worldmessages.hpp"
#include "../script/scriptsystem.hpp"
#include "../utilities/fpscontroller.hpp"
#include "../utilities/logger.hpp"
#include "../utilities/signalcatcher.hpp"
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
        bool requestedQuit() const;
    private:
        fea::MessageBus& mBus;
        Logger mLogger;
        Timer mTimer;
        WorldSystem mWorldSystem;
        ScriptSystem mScriptSystem;
        EntitySystem mEntitySystem;
        GameInterface mGameInterface;
        ServerNetworkingSystem mServerNetworkingSystem;
        FPSController mFPSController;
        SignalCatcher mSignalCatcher;

        uint32_t mFrameNumber;
        bool mQuit;
};
