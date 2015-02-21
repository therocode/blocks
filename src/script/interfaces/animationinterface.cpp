#include "animationinterface.hpp"
#include "../../networking/networkingprotocol.hpp"
#include <angelscript.h>
#include <assert.h>

AnimationInterface::AnimationInterface(fea::MessageBus& bus, GameInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void AnimationInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void setAnimationOnEntity(uint id, const string& in)", asMETHOD(AnimationInterface, setAnimation), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
}

void AnimationInterface::setAnimation(fea::EntityId id, const std::string& animationName)
{
    mBus.send(EntityAnimationMessage{id, animationName});
}
