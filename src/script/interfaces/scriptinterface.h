#pragma once

class asIScriptEngine;

class ScriptInterface
{
    public:
        virtual void registerInterface(asIScriptEngine* engine) = 0;
};
