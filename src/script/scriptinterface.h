#pragma once
#include <featherkit/messaging.h>
#include "../application/applicationmessages.h"
#include "scriptcallback.h"
#include "../utilities/random.h"
#include "../world/worldinterface.h"
#include "scriptentity.h"

class ScriptInterface : 
    public fea::MessageReceiver<FrameMessage>
{
    public:
        ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module, WorldInterface& worldInterface, std::unordered_map<asIScriptObject*, size_t>& uglyReference);
        ~ScriptInterface();
        void registerInterface();
        void registerCallbacks(const std::map<size_t, ScriptEntity>& scriptEntities);
        void handleMessage(const FrameMessage& received);
        asIScriptObject* createEntity(const std::string& type, float x, float y, float z);
        asIScriptObject* instanciateScriptEntity(const std::string& type, size_t id);
        void removeEntity(asIScriptObject* entity);
    private:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        ScriptModule& mModule;
        WorldInterface& mWorldInterface;
        Random random;
        std::string logName;
        std::unordered_map<asIScriptObject*, size_t>& mUglyReference;

        ScriptCallback<int32_t> onFrameCallback;
        int32_t frameTick;
        
        //interface functions
        void scriptPrint(std::string text);
        void setGravity(float constant);
};
