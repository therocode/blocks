#pragma once
#include <cstdint>
#include <glm/glm.hpp>

class DistanceChecker
{
    public:
        static bool isWithinDistanceOf(const glm::i64vec3& coord1, const glm::i64vec3& coord2, int32_t range);
};
