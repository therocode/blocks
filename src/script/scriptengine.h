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
    private:
        void messageCallback(const asSMessageInfo &msg);
        asIScriptEngine* mEngine;
};
