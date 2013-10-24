#pragma once
#include "../../blockstd.h"
#include <angelscript.h>

void registerGlmVectors(asIScriptEngine* engine);
void constructVec3(void* pointer);
void copyConstructVec3(const glm::vec3& other, void* pointer);
void destructVec3(void* pointer);
