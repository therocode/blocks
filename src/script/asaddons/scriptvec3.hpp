#pragma once
#include <string>
#include <sstream>
#include "../../utilities/glm.hpp"
#include <angelscript.h>

void registerGlmVec3(asIScriptEngine* engine, std::string alias);

void constructGlmVec3(void* pointer);
void floatConstructGlmVec3(float x, float y, float z, void* pointer);
void copyConstructGlmVec3(const glm::vec3& other, void* pointer);
void destructGlmVec3(void* pointer);

glm::vec3 glmVec3Add(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 glmVec3Sub(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 glmVec3Mul(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 glmVec3Div(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 glmVec3Mul(const glm::vec3& vec1, float);
glm::vec3 glmVec3Div(const glm::vec3& vec1, float);

float glmVec3Distance(const glm::vec3& vec1, const glm::vec3& vec2);
float glmVec3Length(const glm::vec3& vec);
float glmVec3Dot(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 glmVec3Cross(const glm::vec3& vec1, const glm::vec3& vec2);
glm::vec3 glmVec3Normalise(const glm::vec3& vec);

std::string glmVec3ToString(const glm::vec3& vec);
