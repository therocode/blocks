#include "scriptprimitivealiases.hpp"

void registerPrimitiveAliases(asIScriptEngine* engine) 
{
    uint32_t r = engine->RegisterObjectType("VoxelType", sizeof(uint16_t), asOBJ_VALUE | asOBJ_POD); assert(r >= 0);
    r = engine->RegisterObjectType("WorldId", sizeof(uint32_t), asOBJ_VALUE | asOBJ_POD); assert(r >= 0);
}
