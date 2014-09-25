#pragma once
#include "scriptengine.hpp"
#include "scriptmessages.hpp"
#include "../application/applicationmessages.hpp"
#include "scriptstd.hpp"
#include "scriptcallback.hpp"//temp
#include "../entity/entitymessages.hpp"

class ScriptInterface;
class ScriptCaller;
class WorldInterface;

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage,
                                EntityCreatedMessage,
                                EntityRemovedMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~ScriptHandler();
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
        WorldInterface& mWorldInterface;
        std::vector<std::string> sourceFiles;
        ScriptEntityMap scriptEntities;
        std::string logName;

        std::vector<std::unique_ptr<ScriptInterface>> mInterfaces;
        std::vector<std::unique_ptr<ScriptCaller>> mCallers;
};
