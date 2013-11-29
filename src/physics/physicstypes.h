#pragma once
///This struct contains various geometrical types, like AABB, and AABB. currently only contains AABB.
struct AABB
{
    float x, y, z;
    float width = 1.0f, height = 1.0f, depth = 1.0f;
};