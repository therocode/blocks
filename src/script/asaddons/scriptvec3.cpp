#include "scriptvec3.hpp"
#include <new>

void registerGlmVec3(asIScriptEngine* engine, std::string alias)
{    
    int32_t r = 0;

	std::stringstream ss;

    r = engine->RegisterObjectType(alias.c_str(), sizeof(glm::vec3), asOBJ_VALUE|asOBJ_APP_CLASS_CDAK); assert(r >= 0);
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(constructGlmVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, "void f(float x, float y, float z)", asFUNCTION(floatConstructGlmVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );

	ss << "void f(const " << alias << " &in)";
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, ss.str().c_str(), asFUNCTION(copyConstructGlmVec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
	ss.str("");
	r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(destructGlmVec3),  asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectProperty(alias.c_str(), "float x", asOFFSET(glm::vec3, x)); assert(r >= 0);
    r = engine->RegisterObjectProperty(alias.c_str(), "float y", asOFFSET(glm::vec3, y)); assert(r >= 0);
    r = engine->RegisterObjectProperty(alias.c_str(), "float z", asOFFSET(glm::vec3, z)); assert(r >= 0);

    //r = engine->RegisterObjectMethod("Vec3", "Vec3 opSub(Vec3 vec) const", asMETHODPR(glm::vec3, operator-, (const glm::vec3&) const, glm::vec3), asCALL_THISCALL); assert(r >= 0);

	ss << alias << " opAdd(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmVec3Add, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opSub(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmVec3Sub, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opMul(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmVec3Mul, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opDiv(const " << alias << "& in) const"; 
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmVec3Div, (const glm::vec3&, const glm::vec3&), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opMul(float val) const"; 
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmVec3Mul, (const glm::vec3&, float), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " opDiv(float val) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmVec3Div, (const glm::vec3&, float), glm::vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
	ss.str("");

	ss << alias << " &opAssign(const " << alias << " &in)";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asMETHODPR(glm::vec3, operator=, (const glm::vec3&), glm::vec3&), asCALL_THISCALL); assert( r >= 0 );
	ss.str("");

	ss << "float distance(const " << alias << "& in, const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTION(glmVec3Distance), asCALL_CDECL); assert(r >= 0);
	ss.str("");

	ss << "float length(const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTION(glmVec3Length), asCALL_CDECL); assert(r >= 0);
	ss.str("");

	ss << "float dot(const " << alias << "& in, const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTION(glmVec3Dot), asCALL_CDECL); assert(r >= 0);
	ss.str("");

	ss << alias << " cross(const " << alias << "& in, const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTION(glmVec3Cross), asCALL_CDECL); assert(r >= 0);
	ss.str("");

	ss << alias << " normalize(const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTION(glmVec3Normalise), asCALL_CDECL); assert(r >= 0);
	ss.str("");

	ss << "string toString(const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTIONPR(glmVec3ToString, (const glm::vec3&), std::string), asCALL_CDECL); assert(r >= 0);
	ss.str("");
}

using namespace glm;

void constructGlmVec3(void* pointer)
{
    new(pointer) glm::vec3();
}

void copyConstructGlmVec3(const glm::vec3& other, void* pointer)
{
    new(pointer) glm::vec3(other);
}

void floatConstructGlmVec3(float x, float y, float z, void* pointer)
{
    new(pointer) glm::vec3(x, y, z);
}

void destructGlmVec3(void* pointer)
{
    ((glm::vec3*) pointer)->~vec3();
}

glm::vec3 glmVec3Add(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 + vec2;
}

glm::vec3 glmVec3Sub(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 - vec2;
}

glm::vec3 glmVec3Mul(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 * vec2;
}

glm::vec3 glmVec3Div(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return vec1 / vec2;
}

glm::vec3 glmVec3Mul(const glm::vec3& vec1, float val)
{
    return vec1 * val;
}

glm::vec3 glmVec3Div(const glm::vec3& vec1, float val)
{
    return vec1 / val;
}

float glmVec3Distance(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return glm::distance(vec1, vec2);
}

float glmVec3Length(const glm::vec3& vec)
{
    return glm::length(vec);
}

float glmVec3Dot(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return glm::dot(vec1, vec2);
}

glm::vec3 glmVec3Cross(const glm::vec3& vec1, const glm::vec3& vec2)
{
    return glm::cross(vec1, vec2);
}

glm::vec3 glmVec3Normalise(const glm::vec3& vec)
{
	if(glm::length2(vec) != 0)
		return glm::normalize(vec);
	else 
		return glm::vec3(0,0,0);
}

std::string glmVec3ToString(const glm::vec3& vec)
{
    return glm::to_string(vec);
}
