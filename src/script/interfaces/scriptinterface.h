#pragma once
#include <fea/messaging.hpp>
#include "world/worldinterface.h"
#include "utilities/logger.h"

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
