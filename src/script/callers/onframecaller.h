#pragma once
#include <featherkit/messaging.h>
#include "../../messages.h"
#include "../scriptengine.h"

class OnFrameCaller : 
    public fea::MessageReceiver<FrameMessage>
{
    public:
        OnFrameCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptModule& module);
        ~OnFrameCaller();
        void initialise();
        void handleMessage(const FrameMessage& received);
    private:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        ScriptModule& mModule;

        asIScriptFunction* mOnFrameFunc;
};
