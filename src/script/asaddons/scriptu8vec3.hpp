#pragma once
#include <string>
#include <sstream>
#include "../../utilities/glm.hpp"
#include <angelscript.h>
#include <cstdint>

void registerGlmU8Vec3(asIScriptEngine* engine, std::string alias);

void constructGlmU8Vec3(void* pointer);
void uint8ConstructGlmU8Vec3(uint8_t x, uint8_t y, uint8_t z, void* pointer);
void copyConstructGlmU8Vec3(const glm::u8vec3& other, void* pointer);
void destructGlmU8Vec3(void* pointer);

glm::u8vec3 glmU8Vec3Add(const glm::u8vec3& vec1, const glm::u8vec3& vec2);
glm::u8vec3 glmU8Vec3Sub(const glm::u8vec3& vec1, const glm::u8vec3& vec2);
glm::u8vec3 glmU8Vec3Mul(const glm::u8vec3& vec1, const glm::u8vec3& vec2);
glm::u8vec3 glmU8Vec3Div(const glm::u8vec3& vec1, const glm::u8vec3& vec2);
glm::u8vec3 glmU8Vec3Mul(const glm::u8vec3& vec1, uint8_t);
glm::u8vec3 glmU8Vec3Div(const glm::u8vec3& vec1, uint8_t);

std::string glmU8Vec3ToString(const glm::u8vec3& vec);
