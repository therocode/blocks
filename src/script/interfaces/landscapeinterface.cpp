#include "landscapeinterface.h"
#include "world/worldmessages.h"
#include <angelscript.h>
#include <assert.h>

LandscapeInterface::LandscapeInterface(fea::MessageBus& bus, WorldInterface& worldInterface) : ScriptInterface(bus, worldInterface)
{
}

void LandscapeInterface::registerInterface(asIScriptEngine* engine)
{
    int32_t r = engine->RegisterGlobalFunction("void setVoxelType(float x, float y, float z, uint16 type)", asMETHODPR(LandscapeInterface, setVoxelType, (float x, float y, float z, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("void setVoxelType(const Vec3& in, uint16 type)", asMETHODPR(LandscapeInterface, setVoxelType, (const glm::vec3&, uint16_t type), void), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("uint16 getVoxelType(float x, float y, float z)", asMETHODPR(LandscapeInterface, getVoxelType, (float x, float y, float z), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);
    r = engine->RegisterGlobalFunction("uint16 getVoxelType(const Vec3& in)", asMETHODPR(LandscapeInterface, getVoxelType, (const glm::vec3&), VoxelType), asCALL_THISCALL_ASGLOBAL, this); assert(r >= 0);

}

void LandscapeInterface::setVoxelType(float x, float y, float z, uint16_t type)
{
    mBus.send<SetVoxelMessage>(SetVoxelMessage{VoxelCoord(floor(x), floor(y), floor(z)), type});
}

void LandscapeInterface::setVoxelType(const glm::vec3& coordinate, uint16_t type)
{
    setVoxelType(coordinate.x, coordinate.y, coordinate.z, type);
}

VoxelType LandscapeInterface::getVoxelType(float x, float y, float z)
{
    return mWorldInterface.getVoxelType(x, y, z);
}

VoxelType LandscapeInterface::getVoxelType(const glm::vec3& coordinate)
{
    return mWorldInterface.getVoxelType(coordinate);
}
