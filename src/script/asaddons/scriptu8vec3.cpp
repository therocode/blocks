#include "scriptu8vec3.hpp"
#include <new>

void registerGlmU8Vec3(asIScriptEngine* engine, std::string alias)
{    
    int32_t r = 0;

	std::stringstream ss;

    r = engine->RegisterObjectType(alias.c_str(), sizeof(glm::u8vec3), asOBJ_VALUE|asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(constructGlmU8Vec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, "void f(uint8 x, uint8 y, uint8 z)", asFUNCTION(uint8ConstructGlmU8Vec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	ss << "void f(const " << alias << " &in)";
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, ss.str().c_str(), asFUNCTION(copyConstructGlmU8Vec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	ss.str("");
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(destructGlmU8Vec3),  asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectProperty(alias.c_str(), "uint8 x", asOFFSET(glm::u8vec3, x)); assert(r >= 0);
    r = engine->RegisterObjectProperty(alias.c_str(), "uint8 y", asOFFSET(glm::u8vec3, y)); assert(r >= 0);
    r = engine->RegisterObjectProperty(alias.c_str(), "uint8 z", asOFFSET(glm::u8vec3, z)); assert(r >= 0);

    //r = engine->RegisterObjectMethod("Vec3", "Vec3 opSub(Vec3 vec) const", asMETHODPR(glm::u8vec3, operator-, (const glm::u8vec3&) const, glm::u8vec3), asCALL_THISCALL); assert(r >= 0);

	ss << alias << " opAdd(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmU8Vec3Add, (const glm::u8vec3&, const glm::u8vec3&), glm::u8vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opSub(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmU8Vec3Sub, (const glm::u8vec3&, const glm::u8vec3&), glm::u8vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opMul(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmU8Vec3Mul, (const glm::u8vec3&, const glm::u8vec3&), glm::u8vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opDiv(const " << alias << "& in) const"; 
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmU8Vec3Div, (const glm::u8vec3&, const glm::u8vec3&), glm::u8vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opMul(uint8 val) const"; 
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmU8Vec3Mul, (const glm::u8vec3&, uint8_t), glm::u8vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opDiv(uint8 val) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmU8Vec3Div, (const glm::u8vec3&, uint8_t), glm::u8vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " &opAssign(const " << alias << " &in)";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asMETHODPR(glm::u8vec3, operator=, (const glm::u8vec3&), glm::u8vec3&), asCALL_THISCALL); assert( r >= 0 );
	ss.str("");

	ss << "string toString(const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTIONPR(glmU8Vec3ToString, (const glm::u8vec3&), std::string), asCALL_CDECL); assert(r >= 0);
	ss.str("");
}

using namespace glm;

void constructGlmU8Vec3(void* pointer)
{
    new(pointer) glm::u8vec3();
}

void copyConstructGlmU8Vec3(const glm::u8vec3& other, void* pointer)
{
    new(pointer) glm::u8vec3(other);
}

void uint8ConstructGlmU8Vec3(uint8_t x, uint8_t y, uint8_t z, void* pointer)
{
    new(pointer) glm::u8vec3(x, y, z);
}

void destructGlmU8Vec3(void* pointer)
{
    ((glm::u8vec3*) pointer)->~u8vec3();
}

glm::u8vec3 glmU8Vec3Add(const glm::u8vec3& vec1, const glm::u8vec3& vec2)
{
    return vec1 + vec2;
}

glm::u8vec3 glmU8Vec3Sub(const glm::u8vec3& vec1, const glm::u8vec3& vec2)
{
    return vec1 - vec2;
}

glm::u8vec3 glmU8Vec3Mul(const glm::u8vec3& vec1, const glm::u8vec3& vec2)
{
    return vec1 * vec2;
}

glm::u8vec3 glmU8Vec3Div(const glm::u8vec3& vec1, const glm::u8vec3& vec2)
{
    return vec1 / vec2;
}

glm::u8vec3 glmU8Vec3Mul(const glm::u8vec3& vec1, uint8_t val)
{
    return vec1 * val;
}

glm::u8vec3 glmU8Vec3Div(const glm::u8vec3& vec1, uint8_t val)
{
    return vec1 / val;
}

std::string glmU8Vec3ToString(const glm::u8vec3& vec)
{
    return glm::to_string((glm::uvec3)vec);
}
