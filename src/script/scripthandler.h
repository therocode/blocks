#pragma once
#include "scriptengine.h"
#include "scriptinterface.h"
#include "scriptentity.h"
#include "../worldinterface.h"
#include <map>
#include <unordered_map>

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>,
    public fea::MessageReceiver<EntityNeedsScriptMessage>,
    public fea::MessageReceiver<ScriptEntityFinishedMessage>,
    public fea::MessageReceiver<RemoveScriptEntityMessage>,
    public fea::MessageReceiver<EntityRemovedMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus, WorldInterface& worldInterface);
        ~ScriptHandler();
        void setup();
        void destroy();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
        void handleMessage(const EntityNeedsScriptMessage& message);
        void handleMessage(const ScriptEntityFinishedMessage& message);
        void handleMessage(const RemoveScriptEntityMessage& message);
        void handleMessage(const EntityRemovedMessage& message);
        void requestRemovalOfEntity(asIScriptObject* obj);
    private:
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;
        WorldInterface& mWorldInterface;
        std::vector<std::string> sourceFiles;
        std::map<size_t, ScriptEntity> scriptEntities;
        std::unordered_map<asIScriptObject*, size_t> scriptEntityIds;

        //callers
        ScriptInterface mScriptInterface;
};
