#include "scripti64vec3.hpp"
#include <new>

void registerGlmI64Vec3(asIScriptEngine* engine, std::string alias)
{    
    int32_t r = 0;

    std::stringstream ss;

    r = engine->RegisterObjectType(alias.c_str(), sizeof(glm::i64vec3), asOBJ_VALUE|asOBJ_APP_CLASS_CDAK); assert(r >= 0);
    r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, "void f()", asFUNCTION(constructGlmI64Vec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, "void f(int64 x, int64 y, int64 z)", asFUNCTION(int64ConstructGlmI64Vec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );

    ss << "void f(const " << alias << " &in)";
    r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_CONSTRUCT, ss.str().c_str(), asFUNCTION(copyConstructGlmI64Vec3), asCALL_CDECL_OBJLAST); assert( r >= 0 );
    ss.str("");
    r = engine->RegisterObjectBehaviour(alias.c_str(), asBEHAVE_DESTRUCT,  "void f()", asFUNCTION(destructGlmI64Vec3),  asCALL_CDECL_OBJLAST); assert( r >= 0 );

    r = engine->RegisterObjectProperty(alias.c_str(), "int64 x", asOFFSET(glm::i64vec3, x)); assert(r >= 0);
    r = engine->RegisterObjectProperty(alias.c_str(), "int64 y", asOFFSET(glm::i64vec3, y)); assert(r >= 0);
    r = engine->RegisterObjectProperty(alias.c_str(), "int64 z", asOFFSET(glm::i64vec3, z)); assert(r >= 0);

    //r = engine->RegisterObjectMethod("Vec3", "Vec3 opSub(Vec3 vec) const", asMETHODPR(glm::i64vec3, operator-, (const glm::i64vec3&) const, glm::i64vec3), asCALL_THISCALL); assert(r >= 0);

    ss << alias << " opAdd(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmI64Vec3Add, (const glm::i64vec3&, const glm::i64vec3&), glm::i64vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    ss.str("");

    ss << alias << " opSub(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmI64Vec3Sub, (const glm::i64vec3&, const glm::i64vec3&), glm::i64vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    ss.str("");

    ss << alias << " opMul(const " << alias << "& in) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmI64Vec3Mul, (const glm::i64vec3&, const glm::i64vec3&), glm::i64vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    ss.str("");

    ss << alias << " opDiv(const " << alias << "& in) const"; 
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmI64Vec3Div, (const glm::i64vec3&, const glm::i64vec3&), glm::i64vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    ss.str("");

    ss << alias << " opMul(int64 val) const"; 
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmI64Vec3Mul, (const glm::i64vec3&, int64_t), glm::i64vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    ss.str("");

    ss << alias << " opDiv(int64 val) const";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asFUNCTIONPR(glmI64Vec3Div, (const glm::i64vec3&, int64_t), glm::i64vec3), asCALL_CDECL_OBJLAST); assert(r >= 0);
    ss.str("");

    ss << alias << " &opAssign(const " << alias << " &in)";
    r = engine->RegisterObjectMethod(alias.c_str(), ss.str().c_str(), asMETHODPR(glm::i64vec3, operator=, (const glm::i64vec3&), glm::i64vec3&), asCALL_THISCALL); assert( r >= 0 );
    ss.str("");

    ss << "string toString(const " << alias << "& in)";
    r = engine->RegisterGlobalFunction(ss.str().c_str(), asFUNCTIONPR(glmI64Vec3ToString, (const glm::i64vec3&), std::string), asCALL_CDECL); assert(r >= 0);
    ss.str("");
}

using namespace glm;

void constructGlmI64Vec3(void* pointer)
{
    new(pointer) glm::i64vec3();
}

void copyConstructGlmI64Vec3(const glm::i64vec3& other, void* pointer)
{
    new(pointer) glm::i64vec3(other);
}

void int64ConstructGlmI64Vec3(int64_t x, int64_t y, int64_t z, void* pointer)
{
    new(pointer) glm::i64vec3(x, y, z);
}

void destructGlmI64Vec3(void* pointer)
{
    ((glm::i64vec3*) pointer)->~i64vec3();
}

glm::i64vec3 glmI64Vec3Add(const glm::i64vec3& vec1, const glm::i64vec3& vec2)
{
    return vec1 + vec2;
}

glm::i64vec3 glmI64Vec3Sub(const glm::i64vec3& vec1, const glm::i64vec3& vec2)
{
    return vec1 - vec2;
}

glm::i64vec3 glmI64Vec3Mul(const glm::i64vec3& vec1, const glm::i64vec3& vec2)
{
    return vec1 * vec2;
}

glm::i64vec3 glmI64Vec3Div(const glm::i64vec3& vec1, const glm::i64vec3& vec2)
{
    return vec1 / vec2;
}

glm::i64vec3 glmI64Vec3Mul(const glm::i64vec3& vec1, int64_t val)
{
    return vec1 * val;
}

glm::i64vec3 glmI64Vec3Div(const glm::i64vec3& vec1, int64_t val)
{
    return vec1 / val;
}

std::string glmI64Vec3ToString(const glm::i64vec3& vec)
{
    return glm::to_string((glm::ivec3)vec);
}
