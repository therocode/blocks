#pragma once
#include <fea/entitysystem.hpp>
#include "scriptinterface.hpp"
#include "../../utilities/random.hpp"

class PhysicsInterface : public ScriptInterface
{
    public:
        PhysicsInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setGravity(float constant);
        void applyImpulse(fea::EntityId id, const glm::vec3& force);
        Random random;
};
