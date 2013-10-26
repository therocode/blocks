#pragma once
#include <angelscript.h>
#include <featherkit/messaging.h>
#include "scriptmodule.h"

class ScriptEngine
{
    public:
        ScriptEngine(fea::MessageBus& bus);
        ~ScriptEngine();
        void setup();
        void destroy();
        ScriptModule createModule(const std::string& name);
        void destroyModule(ScriptModule& module);
        asIScriptContext* requestContext();
        void freeContext(asIScriptContext* context);
        asIScriptEngine* getEngine();
    private:
        void messageCallback(const asSMessageInfo &msg);
        fea::MessageBus& mBus;
        asIScriptEngine* mEngine;
        asIScriptContext* mContext;
        std::vector<asIScriptContext*> mContexts;
        uint32_t mContextsInUse;
};
