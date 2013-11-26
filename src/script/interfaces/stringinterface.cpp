#include "stringinterface.h"
#include <angelscript.h>
#include <assert.h>

StringInterface::StringInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void StringInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("string toString(int num)", asFUNCTIONPR(std::to_string, (int32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(uint num)", asFUNCTIONPR(std::to_string, (uint32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(int16 num)", asFUNCTIONPR(std::to_string, (int32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(uint16 num)", asFUNCTIONPR(std::to_string, (uint32_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(int64 num)", asFUNCTIONPR(std::to_string, (int64_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(uint64 num)", asFUNCTIONPR(std::to_string, (uint64_t), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(float num)", asFUNCTIONPR(std::to_string, (float), std::string), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("string toString(double num)", asFUNCTIONPR(std::to_string, (double), std::string), asCALL_CDECL); assert(r >= 0);
}
