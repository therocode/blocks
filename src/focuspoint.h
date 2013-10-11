#pragma once
#include <glm/glm.hpp>

struct FocusPoint
{
    FocusPoint(const glm::vec3& pos, float r);
    glm::vec3 position;
    float radius;
};
