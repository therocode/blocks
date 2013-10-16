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
    private:
        void messageCallback(const asSMessageInfo &msg);
        void scriptPrint(std::string text);
        asIScriptEngine* mEngine;
        asIScriptContext* mContext;
};
