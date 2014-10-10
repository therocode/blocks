#pragma once
#include <unordered_map>
#include <glm/glm.hpp>

using IntRange = std::pair<int64_t, int64_t>;

struct Ranges
{
    Ranges();
    static int64_t MAX;
    static int64_t MIN;
    Ranges(const IntRange& x, const IntRange& y, const IntRange& z);
    bool isWithin(const glm::i64vec3& coordinate) const;
    IntRange xRange;
    IntRange yRange;
    IntRange zRange;
};
