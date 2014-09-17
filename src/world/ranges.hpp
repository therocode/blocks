#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

using IntRange = std::pair<int32_t, int32_t>;

struct Ranges
{
    Ranges();
    static int32_t MAX;
    static int32_t MIN;
    Ranges(const IntRange& x, const IntRange& y, const IntRange& z);
    bool isWithin(const glm::ivec3& coordinate) const;
    IntRange xRange;
    IntRange yRange;
    IntRange zRange;
};
