#include "scriptvectors.h"
#include <new>

void registerGlmVectors(asIScriptEngine* engine)
{    
    int32_t r = 0;

    r = engine->RegisterObjectType("Vec3", sizeof(glm::vec3), asOBJ_VALUE|asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(constructVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(const Vec3 &in)", asFUNCTION(copyConstructVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(destructVec3),  asCALL_CDECL_OBJLAST); assert( r >= 0 );
    //r = engine->RegisterObjectMethod("Vec3", "float x()", asMETHOD(glm::vec3, x), asCALL_THISCALL); assert(r >= 0);
}

using namespace glm;

void constructVec3(void* pointer)
{
    new(pointer) glm::vec3();
}

void copyConstructVec3(const glm::vec3& other, void* pointer)
{
    new(pointer) glm::vec3(other);
}

using namespace glm;

void destructVec3(void* pointer)
{
    ((glm::vec3*) pointer)->~vec3();
}
