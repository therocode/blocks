#include "scriptcallback.h"

void parameterFunction(asIScriptContext* context, std::size_t index, int value) 
{
    int32_t r = context->SetArgDWord(index, value); assert( r >= 0 );
}
 
void parameterFunction(asIScriptContext* context, std::size_t index, float value) 
{
    int32_t r = context->SetArgFloat(index, value); assert( r >= 0 );
}
