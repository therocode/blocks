#pragma once
#include <string>
#include "../../blockstd.hpp"
#include <angelscript.h>

void registerGlmVectors(asIScriptEngine* engine);
void constructVec3(void* pointer);
void floatConstructVec3(float x, float y, float z, void* pointer);
void copyConstructVec3(const glm::vec3& other, void* pointer);
void destructVec3(void* pointer);

glm::vec3 vec3Add(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 vec3Sub(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 vec3Mul(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 vec3Div(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 vec3Mul(const glm::vec3& vec1, float);
glm::vec3 vec3Div(const glm::vec3& vec1, float);

float vec3Distance(const glm::vec3& vec1, const glm::vec3& vec2);
float vec3Length(const glm::vec3& vec);
float vec3Dot(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 vec3Cross(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 vec3Normalise(const glm::vec3& vec);

std::string vec3ToString(const glm::vec3& vec);
