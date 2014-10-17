#include "physicsinterface.hpp"
#include "../../entity/entitymessages.hpp"
#include <angelscript.h>
#include <assert.h>

PhysicsInterface::PhysicsInterface(fea::MessageBus& bus, GameInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void PhysicsInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void setGravity(float constant)", asMETHOD(PhysicsInterface, setGravity), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void applyImpulseOnEntity(uint id, const Vec3& in)", asMETHOD(PhysicsInterface, applyImpulse), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
}

void PhysicsInterface::setGravity(float constant)
{
    mBus.send(GravityRequestedMessage{constant});
}

void PhysicsInterface::applyImpulse(fea::EntityId id, const glm::vec3& force)
{
    mBus.send(PhysicsImpulseMessage{id, force});
}
