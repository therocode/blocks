#include "scriptcallback.hpp"

void parameterFunction(asIScriptContext* context, std::size_t index, int value) 
{
    int32_t r = context->SetArgDWord(index, value); assert( r >= 0 );
}
 
void parameterFunction(asIScriptContext* context, std::size_t index, float value) 
{
    int32_t r = context->SetArgFloat(index, value); assert( r >= 0 );
}
 
void parameterFunction(asIScriptContext* context, std::size_t index, bool value) 
{
    int32_t r = context->SetArgByte(index, value); assert( r >= 0 );
}

void parameterFunction(asIScriptContext* context, std::size_t index, uint32_t value) 
{
    int32_t r = context->SetArgDWord(index, value); assert( r >= 0 );
}

void parameterFunction(asIScriptContext* context, std::size_t index, uint64_t value) 
{
    int32_t r = context->SetArgQWord(index, value); assert( r >= 0 );
}

void parameterFunction(asIScriptContext* context, std::size_t index, glm::i64vec3& value) 
{
    int32_t r = context->SetArgObject(index, &value); assert( r >= 0 );
}

void parameterFunction(asIScriptContext* context, std::size_t index, ScriptChunk& value)
{
    int32_t r = context->SetArgObject(index, &value); assert( r >= 0 );
}

void parameterFunction(asIScriptContext* context, std::size_t index, ScriptEntity& value)
{
    int32_t r = context->SetArgObject(index, value.getScriptObject()); assert( r >= 0 );
}
