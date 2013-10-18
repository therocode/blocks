#pragma once
#include "scriptengine.h"
#include "scriptinterface.h"
#include "scriptentity.h"
#include "../worldinterface.h"
#include <map>

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>,
    public fea::MessageReceiver<EntityNeedsScriptMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~ScriptHandler();
        void setup();
        void destroy();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
        void handleMessage(const EntityNeedsScriptMessage& message);
    private:
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;
        WorldInterface& mWorldInterface;
        std::vector<std::string> sourceFiles;
        std::map<size_t, ScriptEntity> scriptEntities;

        //callers
        ScriptInterface mScriptInterface;
};
