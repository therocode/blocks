#pragma once
#include <fea/util.hpp>
#include "../scriptengine.hpp"
#include "../scriptstd.hpp"

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
