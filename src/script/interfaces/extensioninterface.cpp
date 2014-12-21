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

    r = engine->RegisterInterfaceMethod("IExtension", "void chunkInitiallyGenerated(const uint32, const ChunkCoord &in, Chunk &in)"); assert(r >= 0);

    r = engine->RegisterInterfaceMethod("IExtension", "void chunkCandidate(const uint32, const ChunkCoord &in, Chunk &in, const uint64)"); assert(r >= 0);

    r = engine->RegisterInterfaceMethod("IExtension", "void chunkFinished(const uint32, const ChunkCoord &in, const Chunk &in)"); assert(r >= 0);
}
