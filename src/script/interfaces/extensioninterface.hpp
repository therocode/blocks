#pragma once
#include "scriptinterface.hpp"

class ExtensionInterface :
    public ScriptInterface
{
    public:
        ExtensionInterface(fea::MessageBus& bus, GameInterface& worldInterface);

        void registerInterface(asIScriptEngine* engine) override;
};
