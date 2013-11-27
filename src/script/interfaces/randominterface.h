#pragma once
#include "scriptinterface.h"
#include "utilities/random.h"

class RandomInterface : public ScriptInterface
{
    public:
        RandomInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        Random random;
};