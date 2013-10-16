#pragma once
#include <angelscript.h>

class ScriptEngine
{
    public:
        ScriptEngine();
        ~ScriptEngine();
    private:
        asIScriptEngine* mEngine;
};
