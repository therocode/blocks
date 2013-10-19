#pragma once
#include <featherkit/messaging.h>
#include "../messages.h"
#include "scriptcallback.h"
#include "../random.h"
#include "../worldinterface.h"

class ScriptInterface : 
    public fea::MessageReceiver<FrameMessage>
{
    public:
        ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module, WorldInterface& worldInterface);
        ~ScriptInterface();
        void registerInterface();
        void registerCallbacks();
        void handleMessage(const FrameMessage& received);
        asIScriptObject* createEntity(const std::string& type);
        asIScriptObject* instanciateScriptEntity(const std::string& type);
    private:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        ScriptModule& mModule;
        WorldInterface& mWorldInterface;
        Random random;

        ScriptCallback<int32_t> onFrameCallback;
        int32_t frameTick;
        
        //interface functions
        void scriptPrint(std::string text);
        void setGravity(float constant);
};
