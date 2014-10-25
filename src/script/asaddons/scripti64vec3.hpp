#pragma once
#include <string>
#include <sstream>
#include "../../utilities/glm.hpp"
#include <angelscript.h>
#include <cstdint>

void registerGlmI64Vec3(asIScriptEngine* engine, std::string alias);

void constructGlmI64Vec3(void* pointer);
void int64ConstructGlmI64Vec3(int64_t x, int64_t y, int64_t z, void* pointer);
void copyConstructGlmI64Vec3(const glm::i64vec3& other, void* pointer);
void destructGlmI64Vec3(void* pointer);

glm::i64vec3 glmI64Vec3Add(const glm::i64vec3& vec1, const glm::i64vec3& vec2);
glm::i64vec3 glmI64Vec3Sub(const glm::i64vec3& vec1, const glm::i64vec3& vec2);
glm::i64vec3 glmI64Vec3Mul(const glm::i64vec3& vec1, const glm::i64vec3& vec2);
glm::i64vec3 glmI64Vec3Div(const glm::i64vec3& vec1, const glm::i64vec3& vec2);
glm::i64vec3 glmI64Vec3Mul(const glm::i64vec3& vec1, int64_t);
glm::i64vec3 glmI64Vec3Div(const glm::i64vec3& vec1, int64_t);

std::string glmI64Vec3ToString(const glm::i64vec3& vec);
