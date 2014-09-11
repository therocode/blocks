#pragma once
#include <fea/util.hpp>
#include "world/worldinterface.hpp"
#include "utilities/logger.hpp"

class asIScriptEngine;

class ScriptInterface
{
    public:
        ScriptInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        virtual void registerInterface(asIScriptEngine* engine) = 0;
        virtual ~ScriptInterface();
    protected:
        fea::MessageBus& mBus;
        WorldInterface& mWorldInterface;
        std::string mLogName; 
};
