#pragma once
#include "scriptengine.hpp"
#include "scriptmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "scriptstd.hpp"
#include "scriptcallback.hpp"//temp
#include "../entity/entitymessages.hpp"

class ScriptInterface;
class ScriptCaller;
class GameInterface;

class ScriptSystem :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage,
                                EntityCreatedMessage,
                                EntityRemovedMessage>
{
    public:
        ScriptSystem(fea::MessageBus& bus, GameInterface& worldInterface);
        ~ScriptSystem();
        void setup();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
        void handleMessage(const EntityCreatedMessage& message);
        void handleMessage(const EntityRemovedMessage& message);
    private:
        void registerInterface();
        void registerCallbacks(ScriptEntityMap& scriptEntities);
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;
        GameInterface& mGameInterface;
        std::vector<std::string> sourceFiles;
        ScriptEntityMap scriptEntities;
        std::string logName;

        std::vector<std::unique_ptr<ScriptInterface>> mInterfaces;
        std::vector<std::unique_ptr<ScriptCaller>> mCallers;
};
