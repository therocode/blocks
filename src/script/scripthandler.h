#pragma once
#include "scriptengine.h"
#include "callers/onframecaller.h"

class ScriptHandler
{
    public:
        ScriptHandler(fea::MessageBus& bus);
        ~ScriptHandler();
        void setup();
        void destroy();
    private:
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;

        //callers
        OnFrameCaller mOnFrameCaller;
};
