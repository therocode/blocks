#pragma once
#include "scriptinterface.h"
#include "utilities/random.h"

class StringInterface : public ScriptInterface
{
    public:
        StringInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
};
