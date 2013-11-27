#pragma once
#include <featherkit/messaging.h>
#include "../scriptengine.h"
#include "../scriptstd.h"

class ScriptCaller
{
    public:
        ScriptCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities);
        virtual ~ScriptCaller();
        void setActive(bool active);
    protected:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        bool mActive;
        ScriptEntityMap& mScriptEntities;
};
