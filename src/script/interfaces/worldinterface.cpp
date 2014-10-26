#include "worldinterface.hpp"
#include "../../gameinterface.hpp"
#include "../../world/worldmessages.hpp"
#include "../../world/worldsystem.hpp"
#include <angelscript.h>
#include <assert.h>

WorldInterface::WorldInterface(fea::MessageBus& bus, GameInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void WorldInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void setVoxelType(uint32 worldId, const Vec3& in, uint16 type)", asMETHODPR(WorldInterface, setVoxelType, (WorldId worldId, const glm::vec3&, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("uint16 getVoxelType(uint32 worldId, const Vec3& in)", asMETHODPR(WorldInterface, getVoxelType, (WorldId worldId, const glm::vec3&), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
}

void WorldInterface::setVoxelType(WorldId worldId, const glm::vec3& coordinate, uint16_t type)
{
    mBus.send(SetVoxelMessage{worldId, WorldToVoxel::convert(coordinate), type});
}

VoxelType WorldInterface::getVoxelType(WorldId id, const glm::vec3& coordinate)
{
    VoxelType type = 0;

    const auto& chunks = mGameInterface.getWorldSystem().getWorldVoxels(id);
    const auto chunk = chunks.find(WorldToChunk::convert(coordinate));

    if(chunk != chunks.end())
        type = chunk->second.getVoxelType(WorldToChunkVoxel::convert(coordinate));
    return type;
}
