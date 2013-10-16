#pragma once
#include "scriptengine.h"

class ScriptHandler
{
    public:
        ScriptHandler();
        ~ScriptHandler();
        void setup();
        void destroy();
    private:
        ScriptEngine mEngine;
        ScriptModule mScripts;
};
