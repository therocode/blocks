#include "landscapeinterface.hpp"
#include "world/worldmessages.hpp"
#include <angelscript.h>
#include <assert.h>

LandscapeInterface::LandscapeInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void LandscapeInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void setVoxelType(uint32 worldId, float x, float y, float z, uint16 type)", asMETHODPR(LandscapeInterface, setVoxelType, (WorldId worldId, float x, float y, float z, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void setVoxelType(uint32 worldId, const Vec3& in, uint16 type)", asMETHODPR(LandscapeInterface, setVoxelType, (WorldId worldId, const glm::vec3&, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("uint16 getVoxelType(uint32 worldId, float x, float y, float z)", asMETHODPR(LandscapeInterface, getVoxelType, (WorldId worldId, float x, float y, float z), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("uint16 getVoxelType(uint32 worldId, const Vec3& in)", asMETHODPR(LandscapeInterface, getVoxelType, (WorldId worldId, const glm::vec3&), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

}

void LandscapeInterface::setVoxelType(WorldId worldId, float x, float y, float z, uint16_t type)
{
    mBus.send<SetVoxelMessage>(SetVoxelMessage{worldId, VoxelCoord(floor(x), floor(y), floor(z)), type});
}

void LandscapeInterface::setVoxelType(WorldId worldId, const glm::vec3& coordinate, uint16_t type)
{
    setVoxelType(worldId, coordinate.x, coordinate.y, coordinate.z, type);
}

VoxelType LandscapeInterface::getVoxelType(WorldId id, float x, float y, float z)
{
    return mWorldInterface.getVoxelType(id, x, y, z);
}

VoxelType LandscapeInterface::getVoxelType(WorldId id, const glm::vec3& coordinate)
{
    return mWorldInterface.getVoxelType(id, coordinate);
}
