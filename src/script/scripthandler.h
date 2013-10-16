#pragma once
#include "scriptengine.h"
#include "callers/onframecaller.h"

class ScriptHandler :
    public fea::MessageReceiver<RebuildScriptsRequestedMessage>
{
    public:
        ScriptHandler(fea::MessageBus& bus);
        ~ScriptHandler();
        void setup();
        void destroy();
        void handleMessage(const RebuildScriptsRequestedMessage& message);
    private:
        fea::MessageBus& mBus;
        ScriptEngine mEngine;
        ScriptModule mScripts;
        std::vector<std::string> sourceFiles;

        //callers
        OnFrameCaller mOnFrameCaller;
};
