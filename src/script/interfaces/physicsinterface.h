#pragma once
#include "scriptinterface.h"
#include "utilities/random.h"

class PhysicsInterface : public ScriptInterface
{
    public:
        PhysicsInterface(fea::MessageBus& bus, WorldInterface& worldInterface);
        void registerInterface(asIScriptEngine* engine) override;
    private:
        void setGravity(float constant);
        void applyImpulse(size_t id, const glm::vec3& force);
        Random random;
};
