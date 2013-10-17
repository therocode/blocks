#include "scriptcallback.h"

void parameterFunction(asIScriptContext* context, std::size_t index, int value) 
{
    context->SetArgDWord(index, value); assert( r >= 0 );
}
 
void parameterFunction(asIScriptContext* context, std::size_t index, float value) 
{
    context->SetArgFloat(index, value); assert( r >= 0 );
}
