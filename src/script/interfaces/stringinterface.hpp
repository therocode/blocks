#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"

class WorldInterface;

class StringInterface : public ScriptInterface
{
    public:
        StringInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
};
