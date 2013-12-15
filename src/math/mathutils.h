#pragma once
#include "blockstd.h"
#include "rendering/renderer.h"

///This struct contains various geometrical types, like AABB, and AABB. currently only contains AABB.
struct AABB
{
    ///Anchor point is at lower corner of cube, not center.
    float x, y, z;
    float width = 1.0f, height = 1.0f, depth = 1.0f;
};

using Vec3I = glm::ivec3;
using scalar = double;

float sweepAABB(const AABB a, const AABB b, const glm::vec3 va, const glm::vec3 vb,  glm::ivec3& n);
bool testAABBAABB(const AABB a, const AABB b);
void renderDebugAABB(const AABB a, const int color);
