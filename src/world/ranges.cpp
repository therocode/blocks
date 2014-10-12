#include "ranges.hpp"
#include <fea/assert.hpp>

int64_t Ranges::MAX = std::numeric_limits<int64_t>::max();
int64_t Ranges::MIN = std::numeric_limits<int64_t>::min();

Ranges::Ranges() :
    xRange(MIN, MAX),
    yRange(MIN, MAX),
    zRange(MIN, MAX)
{
}

Ranges::Ranges(const IntRange& x, const IntRange& y, const IntRange& z) :
    xRange(x),
    yRange(y),
    zRange(z)
{
    FEA_ASSERT(x.first <= x.second, "Invalid X range in range!");
    FEA_ASSERT(y.first <= y.second, "Invalid Y range in range!");
    FEA_ASSERT(z.first <= z.second, "Invalid Z range in range!");
}

bool Ranges::isWithin(const glm::i64vec3& coordinate) const
{
    return (coordinate.x >= xRange.first && coordinate.x <= xRange.second &&
            coordinate.y >= yRange.first && coordinate.y <= yRange.second &&
            coordinate.z >= zRange.first && coordinate.z <= zRange.second);   
}
