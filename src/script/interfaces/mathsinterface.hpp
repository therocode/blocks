#pragma once
#include "scriptinterface.hpp"
#include "../../utilities/random.hpp"

class MathsInterface : public ScriptInterface
{
    public:
        MathsInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        Random random;
};
