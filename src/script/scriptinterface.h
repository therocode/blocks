#pragma once
#include <featherkit/messaging.h>
#include "../messages.h"
#include "scriptengine.h"

class ScriptInterface : 
    public fea::MessageReceiver<FrameMessage>
{
    public:
        ScriptInterface(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module);
        ~ScriptInterface();
        void registerInterface();
        void registerCallbacks();
        void handleMessage(const FrameMessage& received);
    private:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        ScriptModule& mModule;

        asIScriptFunction* mOnFrameFunc;
        
        //interface functions

        void scriptPrint(std::string text);
        void setGravity(float constant);
};
