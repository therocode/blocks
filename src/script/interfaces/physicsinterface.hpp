#pragma once
#include "scriptinterface.hpp"
#include "utilities/random.hpp"

class PhysicsInterface : public ScriptInterface
{
    public:
        PhysicsInterface(fea::MessageBus& bus, GameInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setGravity(float constant);
        void applyImpulse(size_t id, const glm::vec3& force);
        Random random;
};
