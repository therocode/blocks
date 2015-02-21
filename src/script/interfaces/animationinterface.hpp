#pragma once
#include <fea/entitysystem.hpp>
#include "scriptinterface.hpp"

class AnimationInterface : public ScriptInterface
{
    public:
        AnimationInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setAnimation(fea::EntityId id, const std::string& animationName);
};
