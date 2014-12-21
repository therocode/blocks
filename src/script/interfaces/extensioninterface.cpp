#include "extensioninterface.hpp"
#include <angelscript.h>
#include <assert.h>

ExtensionInterface::ExtensionInterface(fea::MessageBus& bus, GameInterface& worldInterface)
    : ScriptInterface(bus, worldInterface)
{
}

void ExtensionInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterInterface("IExtension"); assert(r >= 0);
}
