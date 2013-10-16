#pragma once
#include <angelscript.h>
#include "scriptmodule.h"

class ScriptEngine
{
    public:
        ScriptEngine();
        ~ScriptEngine();
        ScriptModule createModule(const std::string& name);
        void destroyModule(ScriptModule& module);
        asIScriptContext* getContext();
        asIScriptEngine* getEngine();
    private:
        void messageCallback(const asSMessageInfo &msg);
        asIScriptEngine* mEngine;
        asIScriptContext* mContext;
};
