#include "distancechecker.hpp"

bool DistanceChecker::isWithinDistanceOf(const glm::i64vec3& coord1, const glm::i64vec3& coord2, int32_t range)
{
    return glm::distance(glm::vec3(coord1), glm::vec3(coord2)) <= (float)range;
}
