#pragma once
#include <featherkit/messaging.h>
#include "../messages.h"
#include "scriptcallback.h"
#include "../utilities/random.h"
#include "../world/worldinterface.h"

class ScriptInterface : 
    public fea::MessageReceiver<FrameMessage>
{
    public:
        ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module, WorldInterface& worldInterface);
        ~ScriptInterface();
        void registerInterface();
        void registerCallbacks();
        void handleMessage(const FrameMessage& received);
        asIScriptObject* createEntity(const std::string& type, float x, float y, float z);
        asIScriptObject* instanciateScriptEntity(const std::string& type);
        void removeEntity(asIScriptObject* entity);
    private:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        ScriptModule& mModule;
        WorldInterface& mWorldInterface;
        Random random;
        std::string logName;

        ScriptCallback<int32_t> onFrameCallback;
        int32_t frameTick;
        
        //interface functions
        void scriptPrint(std::string text);
        void setGravity(float constant);
};
