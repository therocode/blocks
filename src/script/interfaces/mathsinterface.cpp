#include "mathsinterface.hpp"
#include <angelscript.h>
#include <assert.h>
#include "../asaddons/scriptmath.hpp"
#include "../asaddons/scriptvectors.hpp"
#include "../asaddons/scriptprimitivealiases.hpp"

MathsInterface::MathsInterface(fea::MessageBus& bus, GameInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void MathsInterface::registerInterface(asIScriptEngine* engine)
{
    RegisterScriptMath(engine);
    registerGlmVectors(engine);
	registerPrimitiveAliases(engine);
}
