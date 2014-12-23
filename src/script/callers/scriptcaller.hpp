#pragma once
#include <fea/util.hpp>
#include "../scriptengine.hpp"
#include "../scriptstd.hpp"
#include <angelscript.h>
#include <vector>

class ScriptCaller
{
    public:
        ScriptCaller(fea::MessageBus& bus, ScriptEngine& engine, ScriptEntityMap& scriptEntities, std::vector<asIScriptObject*>& extensions);
        virtual ~ScriptCaller();
        void setActive(bool active);
    protected:
        fea::MessageBus& mBus;
        ScriptEngine& mEngine;
        bool mActive;
        ScriptEntityMap& mScriptEntities;
        std::vector<asIScriptObject*>& mExtensions;
};
