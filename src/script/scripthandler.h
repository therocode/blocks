#pragma once
#include "scriptengine.h"
#include "scriptinterface.h"

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>,
    public fea::MessageReceiver<EntitySpawnedMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus);
        ~ScriptHandler();
        void setup();
        void destroy();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
        void handleMessage(const EntitySpawnedMessage& message);
    private:
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;
        std::vector<std::string> sourceFiles;

        //callers
        ScriptInterface mScriptInterface;
};
