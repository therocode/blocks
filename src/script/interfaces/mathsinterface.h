#pragma once
#include "scriptinterface.h"
#include "utilities/random.h"

class MathsInterface : public ScriptInterface
{
    public:
        MathsInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        Random random;
};
