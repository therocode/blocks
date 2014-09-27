#pragma once
#include <angelscript.h>
#include <fea/util.hpp>
#include "scriptmodule.hpp"

class ScriptEngine
{
    public:
        ScriptEngine(fea::MessageBus& bus);
        ~ScriptEngine();
        void setup();
        ScriptModule createModule(const std::string& name);
        void destroyModule(ScriptModule& module);
        asIScriptContext* requestContext();
        void freeContext(asIScriptContext* context);
        asIScriptEngine* getEngine();
    private:
        void messageCallback(const asSMessageInfo &msg);
        fea::MessageBus& mBus;
        asIScriptEngine* mEngine;

        std::vector<asIScriptContext*> mContexts;
        uint32_t mContextsInUse;
};
