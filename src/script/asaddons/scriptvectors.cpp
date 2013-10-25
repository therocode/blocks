#include "scriptvectors.h"
#include <new>

void registerGlmVectors(asIScriptEngine* engine)
{    
    int32_t r = 0;

    r = engine->RegisterObjectType("Vec3", sizeof(glm::vec3), asOBJ_VALUE|asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(constructVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(float x, float y, float z)", asFUNCTION(floatConstructVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_CONSTRUCT, "void f(const Vec3 &in)", asFUNCTION(copyConstructVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour("Vec3", asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(destructVec3),  asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectProperty("Vec3", "float x", asOFFSET(glm::vec3, x)); assert(r >= 0);
    r = engine->RegisterObjectProperty("Vec3", "float y", asOFFSET(glm::vec3, y)); assert(r >= 0);
    r = engine->RegisterObjectProperty("Vec3", "float z", asOFFSET(glm::vec3, z)); assert(r >= 0);

    //r = engine->RegisterObjectMethod("Vec3", "Vec3 opSub(Vec3 vec) const", asMETHODPR(glm::vec3, operator-, (const glm::vec3&) const, glm::vec3), asCALL_THISCALL); assert(r >= 0);

    r = engine->RegisterObjectMethod("Vec3", "Vec3 opAdd(const Vec3& in) const", asFUNCTIONPR(vec3Add, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Vec3", "Vec3 opSub(const Vec3& in) const", asFUNCTIONPR(vec3Sub, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Vec3", "Vec3 opMul(const Vec3& in) const", asFUNCTIONPR(vec3Mul, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Vec3", "Vec3 opDiv(const Vec3& in) const", asFUNCTIONPR(vec3Div, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Vec3", "Vec3 opMul(float val) const", asFUNCTIONPR(vec3Mul, (const glm::vec3&, float), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterObjectMethod("Vec3", "Vec3 opDiv(float val) const", asFUNCTIONPR(vec3Div, (const glm::vec3&, float), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    r = engine->RegisterGlobalFunction("float distance(const Vec3& in, const Vec3& in)", asFUNCTION(vec3Distance), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("float length(const Vec3& in)", asFUNCTION(vec3Length), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("float dot(const Vec3& in, const Vec3& in)", asFUNCTION(vec3Dot), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("Vec3 cross(const Vec3& in, const Vec3& in)", asFUNCTION(vec3Cross), asCALL_CDECL); assert(r >= 0);
    r = engine->RegisterGlobalFunction("Vec3 normalise(const Vec3& in)", asFUNCTION(vec3Normalise), asCALL_CDECL); assert(r >= 0);

    r = engine->RegisterGlobalFunction("string toString(const Vec3& in)", asFUNCTIONPR(vec3ToString, (const glm::vec3&), std::string), asCALL_CDECL); assert(r >= 0);
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

void floatConstructVec3(float x, float y, float z, void* pointer)
{
    new(pointer) glm::vec3(x, y, z);
}

void destructVec3(void* pointer)
{
    ((glm::vec3*) pointer)->~vec3();
}

glm::vec3 vec3Add(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 + vec2;
}

glm::vec3 vec3Sub(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 - vec2;
}

glm::vec3 vec3Mul(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 * vec2;
}

glm::vec3 vec3Div(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 / vec2;
}

glm::vec3 vec3Mul(const glm::vec3& vec1, float val)
{
    return vec1 * val;
}

glm::vec3 vec3Div(const glm::vec3& vec1, float val)
{
    return vec1 / val;
}

float vec3Distance(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return glm::distance(vec1, vec2);
}

float vec3Length(const glm::vec3& vec)
{
    return glm::length(vec);
}

float vec3Dot(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return glm::dot(vec1, vec2);
}

glm::vec3 vec3Cross(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return glm::cross(vec1, vec2);
}

glm::vec3 vec3Normalise(const glm::vec3& vec)
{
	if(glm::length2(vec) != 0)
		return glm::normalize(vec);
	else 
		return glm::vec3(0,0,0);
}

std::string vec3ToString(const glm::vec3& vec)
{
    return std::to_string(vec.x) + ", " + std::to_string(vec.y) + ", " + std::to_string(vec.z);
}
