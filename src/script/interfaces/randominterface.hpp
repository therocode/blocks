#pragma once
#include "scriptinterface.hpp"
#include "../../utilities/random.hpp"

class RandomInterface : public ScriptInterface
{
    public:
        RandomInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        Random random;
};
