#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"

class StringInterface : public ScriptInterface
{
    public:
        StringInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
};
