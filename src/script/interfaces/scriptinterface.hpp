#pragma once
#include <fea/util.hpp>
#include "../../utilities/logger.hpp"

class GameInterface;
class asIScriptEngine;

class ScriptInterface
{
    public:
        ScriptInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        virtual void registerInterface(asIScriptEngine* engine) = 0;
        virtual ~ScriptInterface();
    protected:
        fea::MessageBus& mBus;
        GameInterface& mGameInterface;
};
