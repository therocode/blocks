#include "chunkinterface.hpp"
#include <angelscript.h>
#include <assert.h>
#include "../scriptchunk.hpp"

ChunkInterface::ChunkInterface(fea::MessageBus& bus, GameInterface& worldInterface)
    : ScriptInterface(bus, worldInterface)
{
}

void ChunkInterface::registerInterface(asIScriptEngine* engine) 
{
    int32_t r = engine->RegisterObjectType("Chunk", sizeof(ScriptChunk), asOBJ_REF); assert(r >= 0);

    r = engine->RegisterObjectBehaviour("Chunk", asBEHAVE_FACTORY, "Chunk@ f()", asFUNCTION(constructScriptChunk), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterObjectBehaviour("Chunk", asBEHAVE_ADDREF, "void f()", asMETHOD(ScriptChunk, addRef), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectBehaviour("Chunk", asBEHAVE_RELEASE, "void f()", asMETHOD(ScriptChunk, release), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectMethod("Chunk", "uint16 getVoxelType(const ChunkVoxelCoord &in) const", 
            asMETHODPR(ScriptChunk, getVoxelType, (const glm::u8vec3&), VoxelType), asCALL_THISCALL); assert(r >= 0);
    
    r = engine->RegisterObjectMethod("Chunk", "array<uint16> @getVoxelData()",
            asMETHODPR(ScriptChunk, getVoxelData, (), CScriptArray*), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectMethod("Chunk", "void setVoxelType(ChunkVoxelCoord &in, uint16)", 
        asMETHODPR(ScriptChunk, setVoxelType, (const glm::u8vec3&, VoxelType), void), asCALL_THISCALL); assert(r >= 0); 
    
    r = engine->RegisterObjectMethod("Chunk", "void setVoxelData(array<uint16> &in)", 
        asMETHODPR(ScriptChunk, setVoxelData, (const CScriptArray&), void), asCALL_THISCALL); assert(r >= 0);
}
