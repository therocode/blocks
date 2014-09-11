#include "mathsinterface.hpp"
#include <angelscript.h>
#include <assert.h>
#include "../asaddons/scriptmath.hpp"
#include "../asaddons/scriptvectors.hpp"

MathsInterface::MathsInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void MathsInterface::registerInterface(asIScriptEngine* engine)
{
    RegisterScriptMath(engine);
    registerGlmVectors(engine);
}
