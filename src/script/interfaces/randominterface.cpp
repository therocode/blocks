#include "randominterface.hpp"
#include <angelscript.h>
#include <assert.h>

RandomInterface::RandomInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void RandomInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("uint randomInt()", asMETHOD(Random, randomInt), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
    r = engine->RegisterGlobalFunction("int randomIntRange(int start, int end)", asMETHOD(Random, randomIntRange), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
    r = engine->RegisterGlobalFunction("float randomFloatRange(float start, float end)", asMETHOD(Random, randomFloatRange), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
    r = engine->RegisterGlobalFunction("bool randomChance(float chance)", asMETHOD(Random, randomChance), asCALL_THISCALL_ASGLOBAL, &random); assert(r >= 0);
}
