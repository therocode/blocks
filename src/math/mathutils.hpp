#pragma once
#include <climits>
#include "blockstd.hpp"

///This struct contains various geometrical types, like AABB, and AABB. currently only contains AABB.
struct AABB 
{
    ///Anchor point is at lower corner of cube, not center.
    float x, y, z;
    float width = 1.0f, height = 1.0f, depth = 1.0f;
    float max(const int& axis) const;
    float min(const int& axis) const;
};

static const float EPSILON = FLT_EPSILON;

using Vec3I = glm::ivec3;
using scalar = double;

float sweepAABB(const AABB a, const AABB b, const glm::vec3 va, const glm::vec3 vb,  glm::ivec3& n);
bool testAABBAABB(const AABB& a, const AABB& b);
//returns bool whether or not AABBs overlap, and smallestPenetration, which is a vector of the smallest offset a needs to move out of b
bool testAABBAABB(const AABB& a, const AABB& b, glm::vec3& smallestPenetration);
void renderDebugAABB(const AABB a, const int color);

