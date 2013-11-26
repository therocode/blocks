#include "mathsinterface.h"
#include <angelscript.h>
#include <assert.h>
#include "../asaddons/scriptmath.h"
#include "../asaddons/scriptvectors.h"

MathsInterface::MathsInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void MathsInterface::registerInterface(asIScriptEngine* engine)
{
    RegisterScriptMath(engine);
    registerGlmVectors(engine);
}
