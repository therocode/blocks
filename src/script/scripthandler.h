#pragma once
#include "scriptengine.h"
#include "../world/worldinterface.h"
#include "scriptmessages.h"
#include "../application/applicationmessages.h"
#include "scriptstd.h"
#include "scriptcallback.h" //temp

class ScriptInterface;
class ScriptCaller;

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>,
    public fea::MessageReceiver<EntityCreatedMessage>,
    public fea::MessageReceiver<EntityRemovedMessage>,
    public fea::MessageReceiver<FrameMessage>,
    public fea::MessageReceiver<GameStartMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~ScriptHandler();
        void setup();
        void destroy();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
        void handleMessage(const EntityCreatedMessage& message);
        void handleMessage(const EntityRemovedMessage& message);
        void handleMessage(const FrameMessage& received);
        void handleMessage(const GameStartMessage& received);
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

        ScriptCallback<int32_t> onFrameCallback;
        ScriptCallback<> gameStartCallback;
        int32_t frameTick;
};
