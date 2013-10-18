#pragma once
#include "scriptengine.h"
#include "scriptinterface.h"
#include "scriptentity.h"
#include <map>

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
        std::map<size_t, ScriptEntity> scriptEntities;

        //callers
        ScriptInterface mScriptInterface;
};
