#include "scriptvectors.hpp"
#include <new>

void registerGlmVectors(asIScriptEngine* engine) 
{
    registerGlmVec3(engine, "Vec3");
    registerGlmU8Vec3(engine, "ChunkVoxelCoord");
    registerGlmI64Vec3(engine, "ChunkCoord");
}    


