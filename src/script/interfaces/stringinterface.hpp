#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"

class GameInterface;

class StringInterface : public ScriptInterface
{
    public:
        StringInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
};
